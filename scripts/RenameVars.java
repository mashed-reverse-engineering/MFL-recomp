//Rename local variables / parameters in decompiled functions. Args: <csv>
//  <function address>,<old name as shown by the decompiler>,<new name>[,<type: int uint float char* ...>]
//Names come from the decompiler's own numbering (iVar2, local_c, param_1), so apply on a fresh analysis only once.
//@category Mashed
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.Function;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.SourceType;

public class RenameVars extends GhidraScript {
    @Override
    protected void run() throws Exception {
        Map<String, List<String[]>> by = new LinkedHashMap<>();
        for (String line : Files.readAllLines(Path.of(getScriptArgs()[0]))) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] p = line.split(",");
            by.computeIfAbsent(p[0], k -> new ArrayList<>()).add(p);
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        int done = 0, missed = 0;
        for (Map.Entry<String, List<String[]>> e : by.entrySet()) {
            Function f = getFunctionAt(toAddr(e.getKey()));
            if (f == null) { printerr("no function at " + e.getKey()); continue; }
            HighFunction hf = di.decompileFunction(f, 60, monitor).getHighFunction();
            if (hf == null) { printerr("decompile failed " + e.getKey()); continue; }
            for (String[] p : e.getValue()) {
                HighSymbol hs = null;
                Iterator<HighSymbol> it = hf.getLocalSymbolMap().getSymbols();
                while (it.hasNext()) { HighSymbol s = it.next(); if (s.getName().equals(p[1])) hs = s; }
                if (hs == null) { printerr(e.getKey() + ": no variable " + p[1]); missed++; continue; }
                ghidra.program.model.data.DataType dt = null;
                if (p.length > 3) {
                    String tn = p[3];
                    boolean ptr = tn.endsWith("*");
                    if (ptr) tn = tn.substring(0, tn.length() - 1);
                    dt = currentProgram.getDataTypeManager().getDataType("/Mashed/" + tn);
                    if (dt == null) dt = currentProgram.getDataTypeManager().getDataType("/" + tn);
                    if (dt == null) dt = ghidra.program.model.data.BuiltInDataTypeManager.getDataTypeManager().getDataType("/" + tn);
                    if (dt != null && ptr) dt = new ghidra.program.model.data.PointerDataType(dt);
                }
                HighFunctionDBUtil.updateDBVariable(hs, p[2], dt, SourceType.USER_DEFINED);
                done++;
            }
        }
        println("renamed " + done + ", missed " + missed);
    }
}
