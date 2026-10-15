//Write every non-builtin data type of the program (structs, unions, enums, typedefs) as a C header. Args: <outfile>
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;

public class ExportTypes extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        List<DataType> list = new ArrayList<>();
        Iterator<DataType> it = dtm.getAllDataTypes();
        while (it.hasNext()) {
            DataType dt = it.next();
            if (dt instanceof BuiltInDataType || dt instanceof Pointer || dt instanceof Array || dt instanceof FunctionDefinition) continue;
            if (dt instanceof Structure || dt instanceof Union || dt instanceof ghidra.program.model.data.Enum || dt instanceof TypeDef) list.add(dt);
        }
        try (Writer w = new FileWriter(a[0])) {
            DataTypeWriter dw = new DataTypeWriter(dtm, w);
            dw.write(list, monitor);
        }
        println("wrote " + list.size() + " types");
    }
}
