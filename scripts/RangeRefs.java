//List (and decompile) functions that reference addresses in [start,end). Args: <outfile> <startHex> <endHex>
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.Reference;

public class RangeRefs extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        long s = Long.parseLong(a[1], 16), e = Long.parseLong(a[2], 16);
        Map<Function, TreeSet<String>> fns = new TreeMap<>(Comparator.comparing(Function::getEntryPoint));
        for (Instruction ins : currentProgram.getListing().getInstructions(true))
            for (Reference r : ins.getReferencesFrom()) {
                long t = r.getToAddress().getOffset();
                if (t >= s && t < e && r.getToAddress().isMemoryAddress()) {
                    Function f = getFunctionContaining(ins.getAddress());
                    if (f != null) fns.computeIfAbsent(f, x -> new TreeSet<>()).add(String.format("+0x%x", t - s));
                }
            }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        for (Map.Entry<Function, TreeSet<String>> en : fns.entrySet()) {
            Function f = en.getKey();
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("  offsets: ").append(en.getValue()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0]), sb.toString());
        println("wrote " + a[0] + " " + fns.size() + " functions");
    }
}
