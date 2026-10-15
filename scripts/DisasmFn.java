//Print the disassembly listing of the function(s) at the given addresses. Args: <outfile> <hex address>...
//@category Mashed
import java.io.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class DisasmFn extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            for (int i = 1; i < a.length; i++) {
                Function f = getFunctionAt(toAddr(Long.decode("0x" + a[i])));
                if (f == null) { w.println("; no function at " + a[i]); continue; }
                w.println("; ==== " + f.getName() + " @ " + f.getEntryPoint());
                InstructionIterator it = currentProgram.getListing().getInstructions(f.getBody(), true);
                while (it.hasNext()) {
                    Instruction ins = it.next();
                    StringBuilder sb = new StringBuilder(ins.getAddress().toString()).append("  ").append(ins.toString());
                    for (Reference r : ins.getReferencesFrom()) {
                        Symbol s = getSymbolAt(r.getToAddress());
                        if (s != null && r.getReferenceType().isCall()) sb.append("   ; -> ").append(s.getName());
                    }
                    w.println(sb);
                }
            }
        }
    }
}
