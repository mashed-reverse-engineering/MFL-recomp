//Write "<name> <hex address>" for every primary function / label symbol (names sanitized to C identifiers like ExportDecls does). Args: <outfile>
//@category Mashed
import java.io.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.symbol.*;

public class ExportSymbols extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        int n = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            for (Symbol s : currentProgram.getSymbolTable().getAllSymbols(true)) {
                if (!s.isPrimary()) continue;
                SymbolType t = s.getSymbolType();
                if (t != SymbolType.LABEL && t != SymbolType.FUNCTION) continue;
                if (!s.getAddress().isMemoryAddress()) continue;
                String nm = s.getName().replaceAll("[^A-Za-z0-9_]", "_");
                if (nm.isEmpty() || Character.isDigit(nm.charAt(0))) continue;
                w.println(nm + " " + String.format("%08x", s.getAddress().getOffset()));
                n++;
            }
        }
        println("symbols " + n);
    }
}
