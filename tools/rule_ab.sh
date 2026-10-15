#!/bin/bash
# rule_ab.sh <work dir>: A/B measurement of the systematic fixers (FIX_RULES: widths, ret_wide, ...): one autoloop round per setting, prints the number of byte identical functions for each.
# Usage: rule_ab.sh <dir> [settings...]   default settings: none widths ret_wide widths,ret_wide
T=$(dirname "$(readlink -f "$0")"); W=$(readlink -f "$1"); shift
mkdir -p $W
for s in ${@:-none widths ret_wide widths,ret_wide}; do
  FIX_RULES=$s $T/autoloop.sh $W/$(echo $s | tr ',' '_') 1 2>&1 | tail -1 | sed "s/^/[$s] /" | cut -c1-200 | tee -a $W/ab.txt
done
