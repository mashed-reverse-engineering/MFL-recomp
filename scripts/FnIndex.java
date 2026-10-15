//Write a tab separated index of all functions: address, name, size, callers, referenced strings, named callees. Args: <outfile>
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class FnIndex extends GhidraScript {
    @Override
    protected void run() throws Exception {
        Listing l = currentProgram.getListing();
        ReferenceManager rm = currentProgram.getReferenceManager();
        try (PrintWriter w = new PrintWriter(new FileWriter(getScriptArgs()[0]))) {
            for (Function f : l.getFunctions(true)) {
                long size = f.getBody().getNumAddresses();
                int callers = 0;
                for (Reference r : rm.getReferencesTo(f.getEntryPoint())) if (r.getReferenceType().isCall()) callers++;
                LinkedHashSet<String> strs = new LinkedHashSet<>(), callees = new LinkedHashSet<>();
                for (Address a : f.getBody().getAddresses(true)) {
                    for (Reference r : rm.getReferencesFrom(a)) {
                        if (r.getReferenceType().isCall()) {
                            Function c = getFunctionAt(r.getToAddress());
                            if (c != null && !c.getName().startsWith("FUN_") && !c.getName().startsWith("thunk_FUN_")) callees.add(c.getName());
                        } else if (r.getReferenceType().isData() && strs.size() < 4) {
                            Data d = l.getDataAt(r.getToAddress());
                            if (d != null && d.hasStringValue()) {
                                String s = String.valueOf(d.getValue()).replace("\n", "\\n").replace("\t", " ");
                                strs.add(s.length() > 60 ? s.substring(0, 60) : s);
                            }
                        }
                    }
                }
                w.println(f.getEntryPoint() + "\t" + f.getName() + "\t" + size + "\t" + callers + "\t" + String.join(" | ", strs) + "\t" + String.join(",", callees));
            }
        }
        println("wrote index");
    }
}
