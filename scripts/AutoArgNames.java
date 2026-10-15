//Rename default-named parameters / locals (param_1, iVar3, pcVar2, ...) after the parameter name of the named callee they are passed to.
//Args: [<startHex> <endHex>]   (range default 401000..4b0000). A variable keeps its name when different callees give different names.
//@category Mashed
import java.util.*;
import java.util.regex.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.SourceType;

public class AutoArgNames extends GhidraScript {
    static final Pattern DEFAULT = Pattern.compile("^([a-z]*Var\\d+|param_\\d+)$");

    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Address lo = toAddr(Long.decode("0x" + (a.length > 0 ? a[0] : "401000"))), hi = toAddr(Long.decode("0x" + (a.length > 1 ? a[1] : "4b0000")));
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        int fns = 0, renamed = 0;
        for (Function f : currentProgram.getListing().getFunctions(lo, true)) {
            if (f.getEntryPoint().compareTo(hi) >= 0) break;
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            HighFunction hf = r.getHighFunction();
            if (hf == null) continue;
            Map<HighSymbol, Set<String>> cand = new LinkedHashMap<>();
            Iterator<PcodeOpAST> ops = hf.getPcodeOps();
            while (ops.hasNext()) {
                PcodeOpAST op = ops.next();
                if (op.getOpcode() != PcodeOp.CALL) continue;
                Function callee = getFunctionAt(op.getInput(0).getAddress());
                if (callee == null) continue;
                Parameter[] ps = callee.getParameters();
                for (int i = 1; i < op.getNumInputs() && i <= ps.length; i++) {
                    String pn = ps[i - 1].getName();
                    if (pn == null || pn.length() < 3 || pn.matches("param_\\d+") || pn.matches("^[a-z]*Var\\d+$") || pn.startsWith("local_")) continue;
                    HighVariable hv = op.getInput(i).getHigh();
                    if (hv == null) continue;
                    HighSymbol hs = hv.getSymbol();
                    if (hs == null || !DEFAULT.matcher(hs.getName()).matches()) continue;
                    cand.computeIfAbsent(hs, k -> new LinkedHashSet<>()).add(pn);
                }
            }
            if (cand.isEmpty()) continue;
            Set<String> used = new HashSet<>();
            Iterator<HighSymbol> it = hf.getLocalSymbolMap().getSymbols();
            while (it.hasNext()) used.add(it.next().getName());
            fns++;
            for (Map.Entry<HighSymbol, Set<String>> e : cand.entrySet()) {
                if (e.getValue().size() != 1) continue;
                String base = e.getValue().iterator().next(), name = base;
                for (int i = 2; used.contains(name); i++) name = base + "_" + i;
                try {
                    HighFunctionDBUtil.updateDBVariable(e.getKey(), name, null, SourceType.USER_DEFINED);
                    used.add(name);
                    renamed++;
                } catch (Exception ex) { /* variables that cannot be committed are skipped */ }
            }
        }
        println("renamed " + renamed + " variables in " + fns + " functions");
    }
}
