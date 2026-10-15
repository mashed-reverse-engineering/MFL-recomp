//Delinker input: write every memory reference "from,to,reftype,opindex,source,kind" (kind = I<len> for an instruction, D:<datatype> for defined data, U undefined) plus
//"L,<addr>,<label>" for switch table / function labels. Args: <outfile>
//@category Mashed
import java.io.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class ExportRefs extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        ReferenceManager rm = currentProgram.getReferenceManager();
        Listing li = currentProgram.getListing();
        int n = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            AddressIterator it = rm.getReferenceSourceIterator(currentProgram.getMinAddress(), true);
            while (it.hasNext()) {
                Address from = it.next();
                if (!from.isMemoryAddress()) continue;
                String kind = "U";
                Instruction ins = li.getInstructionContaining(from);
                if (ins != null) kind = "I" + ins.getLength();
                else {
                    Data d = li.getDataContaining(from);
                    if (d != null) {
                        Data p = d.getPrimitiveAt((int) from.subtract(d.getAddress()));
                        kind = "D:" + (p != null ? p.getDataType().getName() : d.getDataType().getName()).replace(',', ';');
                    }
                }
                for (Reference r : rm.getReferencesFrom(from)) {
                    if (!r.getToAddress().isMemoryAddress()) continue;
                    w.println(String.format("%08x,%08x,%s,%d,%s,%s", from.getOffset(), r.getToAddress().getOffset(), r.getReferenceType(), r.getOperandIndex(), r.getSource(), kind));
                    n++;
                }
            }
            long cs = -1, ce = -1;                                    // contiguous instruction runs "C,start,end" (end exclusive)
            for (Instruction ins : li.getInstructions(true)) {
                long s = ins.getAddress().getOffset();
                if (s != ce) { if (cs >= 0) w.println(String.format("C,%08x,%08x", cs, ce)); cs = s; }
                ce = s + ins.getLength();
            }
            if (cs >= 0) w.println(String.format("C,%08x,%08x", cs, ce));
            for (Symbol s : currentProgram.getSymbolTable().getAllSymbols(true)) {
                if (!s.isPrimary() || !s.getAddress().isMemoryAddress()) continue;
                String nm = s.getName();
                if (s.getSymbolType() == SymbolType.FUNCTION || nm.startsWith("switchD") || nm.startsWith("caseD") || nm.startsWith("switchdataD"))
                    w.println(String.format("L,%08x,%s", s.getAddress().getOffset(), nm.replace(',', ';')));
            }
        }
        println("refs " + n);
    }
}
