#!/bin/bash
# negtest.sh: negative fixtures of the whole-program oracles (plan_testing item 2), no game run, about 5 s.  Every oracle must FAIL on its stored bad input and pass on the good one:
#   data    sa_oracles.sh data on tools/negtests/data/{good,bad}/ours.map (bad = T-P15: .data 00052004H)
#   render  d3dtrace/drawcmp.py with the gate's course:0 limits (tools/d3dtrace/render_limits.txt; 0 1 0 without it) on frames 0-2 of kept traces (tools/d3dtrace/ref/neg_*.d3d.gz):
#           neg_tp18 = link 1556 (DrawIndexedPrimitive Base/MinVertexIndex swapped), neg_tp19 = link 2152 (timer_ticks high dword), neg_good = gate 1103_0951 ours, neg_ctrl = its control
#   scen    the verdict() function of sa_oracles.sh (extracted, not copied) on saved scenario logs tools/negtests/scen/*.log (crash = link 1228 T-P17)
#   lints   defectlint.py --selftest, stack_lint.py --selftest, harness_drift.py --selftest (M4; skipped without a standalone link under output/gate)
# Exit 1 on the first oracle that does not behave.
T=$(dirname "$(readlink -f "$0")"); G=$(readlink -f $T/../..); N=$G/tools/negtests; O=$(mktemp -d); rc=0
ok() { echo "negtest ok   $*"; }; bad() { echo "negtest FAIL $*"; rc=1; }
# data
$T/sa_oracles.sh data $N/data/good 0 $O/d > $O/dg.txt 2>&1 && ok "data good: $(cat $O/dg.txt)" || bad "data good map rejected: $(cat $O/dg.txt)"
$T/sa_oracles.sh data $N/data/bad 0 $O/d > $O/db.txt 2>&1 && bad "data bad map accepted: $(cat $O/db.txt)" || ok "data bad: $(cat $O/db.txt)"
# render
LF=$G/tools/d3dtrace/render_limits.txt; LIM=$(awk '$1=="course:0" {print $3, $4, $5; exit}' $LF 2>/dev/null); [ -n "$LIM" ] || { echo "negtest note: no course:0 row in $LF, limits 0 1 0 (noise floor of T-P19)"; LIM="0 1 0"; }
R=$G/tools/d3dtrace/ref
python3 $G/tools/d3dtrace/drawcmp.py $R/neg_good.d3d.gz $R/neg_ctrl.d3d.gz $LIM > $O/rg.txt && ok "render good (limits $LIM): $(head -1 $O/rg.txt | cut -c1-140)" || bad "render good trace rejected: $(head -2 $O/rg.txt)"
for t in tp18 tp19; do
  python3 $G/tools/d3dtrace/drawcmp.py $R/neg_$t.d3d.gz $R/neg_ctrl.d3d.gz $LIM > $O/r$t.txt && bad "render neg_$t accepted: $(head -1 $O/r$t.txt)" || ok "render neg_$t: $(head -1 $O/r$t.txt | cut -c1-140)"
done
# scenario verdict
eval "$(sed -n '/^ *verdict() {/,/; }$/p' $T/sa_oracles.sh)"
if ! type verdict > /dev/null 2>&1; then bad "scen: verdict() not found in sa_oracles.sh"; else
  for f in crash:crashed gsdiff:GameState noresult:"no result" shotonly:screenshot; do      # shotonly: the final menu screenshot is judged since 2026-11-03 (race scenarios run with SCEN_IMGTHRESH=off)
    v=$(verdict $N/scen/${f%%:*}.log | tr '\n' ';'); echo "$v" | grep -q "${f#*:}" && ok "scen ${f%%:*}.log -> $v" || bad "scen ${f%%:*}.log not judged a failure (verdict '$v')"
  done
  v=$(verdict $N/scen/pass.log); [ -z "$v" ] && ok "scen pass.log -> pass" || bad "scen pass.log judged a failure: $v"
fi
# lints
python3 $G/tools/defectlint.py --selftest > $O/dl.txt 2>&1 && ok "$(tail -1 $O/dl.txt)" || { grep -E "FAIL" $O/dl.txt | head -5; bad "$(tail -1 $O/dl.txt)"; }
python3 $G/tools/stack_lint.py --selftest > $O/sl.txt 2>&1 && ok "$(tail -1 $O/sl.txt)" || bad "stack_lint selftest: $(tail -2 $O/sl.txt)"
python3 $T/harness_drift.py --selftest > $O/hd.txt 2>&1 && ok "$(tail -1 $O/hd.txt)" || bad "$(tail -1 $O/hd.txt)"
python3 $T/../trapcheck.py --selftest > $O/tc.txt 2>&1 && ok "$(tail -1 $O/tc.txt)" || { grep FAIL $O/tc.txt | head -5; bad "$(tail -1 $O/tc.txt)"; }
python3 $T/../clean_scan.py --selftest > $O/cs.txt 2>&1 && ok "$(tail -1 $O/cs.txt)" || bad "$(tail -1 $O/cs.txt)"
V=~/.cache/mfl_diffemu_venv/bin/python; if [ -x $V ]; then $V $T/sse_align_check.py --selftest > $O/sa.txt 2>&1 && ok "sse_align selftest: $(grep -c ' ok  ' $O/sa.txt) fixtures" || bad "sse_align selftest: $(grep FAIL $O/sa.txt | head -2)"; else echo "negtest note: no diffemu venv, sse_align selftest skipped"; fi
# collis (D1): rw_collis_check.sh must FAIL on the line-flip fixture (the DLL's line entries get start/end swapped) and pass without it; ~30 s, docker
RD=$G/output/librw/mflrw.dll; CL=WorldForAllTriIntsecLine,GeometryForAllTriIntsecLine
if ! grep -q mflrw_collis_bind $RD 2>/dev/null; then echo "negtest note: $RD has no collis exports, collis fixture skipped"; else
  OUT=$O/cg $T/rw_collis_check.sh $RD 2000 $CL > $O/cg.txt 2>&1 && ok "collis good: $(grep -c EQUAL $O/cg.txt) line entries EQUAL" || bad "collis good rejected: $(grep -E 'DIFF|MISSING' $O/cg.txt | head -2)"
  OUT=$O/cn $T/rw_collis_check.sh $RD 2000 $CL neg > $O/cn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/cn.txt) = 2 ] && ok "collis line-flip: both line entries DIFF" || bad "collis line-flip fixture not caught: $(grep -E 'finite cases' $O/cn.txt | head -2)"
fi
# splanim (D1b): rw_splanim_check.sh must FAIL on the path-swap fixture (the DLL's FindPosition / FindMatrix get path 10 <-> 11) and pass without it; ~30 s, docker
RD=$G/output/librw/mflrw.dll; SL=RpSplineFindPosition,RpSplineFindMatrix
if ! grep -q mflrw_splanim_bind $RD 2>/dev/null; then echo "negtest note: $RD has no splanim exports, splanim fixture skipped"; else
  OUT=$O/sg $T/rw_splanim_check.sh $RD 2000 $SL > $O/sg.txt 2>&1 && ok "splanim good: $(grep -c EQUAL $O/sg.txt) spline entries EQUAL" || bad "splanim good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/sg.txt | head -2)"
  OUT=$O/sn $T/rw_splanim_check.sh $RD 2000 $SL neg > $O/sn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/sn.txt) = 2 ] && ok "splanim path-swap: both spline entries DIFF" || bad "splanim path-swap fixture not caught: $(grep -E 'finite cases' $O/sn.txt | head -2)"
fi
# bezpat (D1c): rw_bezpat_check.sh must FAIL on the step-swap fixture (the DLL's OriginDifference3d / 4d get stepU <-> stepV) and pass without it; ~20 s, docker
RD=$G/output/librw/mflrw.dll; BL=RtBezierQuadOriginDifference3d,RtBezierQuadOriginDifference4d
if ! grep -q rw37_RtBezierQuadOriginDifference4d $RD 2>/dev/null; then echo "negtest note: $RD has no bezpat exports, bezpat fixture skipped"; else
  OUT=$O/bg $T/rw_bezpat_check.sh $RD 2000 $BL > $O/bg.txt 2>&1 && ok "bezpat good: $(grep -c EQUAL $O/bg.txt) entries EQUAL" || bad "bezpat good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/bg.txt | head -2)"
  OUT=$O/bn $T/rw_bezpat_check.sh $RD 2000 $BL neg > $O/bn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/bn.txt) = 2 ] && ok "bezpat step-swap: both entries DIFF" || bad "bezpat step-swap fixture not caught: $(grep -E 'finite cases' $O/bn.txt | head -2)"
fi
# d3d9vd: rw_d3d9vd_check.sh must FAIL on the component-swap fixture (the DLL's InstV3d / InstV2d read x <-> y swapped) and pass without it; ~20 s, docker
RD=$G/output/librw/mflrw.dll; VL=InstV3d,InstV2d
if ! grep -q rw37__rpD3D9VertexDeclarationInstV3d $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9vd exports, d3d9vd fixture skipped"; else
  OUT=$O/vg $T/rw_d3d9vd_check.sh $RD 2000 $VL > $O/vg.txt 2>&1 && ok "d3d9vd good: $(grep -c EQUAL $O/vg.txt) entries EQUAL" || bad "d3d9vd good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/vg.txt | head -2)"
  OUT=$O/vn $T/rw_d3d9vd_check.sh $RD 2000 $VL neg > $O/vn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/vn.txt) = 2 ] && ok "d3d9vd component-swap: both entries DIFF" || bad "d3d9vd component-swap fixture not caught: $(grep -E 'finite cases' $O/vn.txt | head -2)"
fi
# hanimcam (L6): rw_hanimcam_check.sh must FAIL on the swap fixture (the DLL side gets near <-> far and the first keyframe quat x <-> y, inputs restored after the call) and pass without it; ~20 s, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_hanimcam_bind $RD 2>/dev/null; then echo "negtest note: $RD has no hanimcam exports, hanimcam fixture skipped"; else
  OUT=$O/hg $T/rw_hanimcam_check.sh $RD 2000 > $O/hg.txt 2>&1 && ok "hanimcam good: $(grep -c EQUAL $O/hg.txt) entries EQUAL" || bad "hanimcam good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/hg.txt | head -2)"
  OUT=$O/hn $T/rw_hanimcam_check.sh $RD 2000 all neg > $O/hn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/hn.txt) = 3 ] && ok "hanimcam swap: all three entries DIFF" || bad "hanimcam swap fixture not caught: $(grep -E 'finite cases' $O/hn.txt | head -3)"
fi
# rwmath (rw4): rw_rwmath_check.sh must FAIL on the swap fixture (two input words swapped on the DLL side, inputs restored after the call) and pass without it; ~60 s, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_rwmath_bind $RD 2>/dev/null; then echo "negtest note: $RD has no rwmath exports, rwmath fixture skipped"; else
  OUT=$O/rwg $T/rw_rwmath_check.sh $RD 2000 > $O/rwg.txt 2>&1 && ok "rwmath good: $(grep -c EQUAL $O/rwg.txt) entries EQUAL" || bad "rwmath good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/rwg.txt | head -2)"
  OUT=$O/rwn $T/rw_rwmath_check.sh $RD 2000 all neg > $O/rwn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/rwn.txt) = 26 ] && ok "rwmath swap: all 26 entries DIFF" || bad "rwmath swap fixture not caught: $(grep -c 'finite cases DIFF' $O/rwn.txt) of 26 DIFF"
fi
# pixfmt (rw4): rw_pixfmt_check.sh must FAIL on the swap fixture (input bytes 0 <-> 2 swapped on the DLL side, inputs restored after the call) and pass without it; ~60 s, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_pixfmt_bind $RD 2>/dev/null; then echo "negtest note: $RD has no pixfmt exports, pixfmt fixture skipped"; else
  OUT=$O/pig $T/rw_pixfmt_check.sh $RD 2000 > $O/pig.txt 2>&1 && ok "pixfmt good: $(grep -c EQUAL $O/pig.txt) entries EQUAL" || bad "pixfmt good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/pig.txt | head -2)"
  OUT=$O/pin $T/rw_pixfmt_check.sh $RD 2000 all neg > $O/pin.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/pin.txt) = 8 ] && ok "pixfmt swap: all 8 entries DIFF" || bad "pixfmt swap fixture not caught: $(grep -c 'finite cases DIFF' $O/pin.txt) of 8 DIFF"
fi
# tristrip (rw5): rw_tristrip_check.sh must FAIL on the swap fixture (two input words swapped on the DLL side, inputs restored after the call) and pass without it; ~2 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_tristrip_bind $RD 2>/dev/null; then echo "negtest note: $RD has no tristrip exports, tristrip fixture skipped"; else
  OUT=$O/trxg $T/rw_tristrip_check.sh $RD 2000 > $O/trxg.txt 2>&1 && ok "tristrip good: $(grep -c EQUAL $O/trxg.txt) entries EQUAL" || bad "tristrip good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/trxg.txt | head -2)"
  OUT=$O/trxn $T/rw_tristrip_check.sh $RD 2000 all neg > $O/trxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/trxn.txt) = 8 ] && ok "tristrip swap: all 8 entries DIFF" || bad "tristrip swap fixture not caught: $(grep -c 'finite cases DIFF' $O/trxn.txt) of 8 DIFF"
fi
# colltree (rw5): rw_colltree_check.sh must FAIL on the swap fixture (two input words swapped on the DLL side, inputs restored after the call) and pass without it; ~2 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_colltree_bind $RD 2>/dev/null; then echo "negtest note: $RD has no colltree exports, colltree fixture skipped"; else
  OUT=$O/coxg $T/rw_colltree_check.sh $RD 2000 > $O/coxg.txt 2>&1 && ok "colltree good: $(grep -c EQUAL $O/coxg.txt) entries EQUAL" || bad "colltree good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/coxg.txt | head -2)"
  OUT=$O/coxn $T/rw_colltree_check.sh $RD 2000 all neg > $O/coxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/coxn.txt) = 13 ] && ok "colltree swap: all 13 entries DIFF" || bad "colltree swap fixture not caught: $(grep -c 'finite cases DIFF' $O/coxn.txt) of 13 DIFF"
fi
# image (rw5): rw_image_check.sh must FAIL on the swap fixture (two input words swapped on the DLL side, inputs restored after the call) and pass without it; ~2 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_image_bind $RD 2>/dev/null; then echo "negtest note: $RD has no image exports, image fixture skipped"; else
  OUT=$O/imxg $T/rw_image_check.sh $RD 2000 > $O/imxg.txt 2>&1 && ok "image good: $(grep -c EQUAL $O/imxg.txt) entries EQUAL" || bad "image good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/imxg.txt | head -2)"
  OUT=$O/imxn $T/rw_image_check.sh $RD 2000 all neg > $O/imxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/imxn.txt) = 6 ] && ok "image swap: all 6 entries DIFF" || bad "image swap fixture not caught: $(grep -c 'finite cases DIFF' $O/imxn.txt) of 6 DIFF"
fi
# resample (rw5): rw_resample_check.sh must FAIL on the swap fixture (two source pixels swapped on the DLL side, inputs restored after the call) and pass without it; ~2 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q rw37_ImageResampleGetSpan $RD 2>/dev/null; then echo "negtest note: $RD has no resample exports, resample fixture skipped"; else
  OUT=$O/rexg $T/rw_resample_check.sh $RD 2000 > $O/rexg.txt 2>&1 && ok "resample good: $(grep -c EQUAL $O/rexg.txt) entries EQUAL" || bad "resample good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/rexg.txt | head -2)"
  OUT=$O/rexn $T/rw_resample_check.sh $RD 2000 all neg > $O/rexn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/rexn.txt) = 2 ] && ok "resample swap: all 2 entries DIFF" || bad "resample swap fixture not caught: $(grep -c 'finite cases DIFF' $O/rexn.txt) of 2 DIFF"
fi
# sortcmp (rw6): rw_sortcmp_check.sh must FAIL on the swap fixture (the two comparator arguments swapped on the DLL side, inputs restored after the call) and pass without it; ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q rw37_TriStripSortCompare $RD 2>/dev/null; then echo "negtest note: $RD has no sortcmp exports, sortcmp fixture skipped"; else
  OUT=$O/soxg $T/rw_sortcmp_check.sh $RD 2000 > $O/soxg.txt 2>&1 && ok "sortcmp good: $(grep -c EQUAL $O/soxg.txt) entries EQUAL" || bad "sortcmp good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/soxg.txt | head -2)"
  OUT=$O/soxn $T/rw_sortcmp_check.sh $RD 2000 all neg > $O/soxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/soxn.txt) = 2 ] && ok "sortcmp swap: all 2 entries DIFF" || bad "sortcmp swap fixture not caught: $(grep -c 'finite cases DIFF' $O/soxn.txt) of 2 DIFF"
fi
# uvanim (rw6): rw_uvanim_check.sh must FAIL on the swap fixture (two input words swapped on the DLL side, inputs restored after the call) and pass without it; ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_uvanim_bind $RD 2>/dev/null; then echo "negtest note: $RD has no uvanim exports, uvanim fixture skipped"; else
  OUT=$O/uvxg $T/rw_uvanim_check.sh $RD 2000 > $O/uvxg.txt 2>&1 && ok "uvanim good: $(grep -c EQUAL $O/uvxg.txt) entries EQUAL" || bad "uvanim good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/uvxg.txt | head -2)"
  OUT=$O/uvxn $T/rw_uvanim_check.sh $RD 2000 all neg > $O/uvxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/uvxn.txt) = 4 ] && ok "uvanim swap: all 4 entries DIFF" || bad "uvanim swap fixture not caught: $(grep -c 'finite cases DIFF' $O/uvxn.txt) of 4 DIFF"
fi
# camsync (rw6): rw_camsync_check.sh must FAIL on the swap fixture (two input words swapped on the DLL side, inputs restored after the call) and pass without it; ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_camsync_bind $RD 2>/dev/null; then echo "negtest note: $RD has no camsync exports, camsync fixture skipped"; else
  OUT=$O/caxg $T/rw_camsync_check.sh $RD 2000 > $O/caxg.txt 2>&1 && ok "camsync good: $(grep -c EQUAL $O/caxg.txt) entries EQUAL" || bad "camsync good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/caxg.txt | head -2)"
  OUT=$O/caxn $T/rw_camsync_check.sh $RD 2000 all neg > $O/caxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/caxn.txt) = 1 ] && ok "camsync swap: all 1 entries DIFF" || bad "camsync swap fixture not caught: $(grep -c 'finite cases DIFF' $O/caxn.txt) of 1 DIFF"
fi
# rwleaf (rw6): rw_rwleaf_check.sh must FAIL on the swap fixture (two input words swapped on the DLL side, inputs restored after the call) and pass without it; ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_rwleaf_bind $RD 2>/dev/null; then echo "negtest note: $RD has no rwleaf exports, rwleaf fixture skipped"; else
  OUT=$O/rwxg $T/rw_rwleaf_check.sh $RD 2000 > $O/rwxg.txt 2>&1 && ok "rwleaf good: $(grep -c EQUAL $O/rwxg.txt) entries EQUAL" || bad "rwleaf good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/rwxg.txt | head -2)"
  OUT=$O/rwxn $T/rw_rwleaf_check.sh $RD 2000 all neg > $O/rwxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/rwxn.txt) = 5 ] && ok "rwleaf swap: all 5 entries DIFF" || bad "rwleaf swap fixture not caught: $(grep -c 'finite cases DIFF' $O/rwxn.txt) of 5 DIFF"
fi
# d3d9vc (rw7): rw_d3d9vc_check.sh must FAIL on the swap fixture (the DLL reads bytes x and y of every normal swapped) and pass without it; ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q rw37__rpD3D9VertexDeclarationInstV3dComp $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9vc exports, d3d9vc fixture skipped"; else
  OUT=$O/d3vcxg $T/rw_d3d9vc_check.sh $RD 2000 > $O/d3vcxg.txt 2>&1 && ok "d3d9vc good: $(grep -c EQUAL $O/d3vcxg.txt) entries EQUAL" || bad "d3d9vc good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/d3vcxg.txt | head -2)"
  OUT=$O/d3vcxn $T/rw_d3d9vc_check.sh $RD 2000 all neg > $O/d3vcxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/d3vcxn.txt) = 1 ] && ok "d3d9vc swap: all 1 entries DIFF" || bad "d3d9vc swap fixture not caught: $(grep -c 'finite cases DIFF' $O/d3vcxn.txt) of 1 DIFF"
fi
# rwleaf2 (rw7): rw_rwleaf2_check.sh must FAIL on the swap fixture (a changed input on the DLL side) and pass without it; ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_rwleaf2_bind $RD 2>/dev/null; then echo "negtest note: $RD has no rwleaf2 exports, rwleaf2 fixture skipped"; else
  OUT=$O/rwf2xg $T/rw_rwleaf2_check.sh $RD 2000 > $O/rwf2xg.txt 2>&1 && ok "rwleaf2 good: $(grep -c EQUAL $O/rwf2xg.txt) entries EQUAL" || bad "rwleaf2 good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/rwf2xg.txt | head -2)"
  OUT=$O/rwf2xn $T/rw_rwleaf2_check.sh $RD 2000 all neg > $O/rwf2xn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/rwf2xn.txt) = 5 ] && ok "rwleaf2 swap: all 5 entries DIFF" || bad "rwleaf2 swap fixture not caught: $(grep -c 'finite cases DIFF' $O/rwf2xn.txt) of 5 DIFF"
fi
# splopen (rw7): rw_splopen_check.sh must FAIL on the swap fixture (a changed input on the DLL side) and pass without it; ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_splopen_bind $RD 2>/dev/null; then echo "negtest note: $RD has no splopen exports, splopen fixture skipped"; else
  OUT=$O/spenxg $T/rw_splopen_check.sh $RD 2000 > $O/spenxg.txt 2>&1 && ok "splopen good: $(grep -c EQUAL $O/spenxg.txt) entries EQUAL" || bad "splopen good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/spenxg.txt | head -2)"
  OUT=$O/spenxn $T/rw_splopen_check.sh $RD 2000 all neg > $O/spenxn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/spenxn.txt) = 3 ] && ok "splopen swap: all 3 entries DIFF" || bad "splopen swap fixture not caught: $(grep -c 'finite cases DIFF' $O/spenxn.txt) of 3 DIFF"
fi
# imgread (L3c): rw_imgread_check.sh must FAIL on the swap fixture (the DLL reads two distinct pixel bytes swapped) and pass without it; ~30 s, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_imgread_bind $RD 2>/dev/null; then echo "negtest note: $RD has no imgread exports, imgread fixture skipped"; else
  OUT=$O/ig $T/rw_imgread_check.sh $RD 20000 > $O/ig.txt 2>&1 && ok "imgread good: $(grep -c EQUAL $O/ig.txt) entries EQUAL" || bad "imgread good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/ig.txt | head -2)"
  OUT=$O/in $T/rw_imgread_check.sh $RD 20000 all neg > $O/in.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/in.txt) = 1 ] && ok "imgread swap: the entry DIFF" || bad "imgread swap fixture not caught: $(grep -E 'finite cases' $O/in.txt | head -2)"
fi
rm -r $O
exit $rc
