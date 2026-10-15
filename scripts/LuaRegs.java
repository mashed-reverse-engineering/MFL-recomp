//Find the C functions registered for Lua names: the name string is pushed next to the function address in code
//(lua_register-like helper) or sits in a data table {name, function}.  Script args: <names file> <out csv>
//@category Mashed
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.Reference;

public class LuaRegs extends GhidraScript {
    private Address funcPushedNear(Instruction at) {
        Instruction i = at;
        for (int k = 0; k < 3 && i != null; k++) i = i.getPrevious();
        for (int k = 0; k < 7 && i != null; k++, i = i.getNext()) {
            if (!i.getMnemonicString().equals("PUSH") && !i.getMnemonicString().equals("MOV")) continue;
            for (Object o : i.getOpObjects(i.getNumOperands() - 1)) {
                if (o instanceof Scalar) {
                    Address a = toAddr(((Scalar) o).getUnsignedValue());
                    if (a != null && a.getOffset() > 0x401000 && a.getOffset() < 0x5d0000 && getFunctionAt(a) != null) return a;
                }
            }
        }
        return null;
    }

    @Override
    protected void run() throws Exception {
        Set<String> want = new HashSet<>(Files.readAllLines(Path.of(getScriptArgs()[0])));
        StringBuilder csv = new StringBuilder("name,string,ref,function\n");
        int found = 0;
        for (Data d : currentProgram.getListing().getDefinedData(true)) {
            if (!d.hasStringValue()) continue;
            String s = String.valueOf(d.getValue());
            if (!want.contains(s)) continue;
            for (Reference r : getReferencesTo(d.getAddress())) {
                Address fa = null;
                Instruction ins = getInstructionAt(r.getFromAddress());
                if (ins != null) fa = funcPushedNear(ins);
                else {
                    long fn = currentProgram.getMemory().getInt(r.getFromAddress().add(4)) & 0xffffffffL;
                    Address t = toAddr(fn);
                    if (t != null && getFunctionAt(t) != null) fa = t;
                }
                if (fa == null) continue;
                csv.append(s).append(',').append(d.getAddress()).append(',').append(r.getFromAddress()).append(',').append(fa).append('\n');
                found++;
            }
        }
        Files.writeString(Path.of(getScriptArgs()[1]), csv.toString());
        println("found " + found + " registrations");
    }
}
