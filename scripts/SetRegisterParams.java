//Give every function listed in a CSV (name,addr,reg_inputs,ret_n; tools/reginputs.py) custom storage register parameters, so that the decompiler shows the register
//values at the call sites.  Args: <csv>.  Register parameters come first (reg_eax, reg_ecx, ...), the stack / register parameters Ghidra already has are kept.
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.data.*;
import ghidra.program.model.lang.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class SetRegisterParams extends GhidraScript {
    private Map<String, DataType> cache = new HashMap<>();
    private DataType typeByName(String t) {
        int stars = 0;
        while (t.endsWith("*")) { stars++; t = t.substring(0, t.length() - 1).trim(); }
        t = t.replace("const ", "").trim();
        DataType base = cache.get(t);
        if (base == null) {
            if (t.equals("float")) base = FloatDataType.dataType;
            else if (t.equals("int")) base = IntegerDataType.dataType;
            else if (t.equals("void") || t.isEmpty()) base = VoidDataType.dataType;
            else {
                Iterator<DataType> it = currentProgram.getDataTypeManager().getAllDataTypes();
                while (it.hasNext()) { DataType d = it.next(); if (d.getName().equals(t)) { base = d; break; } }
            }
            if (base == null) base = Undefined4DataType.dataType;
            cache.put(t, base);
        }
        DataType r = base;
        for (int i = 0; i < stars; i++) r = new PointerDataType(r);
        if (stars == 0 && r.getLength() != 4 && !(r instanceof VoidDataType)) r = Undefined4DataType.dataType;   // keep the 4 byte slots
        return r;
    }

    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        int done = 0, skipped = 0, failed = 0;
        try (BufferedReader br = new BufferedReader(new FileReader(a[0]))) {
            String line = br.readLine();
            while ((line = br.readLine()) != null) {
                String[] p = line.split(",");
                if (p.length < 4 || p[2].replace("|", "").isEmpty()) continue;
                Function f = getFunctionAt(toAddr(Long.decode("0x" + p[1])));
                if (f == null) { skipped++; continue; }
                List<Variable> params = new ArrayList<>();
                for (String r : p[2].split("\\|")) {
                    String R = r.toUpperCase();
                    Register reg = currentProgram.getRegister(R);
                    if (R.isEmpty() || reg == null) continue;
                    params.add(new ParameterImpl("reg_" + r, Undefined4DataType.dataType, reg, currentProgram));
                }
                if (params.isEmpty()) { skipped++; continue; }
                if (p.length > 4 && !p[4].isEmpty()) {          // stack parameters (types of the decompilation made before this script), 4 bytes each
                    int k = 1;
                    for (String t : p[4].split(";")) {
                        params.add(new ParameterImpl("param_" + k, typeByName(t.trim()), 4 * k, currentProgram));
                        k++;
                    }
                }
                try {
                    f.setCustomVariableStorage(true);
                    f.updateFunction(null, null, params, Function.FunctionUpdateType.CUSTOM_STORAGE, true, SourceType.USER_DEFINED);
                    done++;
                } catch (Exception e) { failed++; println("failed " + p[0] + ": " + e.getMessage()); }
            }
        }
        println("register parameters set: " + done + ", skipped " + skipped + ", failed " + failed);
    }
}
