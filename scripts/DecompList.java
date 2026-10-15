//Decompile the functions at the addresses of a list file (first column hex VA) into the `// ==== name @ va` format t2_auto.py --text reads. Creates a missing function at the address (disassemble + createFunction).
//Args: <listfile> <outfile>   (run in a scratch COPY of the project with -readOnly -noanalysis: nothing is saved)
//@category Mashed
import java.io.*;
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;

public class DecompList extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        StringBuilder sb = new StringBuilder();
        int n = 0, bad = 0;
        for (String line : Files.readAllLines(Path.of(a[0]))) {
            String[] p = line.trim().split("\\s+");
            if (p.length == 0 || p[0].isEmpty()) continue;
            Address ad = toAddr(p[0]);
            Function f = getFunctionAt(ad);
            if (f == null) { disassemble(ad); f = createFunction(ad, null); }
            if (f == null) { bad++; continue; }
            DecompileResults r = di.decompileFunction(f, 30, monitor);
            sb.append("// ==== ").append(f.getName()).append(" @ ").append(f.getEntryPoint()).append("\n");
            if (r.decompileCompleted()) sb.append(r.getDecompiledFunction().getC()).append("\n"); else bad++;
            n++;
        }
        Files.writeString(Path.of(a[1]), sb.toString());
        println("wrote " + n + " functions (" + bad + " failed) to " + a[1]);
    }
}
