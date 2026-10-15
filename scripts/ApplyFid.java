//Apply a FID database to the current program: rename functions whose FID matches agree on one name.
//Script args: <fidb file> <csv out>
//@category FunctionID
import java.io.*;
import java.util.*;

import ghidra.app.script.GhidraScript;
import ghidra.feature.fid.db.*;
import ghidra.feature.fid.service.*;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class ApplyFid extends GhidraScript {
    private static String clean(String n) {
        if (n.startsWith("_") || n.startsWith("@")) n = n.substring(1);
        int at = n.lastIndexOf('@');
        if (at > 0 && n.substring(at + 1).matches("\\d+")) n = n.substring(0, at);
        return n;
    }

    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        File fidb = new File(a[0]);
        FidFileManager.getInstance().addUserFidFile(fidb);
        FidService service = new FidService();
        FidQueryService q = service.openFidQueryService(currentProgram.getLanguage(), false);
        int renamed = 0, ambiguous = 0, kept = 0, matched = 0;
        Map<String, Integer> byLib = new TreeMap<>();
        try (PrintWriter csv = new PrintWriter(new FileWriter(a[1]))) {
            csv.println("address,old_name,new_name,library,score");
            List<FidSearchResult> results = service.processProgram(currentProgram, q, service.getDefaultScoreThreshold(), monitor);
            if (results == null) results = new ArrayList<>();
            for (FidSearchResult r : results) {
                if (r.matches == null || r.matches.isEmpty()) continue;
                matched++;
                float best = -1;
                for (FidMatch m : r.matches) best = Math.max(best, m.getOverallScore());
                Set<String> names = new TreeSet<>();
                String lib = "";
                for (FidMatch m : r.matches)
                    if (m.getOverallScore() >= best - 0.001f) {
                        names.add(clean(m.getFunctionRecord().getName()));
                        lib = m.getLibraryRecord().getLibraryFamilyName() + " " + m.getLibraryRecord().getLibraryVersion();
                    }
                if (names.size() != 1) { ambiguous++; csv.println(r.function.getEntryPoint() + "," + r.function.getName() + ",\"" + String.join("|", names) + "\",ambiguous," + best); continue; }
                String nn = names.iterator().next();
                Function f = r.function;
                if (!f.getName().startsWith("FUN_") && !f.getName().startsWith("thunk_FUN_")) { kept++; continue; }
                f.setName(nn, SourceType.ANALYSIS);
                renamed++;
                byLib.merge(lib, 1, Integer::sum);
                csv.println(f.getEntryPoint() + "," + "FUN," + nn + "," + lib + "," + best);
            }
        }
        println("functions with FID matches: " + matched + ", renamed: " + renamed + ", ambiguous: " + ambiguous + ", already named: " + kept);
        println("by library: " + byLib);
    }
}
