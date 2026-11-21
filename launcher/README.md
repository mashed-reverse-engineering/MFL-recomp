# MFLng launcher

`MFLng-launch.exe`: a small Windows launcher for MFLng. It goes in the game folder next to `MFLng.exe`. An MFLng install looks like this:

```
<game folder>\
  MFL.exe, launch.exe, ...                         the game, untouched
  MFLng.exe  MFLng.nofix.exe  MFLng-launch.exe
  mflrw.dll                                        the exes import it directly, so it must lie beside them
  INSTALL.txt
  mflng\
    mfl_audio_miniaudio.dll  mfl_video_ffmpeg.dll  avformat-*.dll  avcodec-*.dll  avutil-*.dll  swscale-*.dll
    BUILDINFO.txt  SHA256SUMS  LICENSES\
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

The launcher creates nothing in `%TEMP%` and no helper file anywhere. In the game folder and in `mflng\` it leaves only the replaced files. An update briefly creates `<name>.new` and `<name>.old` beside each of them. The `.old` of the running launcher exe is deleted at the next start; after an elevated update in a protected folder it is deleted at the next reboot instead.

## Update path

1. **Download and check.** The launcher downloads `MFLng-<tag>.zip` into memory. It checks the zip against the SHA-256 that the GitHub API gives for the asset (`digest`).
2. **Extract.** It reads the zip itself: a central directory reader, its own raw DEFLATE decoder (`logic.c`, in the style of zlib's `puff`, every read and write bounds-checked) and a CRC-32 check. The game's zlib 1.1.3 is not used.
3. **Install list.** A zip name is the path under the game folder. Only these are installed, at the top or in `mflng/`: `MFLng*.exe`, `mfl*.dll`, the FFmpeg DLLs `avformat-*.dll`, `avcodec-*.dll`, `avutil-*.dll`, `swscale-*.dll`, `*.txt` and `SHA256SUMS`; and `mflng/LICENSES/*.txt`. The zip decides the place of each file (today: the exes, `mflrw.dll` and `INSTALL.txt` at the top, the rest in `mflng/`). Game files (`MFL.exe`, `MFL_original.exe`, `launch.exe`, saves, configs) are never touched. A zip of the older flat layout is refused (it has no `mflng/SHA256SUMS`), and nothing changes.
4. **Verify.** Every `.exe` and `.dll` must be listed in the zip's `mflng/SHA256SUMS` (it lists zip names) and must match it. Other files must match it if they are listed.
5. **Stage.** The verified files go to `update\`.
6. **Writability check.** The launcher creates and deletes a test file in the game folder, and checks that no target file is read-only.
   - Writable: it starts its own exe again with `--apply-update <pid> <data dir>` and exits.
   - Not writable (access denied): it asks *"The install folder needs administrator rights to update. Continue with administrator rights?"*. On *Yes* it starts the same installed exe with the verb `runas` and `--elevated`. That exe lies in the protected folder, so a process without admin rights cannot have swapped it. If you cancel the UAC prompt, the staging is deleted and nothing changes.
7. **Apply step** (the launcher exe itself, in apply mode):
   1. Waits up to 15 s for the first launcher to exit.
   2. Reads every staged file into memory and checks it against the staged `SHA256SUMS` again. It writes from that same buffer, so a file swapped in the user-writable staging folder after the check is never what lands in the game folder.
   3. Writes every file as `<name>.new` next to its target.
   4. Renames `backup\` to `backup.prev\`.
   5. For each file: copies the old file to `backup\`, renames it to `<name>.old` (Windows allows this for a running exe: this is how the launcher replaces itself), and renames `<name>.new` into place.
   6. On success: deletes `backup.prev\` and the `.old` files.
   7. On any failure: puts every `.old` back (or copies the file from `backup\`), deletes the `.new` files, renames `backup.prev\` back to `backup\`, and shows an error naming `update.log`.
   8. Deletes the staging folder and starts the launcher again. After an elevated update the restart goes through `explorer.exe`, so the new launcher runs as the normal user.
Rollback by hand: copy the content of `backup\` back into the game folder (it has the same shape as the install).

## Build

```
launcher/build.sh msvc  <out dir> [version]   # cl 19.44 x86 /MT (msvc-wine image locally; MSVCW_NATIVE=1 in CI)
launcher/build.sh mingw <out dir> [version]   # i686-w64-mingw32-gcc, static
launcher/build.sh test  <out dir>             # native test_logic with ASan/UBSan:
                                              # <out>/test_logic <releases.json> <MFLng-<tag>.zip> <tag> [deflate vectors]
python3 launcher/mk_test_fixture.py <out dir> # writes a fixture zip + releases.json in the release layout and prints those three arguments
```

Imports: kernel32, user32, gdi32, shell32, advapi32, winhttp; `tools/ci/pe_check.py --launcher` checks this list. Static CRT, so no redistributable is needed. Its resources are the RT_MANIFEST `data/link/MFLng.manifest` (asInvoker) and the application icon `assets/icon/mflng.ico` (our own drawing, `mflng.svg` beside it). The launcher uses no third-party code.

Test options (hidden):
- `--test-update-zip <zip>`: uses a local zip as the newest release, with no network.
- `--test-force-elevate`: treats the game folder as not writable, which shows the administrator question and uses the `runas` path.