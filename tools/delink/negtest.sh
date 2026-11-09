#!/bin/bash
# negtest.sh: negative fixtures of the whole-program oracles (plan_testing item 2), no game run, about 5 s.  Every oracle must FAIL on its stored bad input and pass on the good one:
#   data    sa_oracles.sh data on tools/negtests/data/{good,bad}/ours.map (bad = T-P15: .data 00052004H)
#   render  d3dtrace/drawcmp.py with the gate's course:0 limits (tools/d3dtrace/render_limits.txt; 0 1 0 without it) on frames 0-2 of kept traces (tools/d3dtrace/ref/neg_*.d3d.gz):
#           neg_tp18 = link 1556 (DrawIndexedPrimitive Base/MinVertexIndex swapped), neg_tp19 = link 2152 (timer_ticks high dword), neg_good = gate 1103_0951 ours, neg_ctrl = its control
#   scen    the verdict() function of sa_oracles.sh (extracted, not copied) on saved scenario logs tools/negtests/scen/*.log (crash = link 1228 T-P17)
#   lints   defectlint.py --selftest, stack_lint.py --selftest, harness_drift.py --selftest (M4; skipped without a standalone link under output/gate), trapcheck.py --selftest on the baseline link
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
python3 $G/tools/d3dtrace/drawcmp.py $R/neg_ringshift.d3d.gz $R/neg_ctrl.d3d.gz $LIM > $O/rrs.txt && ok "render ringshift (dyn VB ring moved): $(head -1 $O/rrs.txt | cut -c1-140)" || bad "render ringshift rejected: $(head -2 $O/rrs.txt)"
python3 $G/tools/d3dtrace/drawcmp.py $R/neg_ringbad.d3d.gz $R/neg_ctrl.d3d.gz $LIM | grep -q "(vh) differs 1 " && ok "render ringbad: ring content counter 1" || bad "render ringbad: ring content counter not 1"
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
BL=$(awk '$1=="#" && $2=="link" {print $3; exit}' $G/output/zero_original_baseline.txt 2>/dev/null)      # the baseline link (gate.sh writes it with the baseline); none yet = newest link
python3 $T/../trapcheck.py --selftest ${BL:+$G/$BL} > $O/tc.txt 2>&1 && ok "$(tail -1 $O/tc.txt)" || { grep FAIL $O/tc.txt | head -5; bad "$(tail -1 $O/tc.txt)"; }
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
  # R1: planted unbalanced fld1 in front of each DLL entry: the callee-saved / x87 TOP + tag check must give REGDIFF on all 26
  OUT=$O/rwf $T/rw_rwmath_check.sh $RD 2000 all fld > $O/rwf.txt 2>&1; [ $(grep -c 'regs/x87 REGDIFF' $O/rwf.txt) = 26 ] && ok "rwmath fld plant: all 26 entries REGDIFF" || bad "rwmath fld plant not caught: $(grep -c 'regs/x87 REGDIFF' $O/rwf.txt) of 26 REGDIFF"
fi
# rt2d (R2): rw_rt2d_check.sh must FAIL on the input-change fixture (DIFF on all 9) and the planted unbalanced fld (REGDIFF on all 9) and pass without them; ~40 s, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_rt2d_bind $RD 2>/dev/null; then echo "negtest note: $RD has no rt2d exports, rt2d fixture skipped"; else
  OUT=$O/r2g $T/rw_rt2d_check.sh $RD 2000 > $O/r2g.txt 2>&1 && ok "rt2d good: $(grep -c 'finite cases EQUAL' $O/r2g.txt) entries EQUAL" || bad "rt2d good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/r2g.txt | head -2)"
  OUT=$O/r2n $T/rw_rt2d_check.sh $RD 2000 all neg > $O/r2n.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/r2n.txt) = 9 ] && ok "rt2d input fixture: all 9 entries DIFF" || bad "rt2d input fixture not caught: $(grep -c 'finite cases DIFF' $O/r2n.txt) of 9 DIFF"
  OUT=$O/r2f $T/rw_rt2d_check.sh $RD 2000 all fld > $O/r2f.txt 2>&1; [ $(grep -c 'regs/x87 REGDIFF' $O/r2f.txt) = 9 ] && ok "rt2d fld plant: all 9 entries REGDIFF" || bad "rt2d fld plant not caught: $(grep -c 'regs/x87 REGDIFF' $O/r2f.txt) of 9 REGDIFF"
fi
# pngread (R3): rw_pngread_check.sh must pass on the game's PNGs + corrupt variants and FAIL on the IHDR fixture; ~3 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_pngread_bind $RD 2>/dev/null; then echo "negtest note: $RD has no pngread exports, pngread fixture skipped"; else
  OUT=$O/pgg $T/rw_pngread_check.sh $RD 10 > $O/pgg.txt 2>&1 && ok "pngread good: $(grep -c 'B  EQUAL' $O/pgg.txt) files EQUAL" || bad "pngread good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/pgg.txt | head -2)"
  OUT=$O/pgn $T/rw_pngread_check.sh $RD 0 neg > $O/pgn.txt 2>&1; [ $(grep -c 'B  DIFF' $O/pgn.txt) = $(grep -c 'B  EQUAL' $O/pgg.txt) ] && ok "pngread IHDR fixture: every file DIFF" || bad "pngread IHDR fixture not caught: $(grep -c 'B  DIFF' $O/pgn.txt) files DIFF"
fi
# lod (R4): rw_lod_check.sh must FAIL on the input fixture (DIFF on all 4), the fld plant (REGDIFF on all 4) and the lrintf fixture DLL (DIFF on the selector); ~40 s, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_lod_bind $RD 2>/dev/null; then echo "negtest note: $RD has no lod exports, lod fixture skipped"; else
  OUT=$O/lg $T/rw_lod_check.sh $RD 2000 > $O/lg.txt 2>&1 && ok "lod good: $(grep -c 'finite cases EQUAL' $O/lg.txt) entries EQUAL" || bad "lod good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/lg.txt | head -2)"
  OUT=$O/ln $T/rw_lod_check.sh $RD 2000 all neg > $O/ln.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/ln.txt) = 4 ] && ok "lod input fixture: all 4 entries DIFF" || bad "lod input fixture not caught: $(grep -c 'finite cases DIFF' $O/ln.txt) of 4 DIFF"
  OUT=$O/lf $T/rw_lod_check.sh $RD 2000 all fld > $O/lf.txt 2>&1; [ $(grep -c 'regs/x87 REGDIFF' $O/lf.txt) = 4 ] && ok "lod fld plant: all 4 entries REGDIFF" || bad "lod fld plant not caught: $(grep -c 'regs/x87 REGDIFF' $O/lf.txt) of 4 REGDIFF"
  OUT=$O/lr $T/rw_lod_check.sh lrintf 2000 > $O/lr.txt 2>&1; grep -q 'LODAtomicCallBackSelectLOD .*finite cases DIFF' $O/lr.txt && ok "lod lrintf fixture: selector DIFF" || bad "lod lrintf fixture not caught"
fi
# pixfmt (rw4): rw_pixfmt_check.sh must FAIL on the swap fixture (input bytes 0 <-> 2 swapped on the DLL side, inputs restored after the call) and pass without it; ~60 s, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_pixfmt_bind $RD 2>/dev/null; then echo "negtest note: $RD has no pixfmt exports, pixfmt fixture skipped"; else
  OUT=$O/pig $T/rw_pixfmt_check.sh $RD 2000 > $O/pig.txt 2>&1 && ok "pixfmt good: $(grep -c EQUAL $O/pig.txt) entries EQUAL" || bad "pixfmt good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/pig.txt | head -2)"
  OUT=$O/pin $T/rw_pixfmt_check.sh $RD 2000 all neg > $O/pin.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/pin.txt) = 8 ] && ok "pixfmt swap: all 8 entries DIFF" || bad "pixfmt swap fixture not caught: $(grep -c 'finite cases DIFF' $O/pin.txt) of 8 DIFF"
fi
# d3dconv (engine switch X1): rw_d3dconv_check.sh must FAIL on the channel swap fixture (bytes 1 <-> 3 of the DLL's pixel / palette inputs) on all 11 pixel
# entries and pass without it; ~10 s, docker
if ! grep -q mflrw_d3dconv_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3dconv exports, d3dconv fixture skipped"; else
  OUT=$O/dcg $T/rw_d3dconv_check.sh $RD 2000 > $O/dcg.txt 2>&1 && ok "d3dconv good: $(grep -c EQUAL $O/dcg.txt) entries EQUAL" || bad "d3dconv good rejected: $(grep -E 'DIFF|MISSING|TIMEOUT|CRASH' $O/dcg.txt | head -2)"
  OUT=$O/dcn $T/rw_d3dconv_check.sh $RD 2000 all neg > $O/dcn.txt 2>&1; [ $(grep -c 'finite cases DIFF' $O/dcn.txt) = 11 ] && ok "d3dconv swap: all 11 pixel entries DIFF" || bad "d3dconv swap fixture not caught: $(grep -c 'finite cases DIFF' $O/dcn.txt) of 11 DIFF"
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
# d3drs (engine switch D1): rw_d3drs_check.sh must pass and DIFF on two mutations of the original: (a) FlushCache skips its redundant-state compare
# (je 0x4e3b5e -> nop nop: an extra device SetRenderState), (b) the stencil-enable setter drops its dirty-list append (inc eax 0x4e5996 -> nop); ~1 min, docker
RD=$G/output/librw/mflrw.dll
if ! grep -q mflrw_d3d_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3drs exports, d3drs fixtures skipped"; else
  OUT=$O/d3rg $T/rw_d3drs_check.sh $RD 10 1000 > $O/d3rg.txt 2>&1 && ok "d3drs good: $(grep -c EQUAL $O/d3rg.txt) run EQUAL" || bad "d3drs good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/d3rg.txt | head -2)"
  OUT=$O/d3ra $T/rw_d3drs_check.sh $RD 10 1000 4e3b5e:90 4e3b5f:90 > $O/d3ra.txt 2>&1; grep -q 'compared; DIFF' $O/d3ra.txt && ok "d3drs redundant-compare skip: DIFF" || bad "d3drs redundant-compare fixture not caught"
  OUT=$O/d3rb $T/rw_d3drs_check.sh $RD 10 1000 4e5996:90 > $O/d3rb.txt 2>&1; grep -q 'compared; DIFF' $O/d3rb.txt && ok "d3drs dropped dirty append: DIFF" || bad "d3drs dirty-append fixture not caught"
fi
# d3dim2d (engine switch D2): rw_d3dim2d_check.sh must pass and DIFF on two mutations of the original: (a) the dynamic ring lock appends with DISCARD instead of
# NOOVERWRITE (push 0x1800 -> 0x2800 at 0x4ecdfc: a wrong discard flag), (b) ForceDiscard wraps the ring after 3 slots (cmp eax,4 -> 3 at 0x4ec9de); ~1 min, docker
if ! grep -q mflrw_d3dim2d_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3dim2d exports, d3dim2d fixtures skipped"; else
  OUT=$O/d2g $T/rw_d3dim2d_check.sh $RD 10 1000 > $O/d2g.txt 2>&1 && ok "d3dim2d good: $(grep -c EQUAL $O/d2g.txt) run EQUAL" || bad "d3dim2d good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/d2g.txt | head -2)"
  OUT=$O/d2a $T/rw_d3dim2d_check.sh $RD 10 1000 4ecdfc:28 > $O/d2a.txt 2>&1; grep -q 'compared; DIFF' $O/d2a.txt && ok "d3dim2d discard flag: DIFF" || bad "d3dim2d discard-flag fixture not caught"
  OUT=$O/d2b $T/rw_d3dim2d_check.sh $RD 10 1000 4ec9de:03 > $O/d2b.txt 2>&1; grep -q 'compared; DIFF' $O/d2b.txt && ok "d3dim2d ring wrap: DIFF" || bad "d3dim2d ring-wrap fixture not caught"
fi
# d3dras (engine switch window 2, X2): rw_d3dras_check.sh must pass and DIFF on two mutations of the original: (a) a palettised PAL8 raster with a given colour format gets
# depth 4 instead of 8 (mov [esi+0x14],8 -> 4 at 0x4e2284 in _rwD3D9RasterCreate), (b) CreateTextureRaster drops the mip count (levels = 0 always: and esi,1 -> 0 at
# 0x4e28f7); ~1 min, docker
# Window 3 (d3dras2, the check runs both groups): 8 more mutations, one per new path (agents/laneX_w3_negs.py lists them): camera texture retry format (DISPFMT read ->
# BackBufferHeight, 4e2b03 / restore 4e1d1d), z buffer D24X8 fallback 0x4d -> 0x4e (4e2d61), restore D24S8 choice 0x4b -> 0x4c (4e1e5e), swap chain EnableAutoDepthStencil 0 -> 1 (4e1ff9),
# back buffer depth byte -> alpha byte (4e207b), release depth range 0x50 -> 0x4f (4e1b67), mip levels inc edx -> inc ecx (4e3264)
if ! grep -q mflrw_d3dras2_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3dras2 exports, d3dras / d3dras2 fixtures skipped"; else
  for m in 4e2b03:44 4e1d1d:44 4e2d61:4e 4e1ff9:01 4e207b:60 4e1b67:4f 4e3264:41 4e1e5e:4c; do
    n="10 1000"; [ $m = 4e1e5e:4c ] && n="20 3000"      # a released own-surface z buffer restored on a 32-bit z device: first seen at seed 20
    OUT=$O/xr2 $T/rw_d3dras_check.sh $RD $n $m > $O/xr2.txt 2>&1; grep -q 'compared; DIFF' $O/xr2.txt && ok "d3dras2 $m: DIFF" || bad "d3dras2 fixture $m not caught"
  done
  OUT=$O/xrg $T/rw_d3dras_check.sh $RD 10 1000 > $O/xrg.txt 2>&1 && ok "d3dras good: $(grep -c EQUAL $O/xrg.txt) run EQUAL" || bad "d3dras good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/xrg.txt | head -2)"
  OUT=$O/xra $T/rw_d3dras_check.sh $RD 10 1000 4e2284:04 > $O/xra.txt 2>&1; grep -q 'compared; DIFF' $O/xra.txt && ok "d3dras palettised depth 8 -> 4: DIFF" || bad "d3dras palettised-depth fixture not caught"
  OUT=$O/xrb $T/rw_d3dras_check.sh $RD 10 1000 4e28f7:00 > $O/xrb.txt 2>&1; grep -q 'compared; DIFF' $O/xrb.txt && ok "d3dras mip count dropped: DIFF" || bad "d3dras mip-count fixture not caught"
fi
# d3dlights (engine switch window 2, D3): rw_d3dlights_check.sh must pass and DIFF on two mutations of the original: (a) LightsEnable drops the LightEnable(FALSE) of a
# light gone since the last frame (call 0x4db830 at 0x505b77 -> 5 nops), (b) RwD3D9SetLight skips the LightsEqual compare (jne at 0x4db7eb -> 2 nops: an unchanged
# light is sent again); ~1 min, docker
if ! grep -q mflrw_d3dlights_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3dlights exports, d3dlights fixtures skipped"; else
  OUT=$O/d3g $T/rw_d3dlights_check.sh $RD 10 1000 > $O/d3g.txt 2>&1 && ok "d3dlights good: $(grep -c EQUAL $O/d3g.txt) run EQUAL" || bad "d3dlights good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/d3g.txt | head -2)"
  OUT=$O/d3a $T/rw_d3dlights_check.sh $RD 10 1000 505b77:90 505b78:90 505b79:90 505b7a:90 505b7b:90 > $O/d3a.txt 2>&1; grep -q 'compared; DIFF' $O/d3a.txt && ok "d3dlights dropped LightEnable(FALSE): DIFF" || bad "d3dlights LightEnable(FALSE) fixture not caught"
  OUT=$O/d3b $T/rw_d3dlights_check.sh $RD 10 1000 4db7eb:90 4db7ec:90 > $O/d3b.txt 2>&1; grep -q 'compared; DIFF' $O/d3b.txt && ok "d3dlights LightsEqual compare skipped: DIFF" || bad "d3dlights LightsEqual fixture not caught"
fi
# d3dxform (engine switch window 2, D4): rw_d3dxform_check.sh must pass and DIFF on two mutations of the original: (a) RwD3D9SetTransform drops the cache compare
# (jne -> jmp at 0x4db4a1: an unchanged matrix is sent again), (b) _rwD3D9SetStreams passes a wrong stride (push edx -> push ecx at 0x4db31b); ~1 min, docker
if ! grep -q mflrw_d3dxform_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3dxform exports, d3dxform fixtures skipped"; else
  OUT=$O/d4g $T/rw_d3dxform_check.sh $RD 10 1000 > $O/d4g.txt 2>&1 && ok "d3dxform good: $(grep -c EQUAL $O/d4g.txt) run EQUAL" || bad "d3dxform good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/d4g.txt | head -2)"
  OUT=$O/d4a $T/rw_d3dxform_check.sh $RD 10 1000 4db4a1:eb > $O/d4a.txt 2>&1; grep -q 'compared; DIFF' $O/d4a.txt && ok "d3dxform transform cache compare dropped: DIFF" || bad "d3dxform transform-cache fixture not caught"
  OUT=$O/d4b $T/rw_d3dxform_check.sh $RD 10 1000 4db31b:51 > $O/d4b.txt 2>&1; grep -q 'compared; DIFF' $O/d4b.txt && ok "d3dxform wrong stream stride: DIFF" || bad "d3dxform stream-stride fixture not caught"
fi
# d3d9pipe (engine switch window 3, D5): rw_d3d9pipe_check.sh must pass and DIFF on three mutations of the original: (a) InstanceDataDestroy drops the owner type 8
# test (jne -> nops at 0x4fa97f: the atomic's geometry is not looked up), (b) rxD3D9Instance sorts on primitive type 5 instead of 4 (0x4fad10 04 -> 05),
# (c) rxD3D9DefaultRenderCallback resets stage 1 on the wrong condition (jne -> je at 0x4fb364); ~1 min, docker
if ! grep -q mflrw_d3d9pipe_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9pipe exports, d3d9pipe fixtures skipped"; else
  OUT=$O/d5g $T/rw_d3d9pipe_check.sh $RD 10 1000 > $O/d5g.txt 2>&1 && ok "d3d9pipe good: $(grep -c EQUAL $O/d5g.txt) run EQUAL" || bad "d3d9pipe good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/d5g.txt | head -2)"
  OUT=$O/d5a $T/rw_d3d9pipe_check.sh $RD 10 1000 4fa97f:90 4fa980:90 > $O/d5a.txt 2>&1; grep -q 'compared; DIFF' $O/d5a.txt && ok "d3d9pipe destroy owner type test dropped: DIFF" || bad "d3d9pipe destroy-owner fixture not caught"
  OUT=$O/d5b $T/rw_d3d9pipe_check.sh $RD 10 1000 4fad10:05 > $O/d5b.txt 2>&1; grep -q 'compared; DIFF' $O/d5b.txt && ok "d3d9pipe instance sort on wrong primitive type: DIFF" || bad "d3d9pipe instance-sort fixture not caught"
  OUT=$O/d5c $T/rw_d3d9pipe_check.sh $RD 10 1000 4fb364:74 > $O/d5c.txt 2>&1; grep -q 'compared; DIFF' $O/d5c.txt && ok "d3d9pipe render stage-1 reset inverted: DIFF" || bad "d3d9pipe render-stage fixture not caught"
fi
# d3d9texdic (engine switch window 3, D6): rw_d3d9texdic_check.sh must pass and DIFF on two mutations of the original: (a) NativeTextureRead steps the cube face index
# by 2 (0x4edd09 10 -> 20), (b) NativeTextureGetSize counts a PAL4 header one byte too big (0x4ed2a6 e4 -> e5); ~1 min, docker
if ! grep -q mflrw_d3d9texdic_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9texdic exports, d3d9texdic fixtures skipped"; else
  OUT=$O/d6g $T/rw_d3d9texdic_check.sh $RD 10 1000 > $O/d6g.txt 2>&1 && ok "d3d9texdic good: $(grep -c EQUAL $O/d6g.txt) run EQUAL" || bad "d3d9texdic good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/d6g.txt | head -2)"
  OUT=$O/d6a $T/rw_d3d9texdic_check.sh $RD 10 1000 4edd09:20 > $O/d6a.txt 2>&1; grep -q 'compared; DIFF' $O/d6a.txt && ok "d3d9texdic cube face step wrong: DIFF" || bad "d3d9texdic face-step fixture not caught"
  OUT=$O/d6b $T/rw_d3d9texdic_check.sh $RD 10 1000 4ed2a6:e5 > $O/d6b.txt 2>&1; grep -q 'compared; DIFF' $O/d6b.txt && ok "d3d9texdic PAL4 header size wrong: DIFF" || bad "d3d9texdic header-size fixture not caught"
fi
# d3d9vdtan (engine switch window 3, D6): rw_d3d9vdtan_check.sh must pass and DIFF on two mutations of the original FUN_00508600: (a) the tangent cosine merge test
# inverted (jne -> je at 0x508b51), (b) the x determinant test inverted (jne -> je at 0x508805); ~1 min, docker
if ! grep -q mflrw_d3d9vdtan_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9vdtan exports, d3d9vdtan fixtures skipped"; else
  OUT=$O/d7g $T/rw_d3d9vdtan_check.sh $RD 10 1000 > $O/d7g.txt 2>&1 && ok "d3d9vdtan good: $(grep -c EQUAL $O/d7g.txt) run EQUAL" || bad "d3d9vdtan good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/d7g.txt | head -2)"
  OUT=$O/d7a $T/rw_d3d9vdtan_check.sh $RD 10 1000 508b51:74 > $O/d7a.txt 2>&1; grep -q 'compared; DIFF' $O/d7a.txt && ok "d3d9vdtan tangent merge test inverted: DIFF" || bad "d3d9vdtan merge fixture not caught"
  OUT=$O/d7b $T/rw_d3d9vdtan_check.sh $RD 10 1000 508805:74 > $O/d7b.txt 2>&1; grep -q 'compared; DIFF' $O/d7b.txt && ok "d3d9vdtan det test inverted: DIFF" || bad "d3d9vdtan det fixture not caught"
fi
rm -r $O
exit $rc
