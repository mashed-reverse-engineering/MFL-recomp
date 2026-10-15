//Decompile functions that use any of the given scalar constants. Args: <outfile> <hexvalue>...
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;

public class ScalarRefs extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Set<Long> vals = new HashSet<>();
        for (int i = 1; i < a.length; i++) vals.add(Long.parseLong(a[i], 16));
        Map<Function, List<String>> fns = new TreeMap<>(Comparator.comparing(Function::getEntryPoint));
        for (Instruction ins : currentProgram.getListing().getInstructions(true)) {
            for (int k = 0; k < ins.getNumOperands(); k++)
                for (Object o : ins.getOpObjects(k))
                    if (o instanceof Scalar && vals.contains(((Scalar) o).getUnsignedValue())) {
                        Function f = getFunctionContaining(ins.getAddress());
                        if (f != null) fns.computeIfAbsent(f, x -> new ArrayList<>()).add(ins.getAddress() + " " + ins);
                    }
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        for (Map.Entry<Function, List<String>> e : fns.entrySet()) {
            Function f = e.getKey();
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("  uses: ").append(e.getValue()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0]), sb.toString());
        println("wrote " + a[0] + " " + fns.size() + " functions");
    }
}
