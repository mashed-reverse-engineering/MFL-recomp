//Rebuild /Mashed/RwGlobalsRelease from the SDK RwGlobals (RWDEBUG-only members dropped) and keep the pointer at RwEngineInstance (0x7D2128).
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;

public class ApplyRwGlobals extends GhidraScript {
    @Override
    protected void run() throws Exception {
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        List<DataType> f = new ArrayList<>();
        dtm.findDataTypes("RwGlobals", f);
        Structure src = null;
        for (DataType d : f) if (d instanceof Structure && d.getLength() > 0) src = (Structure) d;
        if (src == null) { printerr("RwGlobals not found"); return; }
        Set<String> skip = Set.of("debugFunction", "debugStackDepth", "debugTrace", "freeListExtraDebug");
        StructureDataType s = new StructureDataType(new CategoryPath("/Mashed"), "RwGlobalsRelease", 0);
        for (DataTypeComponent c : src.getDefinedComponents()) {
            println(String.format("src +0x%x %s %s", c.getOffset(), c.getFieldName(), c.getDataType().getName()));
            if (skip.contains(c.getFieldName())) continue;
            s.add(c.getDataType(), c.getFieldName(), c.getComment());
        }
        println("new size " + s.getLength());
        DataType r = dtm.resolve(s, DataTypeConflictHandler.REPLACE_HANDLER);
        var a = toAddr(0x7D2128L);
        currentProgram.getListing().clearCodeUnits(a, a.add(3), false);
        currentProgram.getListing().createData(a, new PointerDataType(r));
        for (DataTypeComponent c : ((Structure) r).getDefinedComponents()) println(String.format("dst +0x%x %s", c.getOffset(), c.getFieldName()));
    }
}
