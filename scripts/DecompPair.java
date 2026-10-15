//Round-trip diff: decompile the ORIGINAL function and OUR compiled copy (placed by tools/overlay.py --no-patch) with identical settings and signature.
//Args: <jobfile> <outdir>.  Job line: `name origVA ovl.exe ovlVA` (# comments ok).  Writes <outdir>/<name>.orig.c and <name>.ours.c.
//Run on a scratch COPY of the project with -readOnly -noanalysis: the overlay block is added in memory and never saved.
//@category Mashed
import java.io.*;
import java.nio.*;
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.listing.Function.FunctionUpdateType;

public class DecompPair extends GhidraScript {
    String decomp(DecompInterface di, Function f) throws Exception {
        for (Variable v : f.getLocalVariables()) f.removeVariable(v);      // auto names (iVar1 ...) on both sides
        DecompileResults r = di.decompileFunction(f, 90, monitor);
        if (!r.decompileCompleted()) return "// DECOMPILE FAILED: " + r.getErrorMessage() + "\n";
        return r.getDecompiledFunction().getC();
    }

    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        Path out = Path.of(a[1]);
        Files.createDirectories(out);
        Memory mem = currentProgram.getMemory();
        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        for (String line : Files.readAllLines(Path.of(a[0]))) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] j = line.split("\\s+");
            String name = j[0];
            try {
                Function orig = getFunctionAt(toAddr(j[1]));
                if (orig == null) { disassemble(toAddr(j[1])); orig = createFunction(toAddr(j[1]), null); }
                if (orig == null) { printerr("no original function for " + name); continue; }
                // our block, from the PE section of ovl.exe that holds ovlVA
                long ova = Long.parseLong(j[3], 16);
                ByteBuffer d = ByteBuffer.wrap(Files.readAllBytes(Path.of(j[2]))).order(ByteOrder.LITTLE_ENDIAN);
                int e = d.getInt(0x3C), ns = d.getShort(e + 6) & 0xffff, osz = d.getShort(e + 20) & 0xffff;
                long base = d.getInt(e + 24 + 28) & 0xffffffffL;
                byte[] bytes = null;
                for (int i = 0; i < ns; i++) {
                    int o = e + 24 + osz + i * 40;
                    long vs = d.getInt(o + 8) & 0xffffffffL, va = d.getInt(o + 12) & 0xffffffffL, rs = d.getInt(o + 16) & 0xffffffffL, rp = d.getInt(o + 20) & 0xffffffffL;
                    if (base + va <= ova && ova < base + va + Math.max(vs, rs)) {
                        bytes = new byte[(int) Math.max(vs, rs)];
                        System.arraycopy(d.array(), (int) rp, bytes, 0, (int) Math.min(rs, bytes.length));
                        ova = base + va;
                        break;
                    }
                }
                if (bytes == null) { printerr("ovlVA not in a section: " + name); continue; }
                long fva = Long.parseLong(j[3], 16);
                MemoryBlock blk = mem.createInitializedBlock("ovl_" + name, toAddr(ova), new ByteArrayInputStream(bytes), bytes.length, monitor, false);
                blk.setRead(true); blk.setExecute(true); blk.setWrite(false);
                try {
                    disassemble(toAddr(fva));
                    Function ours = createFunction(toAddr(fva), name);
                    if (ours == null) { printerr("cannot create our function " + name); continue; }
                    ours.setName(name, SourceType.USER_DEFINED);
                    // a jmp out of the body to another function's entry is a tail call (the analyzer marks the original's the same way)
                    for (Instruction ins : currentProgram.getListing().getInstructions(ours.getBody(), true)) {
                        if (ins.getFlowType().isJump() && ins.getFlowType().isUnConditional() && !ins.getFlowType().isComputed()) {
                            for (Address t : ins.getFlows())
                                if (!ours.getBody().contains(t) && getFunctionAt(t) != null) ins.setFlowOverride(FlowOverride.CALL_RETURN);
                        }
                    }
                    List<Variable> ps = new ArrayList<>();
                    for (Parameter p : orig.getParameters()) ps.add(new ParameterImpl(p.getName(), p.getDataType(), p.getVariableStorage(), currentProgram));
                    Parameter rp = orig.getReturn();
                    String cc = orig.getCallingConventionName();
                    boolean plain = orig.getParameterCount() == 0 && !orig.hasCustomVariableStorage();
                    if (plain) { ours.setReturnType(rp.getDataType(), SourceType.USER_DEFINED); if (!"unknown".equals(cc)) ours.setCallingConvention(cc); }
                    else ours.updateFunction("unknown".equals(cc) ? null : cc, new ReturnParameterImpl(rp.getDataType(), rp.getVariableStorage(), currentProgram), ps,
                        FunctionUpdateType.CUSTOM_STORAGE, true, SourceType.USER_DEFINED);
                    println("sig " + name + ": " + orig.getSignature().getPrototypeString() + " custom=" + orig.hasCustomVariableStorage() + " -> " + ours.getSignature().getPrototypeString());
                    ours.setNoReturn(orig.hasNoReturn());
                    ours.setVarArgs(orig.hasVarArgs());
                    Files.writeString(out.resolve(name + ".orig.c"), decomp(di, orig));
                    Files.writeString(out.resolve(name + ".ours.c"), decomp(di, ours));
                    println("pair ok " + name + " orig=" + orig.getBody().getNumAddresses() + " ours=" + ours.getBody().getNumAddresses());
                    removeFunction(ours);
                } finally {
                    mem.removeBlock(blk, monitor);
                }
            } catch (Exception ex) {
                printerr("pair FAIL " + name + ": " + ex);
            }
        }
    }
}
