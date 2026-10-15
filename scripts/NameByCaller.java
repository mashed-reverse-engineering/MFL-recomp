//Rename auto named functions (<prefix>_fn_<addr>) that have exactly one calling function, after that caller: <caller>_sub_<addr>.
//Args: [<startHex> <endHex>] (default 401000..4b0000). Repeats until nothing changes, so chains of helpers all get the root caller's name.
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class NameByCaller extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Address lo = toAddr(Long.decode("0x" + (a.length > 0 ? a[0] : "401000"))), hi = toAddr(Long.decode("0x" + (a.length > 1 ? a[1] : "4b0000")));
        FunctionManager fm = currentProgram.getFunctionManager();
        ReferenceManager rm = currentProgram.getReferenceManager();
        int total = 0, round = 0, n;
        do {
            n = 0;
            round++;
            for (Function f : fm.getFunctions(lo, true)) {
                if (f.getEntryPoint().compareTo(hi) >= 0) break;
                if (!f.getName().contains("_fn_")) continue;
                Set<Function> callers = new HashSet<>();
                boolean dataRef = false;
                for (Reference r : rm.getReferencesTo(f.getEntryPoint())) {
                    if (r.getReferenceType().isCall() || r.getReferenceType().isJump()) {
                        Function c = fm.getFunctionContaining(r.getFromAddress());
                        if (c != null && !c.equals(f)) callers.add(c);
                    } else dataRef = true;
                }
                if (dataRef || callers.size() != 1) continue;
                Function c = callers.iterator().next();
                String cn = c.getName();
                if (cn.contains("_fn_") || cn.startsWith("FUN_")) continue;
                int i = cn.indexOf("_sub_");
                String root = i >= 0 ? cn.substring(0, i) : cn;
                f.setName(root + "_sub_" + Long.toHexString(f.getEntryPoint().getOffset()), ghidra.program.model.symbol.SourceType.USER_DEFINED);
                n++;
            }
            total += n;
        } while (n > 0 && round < 12);
        println("renamed " + total + " functions in " + round + " rounds");
    }
}
