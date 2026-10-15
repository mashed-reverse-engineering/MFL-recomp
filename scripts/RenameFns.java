//Apply a CSV (address,name[,comment]) given as script arg.
//  name              rename the function (created if missing), comment = function comment
//  @name             data label at the address (comment = end-of-line comment)
//  !                 end-of-line comment only (e.g. on a patched instruction operand)
//@category Mashed
import java.nio.file.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class RenameFns extends GhidraScript {
    @Override
    protected void run() throws Exception {
        int n = 0;
        for (String line : Files.readAllLines(Path.of(getScriptArgs()[0]))) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] p = line.split(",", 3);
            Address a = toAddr(p[0]);
            String comment = p.length > 2 ? p[2] : null;
            if (p[1].equals("!")) {
                setEOLComment(getInstructionContaining(a) != null ? getInstructionContaining(a).getAddress() : a, comment);
            } else if (p[1].startsWith("@")) {
                createLabel(a, p[1].substring(1), true, SourceType.USER_DEFINED);
                if (comment != null) setEOLComment(a, comment);
            } else {
                Function f = getFunctionAt(a);
                if (f == null) { disassemble(a); f = createFunction(a, p[1]); }
                if (f == null) { printerr("no function at " + p[0]); continue; }
                f.setName(p[1], SourceType.USER_DEFINED);
                if (comment != null) f.setComment(comment);
            }
            n++;
        }
        println("applied " + n);
    }
}
