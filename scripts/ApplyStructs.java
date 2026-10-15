//Create structs from a spec file and apply them to data addresses. Args: <spec file>
//  struct,<Name>,<size hex>
//  field,<offset hex>,<type>,<name>[,comment]     type: u8 u16 u32 i32 f32 char[N] u32[N] ... or another struct name
//  apply,<address>,<Name>[,<count>]                 count > 1 makes an array of the struct
//@category Mashed
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.Listing;

public class ApplyStructs extends GhidraScript {
    DataTypeManager dtm;
    Map<String, DataType> made = new HashMap<>();

    DataType base(String t) {
        switch (t) {
            case "u8": return new ByteDataType();
            case "u16": return new UnsignedShortDataType();
            case "i16": return new ShortDataType();
            case "u32": return new UnsignedIntegerDataType();
            case "i32": return new IntegerDataType();
            case "f32": return new FloatDataType();
            case "char": return new CharDataType();
        }
        DataType d = made.get(t);
        if (d == null) {
            List<DataType> found = new ArrayList<>();
            dtm.findDataTypes(t, found);
            for (DataType f : found) if (f.getLength() > 0) { d = f; break; }
        }
        if (d == null) throw new RuntimeException("unknown type " + t);
        return d;
    }

    DataType type(String t) {
        int i = t.indexOf('[');
        if (i < 0) return base(t);
        DataType b = base(t.substring(0, i));
        int n = Integer.parseInt(t.substring(i + 1, t.length() - 1));
        return new ArrayDataType(b, n, b.getLength());
    }

    @Override
    protected void run() throws Exception {
        dtm = currentProgram.getDataTypeManager();
        Listing listing = currentProgram.getListing();
        StructureDataType cur = null;
        List<String[]> applies = new ArrayList<>();
        for (String line : Files.readAllLines(Path.of(getScriptArgs()[0]))) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] p = line.split(",", 5);
            switch (p[0]) {
                case "struct":
                    cur = new StructureDataType(new CategoryPath("/Mashed"), p[1], Integer.decode(p[2]));
                    made.put(p[1], cur);
                    break;
                case "field": {
                    DataType t = type(p[2]);
                    cur.replaceAtOffset(Integer.decode(p[1]), t, t.getLength(), p[3], p.length > 4 ? p[4] : null);
                    break;
                }
                case "apply": applies.add(p); break;
                case "applyptr": applies.add(p); break;
            }
        }
        int n = 0;
        for (String[] p : applies) {
            DataType s = dtm.resolve(made.containsKey(p[2]) ? made.get(p[2]) : base(p[2]), DataTypeConflictHandler.REPLACE_HANDLER);
            int count = p.length > 3 ? Integer.parseInt(p[3]) : 1;
            DataType t = p[0].equals("applyptr") ? new PointerDataType(s) : count > 1 ? new ArrayDataType(s, count, s.getLength()) : s;
            var a = toAddr(p[1]);
            listing.clearCodeUnits(a, a.add(t.getLength() - 1), false);
            listing.createData(a, t);
            n++;
        }
        println("applied " + n + " structs");
    }
}
