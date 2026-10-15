//Print dwords of a data range, 4 per line, marking values that are addresses of known symbols. Args: <startHex> <countDwords>
//@category Mashed
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.symbol.*;

public class DumpWords extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        long start = Long.decode("0x" + a[0]);
        int n = Integer.decode(a[1]);
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < n; i++) {
            Address ad = toAddr(start + 4L * i);
            int v = currentProgram.getMemory().getInt(ad);
            String sym = "";
            Symbol s = getSymbolAt(toAddr(v & 0xffffffffL));
            if (s != null && (v & 0xffffffffL) > 0x400000L) sym = "<" + s.getName() + ">";
            sb.append(String.format("%s: %08x %d %s%n", ad, v, v, sym));
        }
        println("\n" + sb);
    }
}
