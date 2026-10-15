//Rename default-named locals (iVar3, uVar1, pcVar2, ...) after the named function whose result they hold.
//Args: <csv: callee name,variable name> [<startHex> <endHex>]   (range default 401000..4b0000)
//A local keeps its default name when assigned from several different mapped callees; duplicates in one function get _2, _3, ...
//@category Mashed
import java.nio.file.*;
import java.util.*;
import java.util.regex.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.Function;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.SourceType;

public class AutoVarNames extends GhidraScript {
    static final Pattern DEFAULT = Pattern.compile("^[a-z]*Var\\d+$");

    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Map<String, String> map = new HashMap<>();
        for (String line : Files.readAllLines(Path.of(a[0]))) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] p = line.split(",");
            map.put(p[0], p[1]);
        }
        Address lo = toAddr(Long.decode("0x" + (a.length > 1 ? a[1] : "401000"))), hi = toAddr(Long.decode("0x" + (a.length > 2 ? a[2] : "4b0000")));
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
                if (op.getOpcode() != PcodeOp.CALL || op.getOutput() == null) continue;
                Function callee = getFunctionAt(op.getInput(0).getAddress());
                if (callee == null) continue;
                String base = map.get(callee.getName());
                if (base == null) continue;
                HighVariable hv = op.getOutput().getHigh();
                if (hv == null) continue;
                HighSymbol hs = hv.getSymbol();
                if (hs == null || hs.isParameter() || !DEFAULT.matcher(hs.getName()).matches()) continue;
                cand.computeIfAbsent(hs, k -> new LinkedHashSet<>()).add(base);
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
                } catch (Exception ex) { /* register variables that cannot be committed are skipped */ }
            }
        }
        println("renamed " + renamed + " locals in " + fns + " functions");
    }
}
