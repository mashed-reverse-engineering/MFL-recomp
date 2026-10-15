//Write decompiled C (<base>.c) and the Ghidra listing (<base>.asm) of the functions containing the given addresses.
//Args: <outBase> <addr>...   (run after RenameFns.java so the names show up)
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;

public class DumpFns extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Set<Function> fns = new TreeSet<>(Comparator.comparing(Function::getEntryPoint));
        for (int i = 1; i < a.length; i++) {
            Function f = getFunctionContaining(toAddr(a[i]));
            if (f == null) { disassemble(toAddr(a[i])); f = createFunction(toAddr(a[i]), null); }
            if (f != null) fns.add(f);
        }
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder c = new StringBuilder(), asm = new StringBuilder();
        for (Function f : fns) {
            String head = "// ==== " + f.getName() + " @ " + f.getEntryPoint() + "\n";
            c.append(head);
            DecompileResults r = di.decompileFunction(f, 90, monitor);
            if (r.decompileCompleted()) c.append(r.getDecompiledFunction().getC());
            c.append("\n");
            asm.append("; ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("\n");
            if (f.getComment() != null) asm.append("; ").append(f.getComment()).append("\n");
            for (Instruction ins : currentProgram.getListing().getInstructions(f.getBody(), true)) {
                String eol = ins.getComment(CodeUnit.EOL_COMMENT);
                asm.append(String.format("%s  %-40s%s%n", ins.getAddress(), ins.toString(), eol == null ? "" : "; " + eol));
            }
            asm.append("\n");
        }
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0] + ".c"), c.toString());
        java.nio.file.Files.writeString(java.nio.file.Path.of(a[0] + ".asm"), asm.toString());
        println("wrote " + a[0] + ".{c,asm} " + fns.size() + " functions");
    }
}
