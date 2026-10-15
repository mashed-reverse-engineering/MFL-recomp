//Print entry/end of the functions containing the given addresses. Args: <addr>...
//@category Mashed
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class FuncBounds extends GhidraScript {
    @Override
    protected void run() throws Exception {
        for (String s : getScriptArgs()) {
            Function f = getFunctionContaining(toAddr(s));
            println(s + " -> " + (f == null ? "none" : f.getName() + " " + f.getEntryPoint() + " .. " + f.getBody().getMaxAddress()));
        }
    }
}
