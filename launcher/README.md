# MFLng launcher

`MFLng-launch.exe`: a small Windows launcher for MFLng. It goes in the game folder next to `MFLng.exe`. An MFLng install looks like this:

```
<game folder>\
  MFL.exe, launch.exe, ...                         the game, untouched
  MFLng.exe  MFLng.nofix.exe  MFLng-launch.exe
  mflrw.dll                                        the exes import it directly, so it must lie beside them
  mflng\
    mfl_audio_miniaudio.dll  mfl_video_ffmpeg.dll  avformat-*.dll  avcodec-*.dll  avutil-*.dll  swscale-*.dll
    INSTALL.txt  BUILDINFO.txt  SHA256SUMS  LICENSES\
```

The release zip has this shape: unzip it into the game folder. The game exe loads the sound and movie DLLs from `mflng\` itself (and from its own folder if they are not there).

- **Look**: it copies the original launcher.
  - A borderless window, centred on the screen. Drag it by its picture.
  - The picture is read at run time from your installed `launch.exe`: the 640x480 bitmap in its VB6 form data. If that file is missing it uses `TOASTART\PC\Setup.bmp`. The picture is never copied into this repository or into a release.
  - The menu sits where the original's does, measured from launch.exe's form data (twips / 15): transparent labels in Arial Bold 8.25 pt, black over the sky, right-aligned at x 622, rows 34 px apart from y 80. On hover the text turns dark red and is underlined.
  - Two small flat buttons in the top-right corner minimise and close the window.
  - Without the picture, the same layout is drawn on a dark background.
  - The window is scaled for the system DPI (the manifest makes it DPI aware).
- **Play**: *Play* starts `MFLng.exe`, the fixes build. *Play original behaviour* starts `MFLng.nofix.exe` and is shown only when that file exists. Both start in the launcher's folder with no arguments. The launcher has no game options.
- **Bottom edge**: small shadowed text lines.
  - The update status. *Update available: <tag> - click to update* opens the release notes in a translucent overlay, which has an *Update now* link.
  - The two settings as click-to-switch text: *[x] Check for updates at start-up* and *Channel: Releases / Pre-releases*. Next to them, *Check now*.
  - The credit line with the repository link, and *About*, which opens the overlay with the credits, licence note and notice.
- **Keyboard**:
  - Enter = Play, or the link that has the focus.
  - Tab / Shift+Tab move through the links.
  - Esc closes the overlay, or the launcher when no overlay is open.
  - Arrow keys, PgUp / PgDn and the mouse wheel scroll the overlay.
- **Updates**: the launcher reads the GitHub releases of `mashed-reverse-engineering/MFL-recomp` (public API, no token, WinHTTP).
  - The check runs in a background thread, so start-up never waits for it.
  - Connect, send and receive time out after 3 s each (5 s for the API answer).
  - Offline, the rate limit and other HTTP errors each show one status line; click it to retry.

## Settings (`mflng-launch.ini`, plain text)

```
[launcher]
update_at_startup=on        ; on | off  (off: only the "Check for updates" button)
channel=prerelease          ; release = only full releases | prerelease = the newest of releases and pre-releases
```

The checkbox and the drop-down write this file. When a key is missing, these defaults apply:
- `update_at_startup=on`.
- `channel` is `prerelease` when the installed build is a pre-release: `mflng\BUILDINFO.txt` has the line `release <tag> prerelease`, which `release.yml` writes.
- In every other case `channel` is `release`, including a build with no such line (a local build).

## Which release is "newer"

- *Newest* is the first release in GitHub's order (newest created first) that is not a draft, has the asset `MFLng-<tag>.zip` and fits the channel.
- The installed tag comes from the `release <tag>` line of `mflng\BUILDINFO.txt`.
- Equal tags mean up to date.
- If the installed tag's date (`rYYYYMMDD`) is later than the newest one's, the launcher says the installed build is newer and offers nothing.
- In every other case it offers the update. This includes a build without a `release` line.

## Where the launcher keeps its files

MFL itself keeps everything next to its exe, in the current folder: `videocfg.bin`, `contcfg%d.bin`, `gamesave.bin`, and `mflng_settings.cfg` for `MFL_FIX_PREFS`. That folder may be protected (`C:\Program Files`), so the launcher uses one folder of its own: `%LOCALAPPDATA%\mflng-launch\`. If a file `portable.txt` lies next to the launcher, it uses `<game folder>\mflng\mflng-launch\` instead.

```
mflng-launch\
  mflng-launch.ini     settings
  update\              staging of a download (deleted after the update, after a rollback, and at start-up)
  backup\              the files the last successful update replaced (one level; the same shape: mflng\ and mflng\LICENSES\ inside)
  backup.prev\         only while an update runs: the level before, deleted on success, put back on a rollback
  update.log           log of the last update; update.prev.log = the one before
```

The launcher creates nothing in `%TEMP%`. During an update the data folder also holds `mflng-apply.exe`, a copy of the launcher that does the apply step; the next start deletes it. In the game folder and in `mflng\` the launcher leaves only the files of the release. An update briefly creates `<name>.new` and `<name>.old` beside each of them. A start of the launcher deletes `<release file>.old` and `.new` that an update left behind (after an elevated update in a protected folder the reboot deletes the launcher's `.old`). In one case a file stays until the next start: `MFLng-launch.new.exe`, see step 7.8.

With `portable.txt` the data folder lies in the game folder. If that folder is protected (`C:\Program Files`), the launcher cannot write there and no update can be staged: use `portable.txt` only in a folder you can write to.

## Update path

1. **Download and check.** The launcher downloads `MFLng-<tag>.zip` into memory. It checks the zip against the SHA-256 that the GitHub API gives for the asset (`digest`).
2. **Extract.** It reads the zip itself: a central directory reader, its own raw DEFLATE decoder (`logic.c`, in the style of zlib's `puff`, every read and write bounds-checked) and a CRC-32 check. The game's zlib 1.1.3 is not used.
3. **Install list.** A zip name is the path under the game folder. The launcher installs only these:
   - at the top or in `mflng/`: `MFLng*.exe`, `mfl*.dll` and the FFmpeg DLLs `avformat-*.dll`, `avcodec-*.dll`, `avutil-*.dll`, `swscale-*.dll`. The zip decides the place of each (today: the exes and `mflrw.dll` at the top, the other DLLs in `mflng/`), so a DLL can change its place without a new launcher;
   - only in `mflng/`: `*.txt` (`INSTALL.txt`, `BUILDINFO.txt`) and `SHA256SUMS`;
   - `mflng/LICENSES/*.txt`.

   A name is letters, digits, `.`, `_` and `-` only, so no path out of the game folder, no drive letter and no alternate data stream can pass; Windows device names (`nul.txt`) are refused too. Game files (`MFL.exe`, `MFL_original.exe`, `launch.exe`, saves, configs) and text files beside the exes are never written.
   **A zip with any other file is refused as a whole, and nothing changes**: installing the rest would leave half a release. This covers a top-level `INSTALL.txt` or `SHA256SUMS`, a zip of the older flat layout, and a later layout this launcher does not know (the message says to unpack that release by hand once). The zip must also have `mflng/SHA256SUMS`, a `mflng/BUILDINFO.txt` that names the release that was offered, at least one program file, no name twice, and at most 96 entries.
4. **Verify.** Every `.exe` and `.dll` must be listed in the zip's `mflng/SHA256SUMS` (it lists zip names) and must match it. Other files must match it if they are listed.
5. **Stage.** The verified files go to `update\`. Nothing is written before every name has passed step 3.
6. **Writability check.** The launcher creates and deletes a test file in the game folder, and checks that no target file is read-only.
   - Writable: it copies its own exe to `<data dir>\mflng-apply.exe`, starts that copy with `--apply-update <pid> <data dir> --for <launcher exe>` and exits. So no process runs from `MFLng-launch.exe` while it is replaced: renaming a running exe works on NTFS, but a FAT/exFAT drive, a virus scanner or a leftover `.old` that is still in use can refuse it. If the copy cannot be made or started, the installed exe is started instead, as before.
   - Not writable (access denied): it asks *"The install folder needs administrator rights to update. Continue with administrator rights?"*. On *Yes* it starts the installed exe itself with the verb `runas` and `--elevated`, never the copy. That exe lies in the protected folder, so a process without admin rights cannot have swapped it. If you cancel the UAC prompt, the staging is deleted and nothing changes.
7. **Apply step** (the launcher in apply mode: the copy in the data folder, or the installed exe when elevated):
   1. Waits up to 15 s for the first launcher to exit. Only one apply step runs at a time; a second one says so and goes.
   2. With administrator rights: stops if the data folder or one of its `update\`, `backup\`, `backup.prev\` folders is a link (junction) instead of a folder.
   3. Stops if `MFLng.exe` or `MFLng.nofix.exe` is running: *"The game is running. Close it and update again."* Nothing is changed.
   4. Reads every staged file into memory and checks it against the staged `SHA256SUMS` again. It writes from that same buffer, so a file swapped in the user-writable staging folder after the check is never what lands in the game folder. A file that someone put into the staging folder is not installed.
   5. Lists the **files to remove**: the names in the installed `mflng\SHA256SUMS` (the release that is installed now) that the new release no longer has, for example a DLL that moved between the game folder and `mflng\`, or an FFmpeg DLL with a new version number. The `SHA256SUMS` in `backup\` (the release before that) is read as well, for what an older launcher left behind. A file is removed only if it still has the SHA-256 that list gives. So the launcher removes only what an MFLng release put there, unchanged: never a game file, never a file of yours in `mflng\`, never a release file you replaced, and never the running launcher. (An install of the flat layout before `mflng\` has its `SHA256SUMS` beside the exes; that list is read the same way, so its `mfl_*.dll`, FFmpeg DLLs, `BUILDINFO.txt`, `INSTALL.txt`, `LICENSES\*.txt` and the list itself go with the first update.)
   6. Creates `mflng\` and `mflng\LICENSES\` if the release needs them, and writes every file as `<name>.new` next to its target.
   7. Deletes a `backup.prev\` that an earlier attempt left (the whole folder), then renames `backup\` to `backup.prev\`. If that rename fails, the older backup is dropped and the update goes on.
   8. For each file: copies the old file to `backup\`, renames it to `<name>.old`, and renames `<name>.new` into place. A file to remove is copied to `backup\` and renamed to `<name>.old` in the same way. A rename that gets *access denied* or *in use* is tried again for about 3 s, and a read-only attribute in its way is taken off. If a file still cannot be renamed, the update is rolled back (step 10), with one exception, **the launcher exe itself**:
      - it is written over in place (it is not running: the apply step is the copy);
      - if that fails too because the exe is in use (a second launcher window, or the elevated step, which runs from it), everything else is installed and the new launcher is left beside it as `MFLng-launch.new.exe`. The next start of the launcher starts that file, which waits for the old one to exit, takes its place and starts it; the launcher then deletes it. If even that fails, a message says to copy the file over `MFLng-launch.exe` by hand.

      So an update is never half done: either every file is the new release (the launcher at the latest after its next start), or everything is as before.
   9. On success: deletes `backup.prev\` and the `.old` files.
   10. On any failure: puts every `.old` back (or copies the file from `backup\`), deletes the `.new` files and the folders this update created, renames `backup.prev\` back to `backup\`, and shows an error naming `update.log`. The game folder is then exactly as before. If a file cannot be put back either, `backup\` is kept (it holds the previous files) and the error says so.
   11. Deletes the staging folder and starts the launcher again. After an elevated update the restart goes through `explorer.exe`, so the new launcher runs as the normal user.

`update.log` has one line per step: the versions (`update <installed tag> -> <new tag>`), `replaced <name>` (with `(new file)`), `removed <name>`, `kept <name>` for a dropped file you changed, and every failure with the Windows error code. A rename that failed has a line of its own: both paths, the error code and its text, the file system of the drive, the attributes of the file, and whether the destination exists. The error box shows the first of them with the file name.

Rollback by hand: copy the content of `backup\` back into the game folder (it has the same shape as the install).

The elevated apply step works for the user who confirmed the administrator question: it installs what that user's launcher staged, checked only against the `SHA256SUMS` staged with it. It can only write release file names into the game folder.

## Build

```
launcher/build.sh msvc  <out dir> [version]   # cl 19.44 x86 /MT (msvc-wine image locally; MSVCW_NATIVE=1 in CI)
launcher/build.sh mingw <out dir> [version]   # i686-w64-mingw32-gcc, static
launcher/build.sh test  <out dir>             # native test_logic with ASan/UBSan:
                                              # <out>/test_logic <releases.json> <MFLng-<tag>.zip> <tag> [deflate vectors]
python3 launcher/mk_test_fixture.py <out dir> # writes a fixture zip + releases.json in the release layout and prints those three arguments
```

`test_logic` also runs the update itself on folder trees (in `<folder of the zip>/upd_work`). The staging, apply and rollback code is `logic.c` (`upd_stage`, `upd_apply`) over a small file layer: `launcher.c` implements it with Win32, the test with POSIX calls and injected failures. Each scenario compares the complete game folder and data folder with the expected trees: first install, replaced files, new / removed / moved files in `mflng\`, your own files, refused zips (33 names that must not pass), a changed staging folder, one failure at every write / rename / copy / mkdir / delete of an update (it completes or the trees are exactly as before), a rollback that fails too, the flat layout, `portable.txt`, leftovers, a launcher exe that cannot be renamed (written over; in use: `MFLng-launch.new.exe`; and rolled back with the rest), what an attempt before left behind (`backup.prev\` with unknown folders, `.new`, `.old`), a backup that cannot be moved. What needs Windows is not in it: the Win32 file layer (retries, read-only, the rename report), elevation, a running game, the apply step from the copy, the swap of `MFLng-launch.new.exe` at start.

Imports: kernel32, user32, gdi32, shell32, advapi32, winhttp; `tools/ci/pe_check.py --launcher` checks this list. Static CRT, so no redistributable is needed. Its resources are the RT_MANIFEST `data/link/MFLng.manifest` (asInvoker) and the application icon `assets/icon/mflng.ico` (our own drawing, `mflng.svg` beside it). The launcher uses no third-party code.

Test options (hidden):
- `--test-update-zip <zip>`: uses a local zip as the newest release, with no network.
- `--test-force-elevate`: treats the game folder as not writable, which shows the administrator question and uses the `runas` path.
- `--test-apply-in-place`: the apply step is the installed exe, not the copy in the data folder.