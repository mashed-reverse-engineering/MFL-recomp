#!/bin/bash
# msvcw_install.sh --accept-license : build the mfl-msvc-wine image, then download Microsoft's MSVC v143 toolset (VS 2022 17.14 = MSVC 14.44,
# host x64, targets x86 + x64) and the current Windows 10/11 SDK with msvc-wine's vsdownload.py into a cache dir OUTSIDE the repo, and run
# msvc-wine's install.sh there.  Run by the user only: passing --accept-license is your acceptance of the Visual Studio Build Tools licence
# (vsdownload.py prints its URL from Microsoft's manifest).  Nothing from the download goes into any repository or image.
# Cache: MSVCW_CACHE (default ~/.cache/mfl/msvc-wine): msvc/ (toolset + SDK, mounted at /opt/msvc), wpfx/ (Wine prefix), msvc-wine.sha, cl_version.txt.
# Expected: image build ~1 GB pulled, 3-6 min; download ~2-3 GB, unpacked ~4-6 GB in msvc/, 10-30 min depending on the line.
set -e
[ "$1" = --accept-license ] || { echo "usage: msvcw_install.sh --accept-license   (your acceptance of the VS Build Tools licence; see the header)"; exit 2; }
H=$(dirname "$(readlink -f "$0")")
C=${MSVCW_CACHE:-$HOME/.cache/mfl/msvc-wine}
case "$(readlink -f "$C")/" in "$(readlink -f "$H/../..")"/*) echo "msvcw_install: cache $C is inside the repo; pick a dir outside it"; exit 2;; esac
mkdir -p "$C/msvc" "$C/wpfx"
# --builder default: the plain docker driver (a docker-container buildx builder had no route to archive.ubuntu.com); --network=host: the host's DNS/route
docker buildx build --builder default --network=host --load -t mfl-msvc-wine:local "$H"
U="--network=host --user $(id -u):$(id -g) -e HOME=/tmp -e WINEPREFIX=/wpfx -v $C/msvc:/opt/msvc -v $C/wpfx:/wpfx"
docker run --rm $U mfl-msvc-wine:local python3 /msvc-wine/vsdownload.py --accept-license --msvc-version 17.14 --architecture x86 x64 --dest /opt/msvc
docker run --rm $U mfl-msvc-wine:local /msvc-wine/install.sh /opt/msvc
docker run --rm $U mfl-msvc-wine:local cat /msvc-wine.sha > "$C/msvc-wine.sha"
docker run --rm $U mfl-msvc-wine:local /opt/msvc/bin/x86/cl > "$C/cl_version.txt" 2>&1 || true
du -sh "$C/msvc"; head -2 "$C/cl_version.txt"
echo "msvcw_install: done; compile check: python3 tools/run.py docker msvcw-compile <out dir>"
