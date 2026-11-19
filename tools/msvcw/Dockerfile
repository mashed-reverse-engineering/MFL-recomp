# mfl-msvc-wine: Wine + the msvc-wine scripts (github.com/mstorsjo/msvc-wine). The image holds NO Microsoft files:
# the MSVC toolset and the Windows SDK are downloaded by tools/msvcw/msvcw_install.sh (licence accepted by the user) into a host
# cache dir that is mounted at /opt/msvc (install.sh bakes that path into its wrappers, so it is /opt/msvc both at install and at use).
# ponytail: ubuntu:24.04 by tag and msvc-wine by branch; msvcw_install.sh records the resolved sha in the cache dir. Pin both once it works.
FROM ubuntu:24.04
ARG MSVC_WINE_REF=master
ENV DEBIAN_FRONTEND=noninteractive WINEDEBUG=-all WINEPREFIX=/wpfx
# the msvc-wine README's package list (64-bit Wine is enough: the Hostx64/x86 cl.exe is a 64-bit program) + git
RUN apt-get update && apt-get install -y --no-install-recommends \
      wine64 python3 msitools ca-certificates git winbind && rm -rf /var/lib/apt/lists/*
# Ubuntu's wine64 package installs the binary only as /usr/lib/wine/wine64; the msvc-wine wrappers look for wine64 or wine on PATH
# (WINE=$(command -v wine64 || command -v wine || false) -> empty -> "line 72: : command not found", rc 127 on every cl call)
RUN ln -sf /usr/lib/wine/wine64 /usr/local/bin/wine64
RUN git clone https://github.com/mstorsjo/msvc-wine /msvc-wine && git -C /msvc-wine checkout "$MSVC_WINE_REF" \
 && git -C /msvc-wine rev-parse HEAD > /msvc-wine.sha
