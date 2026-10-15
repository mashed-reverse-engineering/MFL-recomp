//Export the signature of every function in an address range with the storage of each parameter and of the return value.
//Args: <outfile> <startHex> <endHex>.  One line per function: name|addr|callconv|ret type|ret storage|p1 type:p1 name:p1 storage;...|purge
//Storage strings are Ghidra's ("ECX:4", "Stack[0x4]:4", "ST0:10", "EDX:EAX").
//@category Mashed
import java.io.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;

public class ExportSigs extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Address lo = toAddr(Long.decode("0x" + a[1])), hi = toAddr(Long.decode("0x" + a[2]));
        int n = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            for (Function f : currentProgram.getListing().getFunctions(lo, true)) {
                if (f.getEntryPoint().compareTo(hi) >= 0) break;
                StringBuilder sb = new StringBuilder();
                for (Parameter p : f.getParameters()) {
                    sb.append(p.getDataType().getName()).append(':').append(p.getName()).append(':').append(p.getVariableStorage().toString()).append(';');
                }
                String rs = f.getReturn().getVariableStorage().toString();
                w.println(f.getName() + "|" + f.getEntryPoint() + "|" + f.getCallingConventionName() + "|" + f.getReturnType().getName() + "|" + rs + "|" + sb + "|" + f.getStackPurgeSize());
                n++;
            }
        }
        println("wrote " + n + " signatures");
    }
}
