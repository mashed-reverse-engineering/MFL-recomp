//Import RenderWare SDK .lib files (MS COFF archives) and build one Function ID library from them.
//Script args: <libDir> <outFidb> <family> <version> <variant>
//@category FunctionID
import java.io.File;
import java.util.*;

import ghidra.app.script.GhidraScript;
import ghidra.app.util.bin.*;
import ghidra.app.util.bin.format.coff.*;
import ghidra.app.util.bin.format.coff.archive.*;
import ghidra.app.util.importer.MessageLog;
import ghidra.app.util.importer.ProgramLoader;
import ghidra.app.util.opinion.*;
import ghidra.feature.fid.db.*;
import ghidra.feature.fid.service.*;
import ghidra.framework.model.*;
import ghidra.framework.store.local.LocalFileSystem;
import ghidra.program.model.lang.LanguageID;
import ghidra.program.model.listing.Program;
import ghidra.util.task.CancelOnlyWrappingTaskMonitor;
import ghidra.util.task.TaskMonitor;

public class BuildRwFid extends GhidraScript {
    private String mangle(String n) {
        StringBuilder sb = new StringBuilder();
        for (char c : n.toCharArray()) sb.append(LocalFileSystem.isValidNameCharacter(c) ? c : '_');
        return sb.toString();
    }

    @Override
    protected void run() throws Exception {
        String[] a = getScriptArgs();
        File libDir = new File(a[0]);
        File out = new File(a[1]);
        DomainFolder root = state.getProject().getProjectData().getRootFolder();
        ArrayList<DomainFile> programs = new ArrayList<>();
        MessageLog log = new MessageLog();
        File[] libs = libDir.listFiles((d, n) -> n.toLowerCase().endsWith(".lib"));
        Arrays.sort(libs);
        for (File lib : libs) {
            DomainFolder folder = root.getFolder(mangle(lib.getName()));
            if (folder == null) folder = root.createFolder(mangle(lib.getName()));
            try (RandomAccessByteProvider provider = new RandomAccessByteProvider(lib)) {
                if (!CoffArchiveHeader.isMatch(provider)) { println("not a COFF archive: " + lib); continue; }
                CoffArchiveHeader hdr = CoffArchiveHeader.read(provider, TaskMonitor.DUMMY);
                HashSet<Long> seen = new HashSet<>();
                int n = 0;
                for (CoffArchiveMemberHeader m : hdr.getArchiveMemberHeaders()) {
                    if (!seen.add(m.getPayloadOffset()) || !m.isCOFF()) continue;
                    try (ByteProvider cp = new ByteProviderWrapper(provider, m.getPayloadOffset(), m.getSize())) {
                        CoffFileHeader ch = new CoffFileHeader(cp);
                        if (!CoffMachineType.isMachineTypeDefined(ch.getMagic())) continue;
                        String[] sp = m.getName().split("[/\\\\]");
                        String name = mangle(sp[sp.length - 1]) + "_" + (n++);
                        try (LoadResults<Program> lr = ProgramLoader.builder().source(cp)
                                .project(state.getProject()).projectFolderPath(folder.getPathname())
                                .loaders(MSCoffLoader.class).compiler("windows").name(name)
                                .log(log).monitor(new CancelOnlyWrappingTaskMonitor(monitor)).load()) {
                            for (Loaded<Program> l : lr) {
                                Program p = l.getDomainObject(this);
                                int tx = p.startTransaction("analyze"); try { analyzeAll(p); } finally { p.endTransaction(tx, true); }
                                try { l.save(monitor); programs.add(p.getDomainFile()); }
                                finally { p.release(this); }
                            }
                        }
                    }
                    catch (Exception e) { printerr("skip " + lib.getName() + " " + m.getName() + ": " + e); }
                }
            }
            println("imported " + lib.getName());
        }
        println("programs imported: " + programs.size());

        if (out.exists()) out.delete();
        FidFileManager.getInstance().createNewFidDatabase(out);
        FidFileManager.getInstance().addUserFidFile(out);
        FidService service = new FidService();
        FidFile ff = null;
        for (FidFile f : FidFileManager.getInstance().getUserAddedFiles())
            if (f.getPath().equals(out.getAbsolutePath()) || f.getName().equals(out.getName())) ff = f;
        FidDB db = ff.getFidDB(true);
        try {
            FidPopulateResult res = service.createNewLibraryFromPrograms(db, a[2], a[3], a[4], programs, null,
                new LanguageID("x86:LE:32:default"), null, null, TaskMonitor.DUMMY);
            println("visited " + res.getTotalAttempted() + ", added " + res.getTotalAdded() + ", excluded " + res.getTotalExcluded());
            for (Map.Entry<FidPopulateResult.Disposition, Integer> e : res.getFailures().entrySet())
                if (e.getKey() != FidPopulateResult.Disposition.INCLUDED) println("  " + e.getKey() + ": " + e.getValue());
            db.saveDatabase("Saving", monitor);
        } finally { db.close(); }
        println("wrote " + out);
    }
}
