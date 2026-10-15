#!/bin/bash
# live.sh [regress workdir]: colour terminal view of the regress that runs now (default: the last one started); see live.py
exec python3 "$(dirname "$(readlink -f "$0")")/live.py" "$@"
