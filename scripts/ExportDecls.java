//Write C extern declarations for every function (prototype with calling convention) and every typed data symbol of the program. Args: <outfile>
//@category Mashed
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class ExportDecls extends GhidraScript {
    static String decl(DataType dt, String name) {
        String dims = "";
        while (dt instanceof Array) { Array ar = (Array) dt; dims += "[" + ar.getNumElements() + "]"; dt = ar.getDataType(); }
        String stars = "";
        while (dt instanceof Pointer) { stars += "*"; DataType t = ((Pointer) dt).getDataType(); if (t == null) { dt = null; break; } dt = t; }
        String base;
        if (dt == null) base = "void";
        else if (dt instanceof TypeDef) base = dt.getName();
        else if (dt instanceof Structure) base = "struct " + dt.getName();
        else if (dt instanceof Union) base = "union " + dt.getName();
        else if (dt instanceof ghidra.program.model.data.Enum) base = "enum " + dt.getName();
        else if (dt instanceof FunctionDefinition) { base = "void"; stars = "*" + stars; return "void (*" + name + ")()" ; }
        else base = dt.getName();
        base = base.replace(" ", "_").replace("struct_", "struct ").replace("union_", "union ").replace("enum_", "enum ");
        return base + " " + stars + name + dims;
    }
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        try (PrintWriter w = new PrintWriter(new FileWriter(a[0]))) {
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                String n = f.getName().replaceAll("[^A-Za-z0-9_]", "_");
                if (f.getName().startsWith("thunk_") || n.isEmpty() || Character.isDigit(n.charAt(0))) continue;
                StringBuilder sb = new StringBuilder();
                sb.append("/*").append(String.format("%08x", f.getEntryPoint().getOffset())).append("*/ extern ");
                sb.append(decl(f.getReturn().getDataType(), "").trim()).append(" ");
                String cc = f.getCallingConventionName();
                if (cc != null && (cc.equals("__stdcall") || cc.equals("__fastcall") || cc.equals("__thiscall") || cc.equals("__cdecl"))) sb.append(cc.equals("__thiscall") ? "__fastcall" : cc).append(" ");
                sb.append(n).append("(");
                Parameter[] ps = f.getParameters();
                for (int i = 0; i < ps.length; i++) { if (i > 0) sb.append(", "); sb.append(decl(ps[i].getDataType(), "p" + i)); }
                
                sb.append(");");
                w.println(sb);
            }
            for (Symbol s : currentProgram.getSymbolTable().getAllSymbols(true)) {
                if (!s.isPrimary() || s.getSymbolType() != SymbolType.LABEL) continue;
                Data d = currentProgram.getListing().getDataAt(s.getAddress());
                if (d == null || d.getDataType() == null) continue;
                if (s.getName().startsWith("PTR_") || s.getName().startsWith("LAB_") || s.getName().startsWith("s_") || s.getName().startsWith("u_")) continue;
                String n = s.getName().replaceAll("[^A-Za-z0-9_]", "_");
                if (n.isEmpty() || Character.isDigit(n.charAt(0))) continue;
                w.println("/*" + String.format("%08x", s.getAddress().getOffset()) + "*/ extern " + decl(d.getDataType(), n) + ";");
            }
        }
        println("done");
    }
}
