//Define enums from names/MFL_enums.txt in /Mashed and apply them to function return types / parameters. Args: <spec file>
//  enum,<Name>,<size>   v,<value>,<NAME>   ret,<function>,<Enum>   param,<function>,<index>,<Enum>
//@category Mashed
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyEnums extends GhidraScript {
    @Override
    protected void run() throws Exception {
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        Map<String, EnumDataType> made = new LinkedHashMap<>();
        List<String[]> uses = new ArrayList<>();
        EnumDataType cur = null;
        for (String line : Files.readAllLines(Path.of(getScriptArgs()[0]))) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] p = line.split(",");
            switch (p[0]) {
                case "enum": cur = new EnumDataType(new CategoryPath("/Mashed"), p[1], Integer.parseInt(p[2])); made.put(p[1], cur); break;
                case "v": cur.add(p[2], Long.decode(p[1]) & 0xFFFFFFFFL); break;
                default: uses.add(p);
            }
        }
        Map<String, DataType> res = new HashMap<>();
        for (var e : made.entrySet()) res.put(e.getKey(), dtm.resolve(e.getValue(), DataTypeConflictHandler.REPLACE_HANDLER));
        println("enums defined: " + res.size());
        int n = 0;
        for (String[] u : uses) {
            List<Function> fs = currentProgram.getListing().getGlobalFunctions(u[1]);
            if (fs.isEmpty()) { printerr("no function " + u[1]); continue; }
            Function f = fs.get(0);
            if (u[0].equals("ret")) f.setReturnType(res.get(u[2]), SourceType.USER_DEFINED);
            else {
                Parameter pa = f.getParameter(Integer.parseInt(u[2]));
                if (pa == null) { printerr("no param " + u[2] + " in " + u[1]); continue; }
                pa.setDataType(res.get(u[3]), SourceType.USER_DEFINED);
            }
            n++;
        }
        println("enum uses applied: " + n);
    }
}
