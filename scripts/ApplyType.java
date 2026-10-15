//Apply a data type (found by name in any category of the program's data type manager) to a global. Args: <address> <type name> <ptr|val> [<label>]
//@category Mashed
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyType extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Address addr = toAddr(a[0]);
        List<DataType> found = new ArrayList<>();
        currentProgram.getDataTypeManager().findDataTypes(a[1], found);
        if (found.isEmpty()) { printerr("type not found: " + a[1]); return; }
        DataType dt = found.get(0);
        for (DataType d : found) if (d instanceof Structure && ((Structure) d).getLength() > 0) { dt = d; break; }
        if (a[2].equals("ptr")) dt = new PointerDataType(dt);
        currentProgram.getListing().clearCodeUnits(addr, addr.add(dt.getLength() - 1), false);
        currentProgram.getListing().createData(addr, dt);
        if (a.length > 3) createLabel(addr, a[3], true, SourceType.USER_DEFINED);
        println("applied " + dt.getName() + " (" + dt.getLength() + " bytes) at " + addr);
    }
}
