//List the most referenced DAT_* globals: address, references, reads, writes, referencing functions (named ones first). Args: <outfile> [min refs]
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class GlobalIndex extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        int min = a.length > 1 ? Integer.parseInt(a[1]) : 8;
        List<String[]> rows = new ArrayList<>();
        SymbolIterator it = currentProgram.getSymbolTable().getAllSymbols(true);
        while (it.hasNext()) {
            Symbol s = it.next();
            if (!s.getName().startsWith(a.length > 2 ? a[2] : "DAT_") || !s.isPrimary()) continue;
            int reads = 0, writes = 0;
            LinkedHashSet<String> fns = new LinkedHashSet<>();
            for (Reference r : currentProgram.getReferenceManager().getReferencesTo(s.getAddress())) {
                if (r.getReferenceType().isWrite()) writes++; else if (r.getReferenceType().isRead()) reads++;
                Function f = getFunctionContaining(r.getFromAddress());
                if (f != null && fns.size() < 40) fns.add(f.getName());
            }
            if (reads + writes < min) continue;
            List<String> named = new ArrayList<>(), other = new ArrayList<>();
            for (String f : fns) (f.startsWith("FUN_") ? other : named).add(f);
            named.addAll(other);
            rows.add(new String[]{String.format("%08x", s.getAddress().getOffset()) + " " + s.getName(), String.valueOf(reads + writes), String.valueOf(reads), String.valueOf(writes),
                String.join(",", named.subList(0, Math.min(6, named.size())))});
        }
        rows.sort((x, y) -> Integer.parseInt(y[1]) - Integer.parseInt(x[1]));
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            for (String[] r : rows) w.println(String.join("\t", r));
        }
        println("globals " + rows.size());
    }
}
