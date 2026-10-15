//Print every reference to the given addresses: "<target> <from> <type> <function containing from>". Args: <hex address>...
//@category Mashed
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class ListRefs extends GhidraScript {
    @Override
    protected void run() throws Exception {
        for (String a : getScriptArgs()) {
            for (Reference r : currentProgram.getReferenceManager().getReferencesTo(toAddr(Long.decode("0x" + a)))) {
                Function f = getFunctionContaining(r.getFromAddress());
                println("REF " + a + " " + r.getFromAddress() + " " + r.getReferenceType() + " " + (f == null ? "-" : f.getName()));
            }
        }
    }
}
