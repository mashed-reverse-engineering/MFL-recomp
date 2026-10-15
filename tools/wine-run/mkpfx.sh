#!/bin/bash
# mkpfx.sh N: make sure the Wine prefix volume wine_pfx_N exists (a copy of wine_pfx); parallel runs must not share a prefix.
docker volume inspect wine_pfx_$1 >/dev/null 2>&1 && exit 0
docker volume create wine_pfx_$1 >/dev/null
docker run --rm -v wine_pfx:/a:ro -v wine_pfx_$1:/b tianon/wine:latest cp -a /a/. /b/
