//Import a data type archive (.gdt) into the current program and apply function signatures to functions
//whose name equals a function definition in the archive (use after ApplyFid.java).
//Script args: <archive.gdt>
//@category Mashed
import java.io.File;
import java.util.*;
import ghidra.app.cmd.function.ApplyFunctionSignatureCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class ApplyRwTypes extends GhidraScript {
    @Override
    protected void run() throws Exception {
        FileDataTypeManager arch = FileDataTypeManager.openFileArchive(new File(getScriptArgs()[0]), false);
        try {
            DataTypeManager dtm = currentProgram.getDataTypeManager();
            List<DataType> all = new ArrayList<>();
            arch.getAllDataTypes(all);
            Map<String, FunctionDefinition> defs = new HashMap<>();
            List<DataType> types = new ArrayList<>();
            for (DataType d : all) {
                if (d instanceof FunctionDefinition) defs.put(d.getName(), (FunctionDefinition) d);
                else types.add(d);
            }
            dtm.addDataTypes(types, DataTypeConflictHandler.KEEP_HANDLER, monitor);
            println("data types added: " + types.size() + ", function definitions in archive: " + defs.size());
            int applied = 0, failed = 0;
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                FunctionDefinition def = defs.get(f.getName());
                if (def == null) continue;
                ApplyFunctionSignatureCmd cmd = new ApplyFunctionSignatureCmd(f.getEntryPoint(), def, SourceType.USER_DEFINED, false, true);
                if (cmd.applyTo(currentProgram, monitor)) applied++; else { failed++; printerr("failed " + f.getName() + ": " + cmd.getStatusMsg()); }
            }
            println("signatures applied: " + applied + ", failed: " + failed);
        } finally { arch.close(); }
    }
}
