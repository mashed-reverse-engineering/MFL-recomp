//Decompile every function with a real name (not FUN_*) in an address range into one C file. Args: <outfile> <startHex> <endHex>
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;

public class DumpNamedRange extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Address lo = toAddr(Long.decode("0x" + a[1])), hi = toAddr(Long.decode("0x" + a[2]));
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        int n = 0, bad = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            w.println("// Decompilation of the named functions in " + a[1] + ".." + a[2] + " of " + currentProgram.getName() + " (Ghidra, names and structs from ghidra/names)\n");
            for (Function f : currentProgram.getListing().getFunctions(lo, true)) {
                if (f.getEntryPoint().compareTo(hi) >= 0) break;
                if (f.getName().startsWith("FUN_") || f.getName().startsWith("thunk_FUN_")) continue;
                DecompileResults r = di.decompileFunction(f, 60, monitor);
                w.println("// ==== " + f.getName() + " @ " + f.getEntryPoint());
                if (r.decompileCompleted()) { w.println(r.getDecompiledFunction().getC()); n++; }
                else { w.println("// decompilation failed: " + r.getErrorMessage() + "\n"); bad++; }
            }
        }
        println("wrote " + n + " functions, failed " + bad);
    }
}
