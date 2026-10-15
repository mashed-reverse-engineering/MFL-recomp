//Declare the functions that pop their own arguments (`ret N`, tools/stdcall_scan.py -> output/stdcall.txt: "<hex address> <bytes>") as __stdcall with N/4 parameters, so the decompiler
//stops adding `add esp,N` after calls to them and writes the right argument count.  Functions with custom register storage (register parameters) are left alone.  Args: <file>
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class SetStdcall extends GhidraScript {
    @Override
    protected void run() throws Exception {
        int done = 0, same = 0, skipped = 0, failed = 0;
        try (BufferedReader br = new BufferedReader(new FileReader(getScriptArgs()[0]))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] p = line.trim().split("\\s+");
                if (p.length < 2) continue;
                Function f = getFunctionAt(toAddr(Long.decode("0x" + p[0])));
                int want = Integer.parseInt(p[1]) / 4;
                if (f == null || f.hasCustomVariableStorage() || f.isThunk()) { skipped++; continue; }
                String cc = f.getCallingConventionName();
                if (cc != null && (cc.contains("fastcall") || cc.contains("thiscall"))) { skipped++; continue; }
                try {
                    List<Variable> params = new ArrayList<>();
                    Parameter[] cur = f.getParameters();
                    for (int i = 0; i < want; i++) {
                        if (i < cur.length) params.add(new ParameterImpl(cur[i].getName(), cur[i].getDataType(), currentProgram));
                        else params.add(new ParameterImpl("param_" + (i + 1), Undefined4DataType.dataType, currentProgram));
                    }
                    if (cur.length == want && "__stdcall".equals(cc)) { same++; continue; }
                    f.updateFunction("__stdcall", null, params, Function.FunctionUpdateType.DYNAMIC_STORAGE_FORMAL_PARAMS, true, SourceType.USER_DEFINED);
                    done++;
                } catch (Exception e) { failed++; println("failed " + p[0] + ": " + e.getMessage()); }
            }
        }
        println("stdcall set: " + done + ", already ok " + same + ", skipped " + skipped + ", failed " + failed);
    }
}
