//Give parameters struct pointer types. Args: <csv> with lines `fn,param_index(0-based),type[,new_name]` (fn = function name or 8 hex digit address; type e.g. `Car*`).
//Only the data type (and optionally the name) of an existing parameter changes; calling convention, parameter count and storage stay.
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class SetParamTypes extends GhidraScript {
    @Override
    protected void run() throws Exception {
        int done = 0, failed = 0;
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        try (BufferedReader br = new BufferedReader(new FileReader(getScriptArgs()[0]))) {
            String line;
            while ((line = br.readLine()) != null) {
                if (line.startsWith("#") || line.trim().isEmpty()) continue;
                String[] p = line.trim().split(",");
                Function f = null;
                if (p[0].matches("[0-9a-fA-F]{8}")) f = getFunctionAt(toAddr(Long.decode("0x" + p[0])));
                else { List<Function> l = getGlobalFunctions(p[0]); if (l.size() == 1) f = l.get(0); }
                String tn = p[2].replace("*", "").trim();
                List<DataType> dts = new ArrayList<>();
                dtm.findDataTypes(tn, dts);
                int i = Integer.parseInt(p[1]);
                if (f == null || dts.isEmpty() || i >= f.getParameterCount()) { failed++; println("skip " + line); continue; }
                try {
                    Parameter par = f.getParameter(i);
                    DataType dt = dts.get(0);
                    par.setDataType(p[2].contains("*") ? new PointerDataType(dt) : dt, SourceType.USER_DEFINED);
                    if (p.length > 3 && !p[3].isEmpty()) par.setName(p[3], SourceType.USER_DEFINED);
                    done++;
                } catch (Exception e) { failed++; println("failed " + line + ": " + e.getMessage()); }
            }
        }
        println("param types set: " + done + ", failed " + failed);
    }
}
