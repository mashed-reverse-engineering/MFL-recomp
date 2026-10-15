//Parse RenderWare SDK headers into a Ghidra data type archive (.gdt).
//Script args: <headerDir> <stubDir> <out.gdt>
//@category Mashed
import java.io.File;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.cparser.C.CParserUtils;
import ghidra.program.model.data.*;

public class ParseRwHeaders extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        // Ghidra's preprocessor chokes on "#pragma message (...\n)" even in dead #if branches: parse a filtered copy
        java.nio.file.Path tmp = java.nio.file.Files.createTempDirectory("rwhdr");
        try (java.util.stream.Stream<java.nio.file.Path> st = java.nio.file.Files.list(java.nio.file.Path.of(a[0]))) {
            for (java.nio.file.Path p : (Iterable<java.nio.file.Path>) st::iterator) {
                if (!java.nio.file.Files.isRegularFile(p)) continue;
                List<String> keep = new ArrayList<>();
                for (String l : java.nio.file.Files.readAllLines(p, java.nio.charset.StandardCharsets.ISO_8859_1))
                    if (!l.matches("^\\s*#\\s*pragma\\s+(message|comment).*")) keep.add(l);
                java.nio.file.Files.write(tmp.resolve(p.getFileName()), keep, java.nio.charset.StandardCharsets.ISO_8859_1);
            }
        }
        String hdr = tmp.toString();
        String[] names = {"rwcore.h", "rpworld.h", "rtfsyst.h", "rtpitexd.h", "rtdict.h", "rtanim.h", "rphanim.h", "rpskin.h", "rpmatfx.h", "rtquat.h", "rpspline.h", "rpuvanim.h"};
        List<String> files = new ArrayList<>();
        for (String n : names) if (new File(hdr, n).exists()) files.add(new File(hdr, n).getPath()); else println("missing " + n);
        String[] args = {"-D_WIN32", "-DWIN32", "-D_MSC_VER=1310", "-DNDEBUG", "-D_X86_", "-DRWDEBUG=0", "-DNOASM", "-DRWINT32FROMFLOAT"};
        File out = new File(a[2]);
        if (out.exists()) out.delete();
        FileDataTypeManager dtm = CParserUtils.parseHeaderFiles(new DataTypeManager[0], files.toArray(new String[0]),
            new String[] {hdr, a[1]}, args, a[2], "x86:LE:32:default", "windows", monitor);
        println("parsed: " + dtm.getDataTypeCount(true) + " data types -> " + a[2]);
        dtm.save();
        dtm.close();
    }
}
