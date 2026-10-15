import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;

// args: <outfile> <start> <end>  creates missing functions (after nop padding) and decompiles all in range
public class DecompRange extends GhidraScript {
    public void run() throws Exception {
        String[] a = getScriptArgs();
        Address s = toAddr(a[1]), e = toAddr(a[2]);
        Memory mem = currentProgram.getMemory();
        boolean pad = true;
        for (Address p = s; p.compareTo(e) < 0; p = p.add(1)) {
            int b = mem.getByte(p) & 0xff;
            boolean isPad = (b == 0x90 || b == 0xcc);
            if (pad && !isPad && getFunctionContaining(p) == null) {
                disassemble(p);
                createFunction(p, null);
            }
            pad = isPad;
            Function f = getFunctionContaining(p);
            if (f != null) { p = f.getBody().getMaxAddress(); pad = false; }
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        for (Function f : currentProgram.getFunctionManager().getFunctions(s, true)) {
            if (f.getEntryPoint().compareTo(e) > 0) break;
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n");
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0]), sb.toString());
        println("wrote " + a[0]);
    }
}
