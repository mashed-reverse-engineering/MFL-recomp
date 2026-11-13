#!/bin/bash
# negtest.sh: negative fixtures of the whole-program oracles (plan_testing item 2), no game run, about 5 s.  Every oracle must FAIL on its stored bad input and pass on the good one:
#   data    sa_oracles.sh data on tools/negtests/data/{good,bad}/ours.map (bad = T-P15: .data 00052004H)
#   render  d3dtrace/drawcmp.py with the gate's course:0 limits (tools/d3dtrace/render_limits.txt; 0 1 0 without it) on frames 0-2 of kept traces (tools/d3dtrace/ref/neg_*.d3d.gz):
#           neg_tp18 = link 1556 (DrawIndexedPrimitive Base/MinVertexIndex swapped), neg_tp19 = link 2152 (timer_ticks high dword), neg_good = gate 1103_0951 ours, neg_ctrl = its control
#   scen    the verdict() function of sa_oracles.sh (extracted, not copied) on saved scenario logs tools/negtests/scen/*.log (crash = link 1228 T-P17)
#   DLL groups (from collis on): docker, ~30 min; skipped with NEGTEST_SKIP_DLL=1
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
# ---- everything below is the DLL groups (docker, ~30 min).  NEGTEST_SKIP_DLL=1 (check_all.sh, cache HIT of tools/negtest_key.py) skips them; the fast part above always runs
if [ "${NEGTEST_SKIP_DLL:-0}" = 1 ]; then echo "negtest note: DLL groups skipped (check_all cache HIT, tools/negtest_key.py)"; else
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
# texread (engine switch window 5, lane K): rw_texread_check.sh must DIFF on four mutations of the original: (a) RwImageRead searches for '\\' instead of '/' (push imm 0x2f -> 0x5c
# at 0x4ddc6a), (b) RwImageStreamRead takes the stride-equal branch the other way (jne -> je at 0x4de7e4), (c) TextureDefaultFind folds 'z' wrongly (cmp al, 0x7a -> 0x79 at 0x4d7674),
# (d) RwTextureStreamRead leaves filter 4 out of the mipmapped set (cmp esi, 4 -> 7 at 0x4df3fc); ~1 min each, docker.  The good run is the texread2 one below (a superset).
if ! grep -q mflrw_texread_bind $RD 2>/dev/null; then echo "negtest note: $RD has no texread exports, texread fixtures skipped"; else
  OUT=$O/e1a $T/rw_texread_check.sh $RD 20 300 4ddc6a:5c > $O/e1a.txt 2>&1; grep -q 'compared; DIFF' $O/e1a.txt && ok "texread image extension search on wrong separator: DIFF" || bad "texread image-search fixture not caught"
  OUT=$O/e1b $T/rw_texread_check.sh $RD 20 300 4de7e4:74 > $O/e1b.txt 2>&1; grep -q 'compared; DIFF' $O/e1b.txt && ok "texread image stream stride test inverted: DIFF" || bad "texread image-stream fixture not caught"
  OUT=$O/e1c $T/rw_texread_check.sh $RD 20 300 4d7674:79 > $O/e1c.txt 2>&1; grep -q 'compared; DIFF' $O/e1c.txt && ok "texread default find folds z wrongly: DIFF" || bad "texread default-find fixture not caught"
  OUT=$O/e1d $T/rw_texread_check.sh $RD 20 300 4df3fc:07 > $O/e1d.txt 2>&1; grep -q 'compared; DIFF' $O/e1d.txt && ok "texread stream filter 4 not mipmapped: DIFF" || bad "texread texture-stream fixture not caught"
fi
# texread2 (window 5, lane K batches 2 / 3): rw_texread2_check.sh (re-runs the texread rows too) must pass and DIFF on (e) RwImageSetGamma loop bound (cmp esi, 0x100 -> 0 at 0x4deb3e),
# (f) rwTextureOpen stores a wrong default read callback (0x4d6340 -> 0x4d6341 at 0x4d6277), (g) PalettizeMipmaps shared palette test inverted (jne -> je at 0x4d67af),
# (h) TextureRasterDefaultBuildMipmaps lock mode (push 2 -> 3 at 0x4d7469); ~1 min each, docker
if ! grep -q mflrw_texread2_bind $RD 2>/dev/null; then echo "negtest note: $RD has no texread2 exports, texread2 fixtures skipped"; else
  OUT=$O/e2g $T/rw_texread2_check.sh $RD 20 300 > $O/e2g.txt 2>&1 && ok "texread2 good: $(grep -c EQUAL $O/e2g.txt) run EQUAL" || bad "texread2 good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e2g.txt | head -2)"
  OUT=$O/e2e $T/rw_texread2_check.sh $RD 20 300 4deb3e:00 > $O/e2e.txt 2>&1; grep -q 'compared; DIFF' $O/e2e.txt && ok "texread2 gamma table loop bound wrong: DIFF" || bad "texread2 gamma fixture not caught"
  OUT=$O/e2f $T/rw_texread2_check.sh $RD 20 300 4d6277:41 > $O/e2f.txt 2>&1; grep -q 'compared; DIFF' $O/e2f.txt && ok "texread2 texture open callback wrong: DIFF" || bad "texread2 texture-open fixture not caught"
  OUT=$O/e2g2 $T/rw_texread2_check.sh $RD 20 300 4d67af:74 > $O/e2g2.txt 2>&1; grep -q 'compared; DIFF' $O/e2g2.txt && ok "texread2 palettize shared-palette test inverted: DIFF" || bad "texread2 palettize fixture not caught"
  OUT=$O/e2h $T/rw_texread2_check.sh $RD 20 300 4d7469:03 > $O/e2h.txt 2>&1; grep -q 'compared; DIFF' $O/e2h.txt && ok "texread2 build mipmaps lock mode wrong: DIFF" || bad "texread2 build-mipmaps fixture not caught"
fi
# matfx (window 5, lane K batches 2 / 3): rw_matfx_check.sh must pass and DIFF on (a) user data array element size (mov eax, 4 -> 8 at 0x554210), (b) atomic instance callback
# material type test (sub eax, 2 -> 1 at 0x553f11), (c) world sector instance callback, same test (0x553f89), (d) bump map texture compression test inverted (jne -> je at 0x5540e2),
# (e) render callback type range (cmp edx, 5 -> 4 at 0x553bfc), (f) DefaultRender lit pass flag test (0x5508de 84 -> 80), (g) env map projection constant (0x55211e 03 -> 04),
# (h) cube matrix w (0x5521bd 3f -> 3e), (i) PipelinesCreate blend stage test (0x553dbf 02 -> 03), (j) pixel shader version test (0x553e2d 01 -> 02), (k) / (l) EnvRender
# (0x55199d -> fe, 0x551ed3 -> 3d), (m) / (n) BumpMapRender (0x55234d -> 53, 0x552314 -> 2d); ~1 min each, docker
if ! grep -q mflrw_matfx_bind $RD 2>/dev/null; then echo "negtest note: $RD has no matfx exports, matfx fixtures skipped"; else
  OUT=$O/e3g $T/rw_matfx_check.sh $RD 20 300 > $O/e3g.txt 2>&1 && ok "matfx good: $(grep -c EQUAL $O/e3g.txt) run EQUAL" || bad "matfx good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e3g.txt | head -2)"
  OUT=$O/e3a $T/rw_matfx_check.sh $RD 20 300 554210:08 > $O/e3a.txt 2>&1; grep -q 'compared; DIFF' $O/e3a.txt && ok "matfx user data element size wrong: DIFF" || bad "matfx user-data fixture not caught"
  OUT=$O/e3b $T/rw_matfx_check.sh $RD 20 300 553f11:01 > $O/e3b.txt 2>&1; grep -q 'compared; DIFF' $O/e3b.txt && ok "matfx atomic instance material type test wrong: DIFF" || bad "matfx atomic-instance fixture not caught"
  OUT=$O/e3c $T/rw_matfx_check.sh $RD 20 300 553f89:01 > $O/e3c.txt 2>&1; grep -q 'compared; DIFF' $O/e3c.txt && ok "matfx sector instance material type test wrong: DIFF" || bad "matfx sector-instance fixture not caught"
  OUT=$O/e3d $T/rw_matfx_check.sh $RD 20 300 5540e2:74 > $O/e3d.txt 2>&1; grep -q 'compared; DIFF' $O/e3d.txt && ok "matfx bump texture compression test inverted: DIFF" || bad "matfx bump-texture fixture not caught"
  OUT=$O/e3e $T/rw_matfx_check.sh $RD 20 300 553bfc:04 > $O/e3e.txt 2>&1; grep -q 'compared; DIFF' $O/e3e.txt && ok "matfx render callback type range wrong: DIFF" || bad "matfx render-callback fixture not caught"
  OUT=$O/e3f $T/rw_matfx_check.sh $RD 20 300 5508de:80 > $O/e3f.txt 2>&1; grep -q 'compared; DIFF' $O/e3f.txt && ok "matfx default render lit flag test wrong: DIFF" || bad "matfx default-render fixture not caught"
  OUT=$O/e3g2 $T/rw_matfx_check.sh $RD 20 300 55211e:04 > $O/e3g2.txt 2>&1; grep -q 'compared; DIFF' $O/e3g2.txt && ok "matfx env map projection constant wrong: DIFF" || bad "matfx env-matrix fixture not caught"
  OUT=$O/e3h $T/rw_matfx_check.sh $RD 20 300 5521bd:3e > $O/e3h.txt 2>&1; grep -q 'compared; DIFF' $O/e3h.txt && ok "matfx cube matrix w wrong: DIFF" || bad "matfx cube-matrix fixture not caught"
  OUT=$O/e3i $T/rw_matfx_check.sh $RD 20 300 553dbf:03 > $O/e3i.txt 2>&1; grep -q 'compared; DIFF' $O/e3i.txt && ok "matfx pipelines create blend stage test wrong: DIFF" || bad "matfx pipelines fixture not caught"
  OUT=$O/e3j $T/rw_matfx_check.sh $RD 20 300 553e2d:02 > $O/e3j.txt 2>&1; grep -q 'compared; DIFF' $O/e3j.txt && ok "matfx pixel shader version test wrong: DIFF" || bad "matfx shader fixture not caught"
  OUT=$O/e3k $T/rw_matfx_check.sh $RD 20 300 55199d:fe > $O/e3k.txt 2>&1; grep -q 'compared; DIFF' $O/e3k.txt && ok "matfx env render coefficient test wrong: DIFF" || bad "matfx env-render fixture k not caught"
  OUT=$O/e3l $T/rw_matfx_check.sh $RD 20 300 551ed3:3d > $O/e3l.txt 2>&1; grep -q 'compared; DIFF' $O/e3l.txt && ok "matfx env render texture factor state wrong: DIFF" || bad "matfx env-render fixture l not caught"
  OUT=$O/e3m $T/rw_matfx_check.sh $RD 20 300 55234d:53 > $O/e3m.txt 2>&1; grep -q 'compared; DIFF' $O/e3m.txt && ok "matfx bump render FVF wrong: DIFF" || bad "matfx bump-render fixture m not caught"
  OUT=$O/e3n $T/rw_matfx_check.sh $RD 20 300 552314:2d > $O/e3n.txt 2>&1; grep -q 'compared; DIFF' $O/e3n.txt && ok "matfx bump render lock stride wrong: DIFF" || bad "matfx bump-render fixture n not caught"
fi
# matfx2 (window 5, lane K batch 4): rw_matfx2_check.sh must pass and DIFF on mutations of the original:
# DualPass: (a) source blend test 9 -> 10 (cmp eax, 9 at 0x550cd1), (c) (5,6) case source test 5 -> 6 (0x550e61).  CalculatePerturbedUVs: (e) vertex tangent path reads the wrong
# light component (fld [esp+0xd0] disp at 0x552d33), (h) uv scale (fmul [esp+0x70] disp at 0x552e87), (j) tristrip first two vertices test 2 -> 3 (0x552fbd).
if ! grep -q mflrw_matfx2_bind $RD 2>/dev/null; then echo "negtest note: $RD has no matfx2 exports, matfx2 fixtures skipped"; else
  OUT=$O/e4g $T/rw_matfx2_check.sh $RD 20 300 > $O/e4g.txt 2>&1 && ok "matfx2 good: $(grep -c EQUAL $O/e4g.txt) run EQUAL" || bad "matfx2 good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e4g.txt | head -2)"
  OUT=$O/e4a $T/rw_matfx2_check.sh $RD 20 300 550cd1:0a > $O/e4a.txt 2>&1; grep -q 'compared; DIFF' $O/e4a.txt && ok "matfx2 dual pass blend source test wrong: DIFF" || bad "matfx2 dual-pass fixture a not caught"
  OUT=$O/e4b $T/rw_matfx2_check.sh $RD 20 300 550e61:06 > $O/e4b.txt 2>&1; grep -q 'compared; DIFF' $O/e4b.txt && ok "matfx2 dual pass (5,6) case wrong: DIFF" || bad "matfx2 dual-pass fixture c not caught"
  OUT=$O/e4e $T/rw_matfx2_check.sh $RD 20 300 552d33:d4 > $O/e4e.txt 2>&1; grep -q 'compared; DIFF' $O/e4e.txt && ok "matfx2 perturbed uv light component wrong: DIFF" || bad "matfx2 perturbed-uv fixture e not caught"
  OUT=$O/e4h $T/rw_matfx2_check.sh $RD 20 300 552e87:74 > $O/e4h.txt 2>&1; grep -q 'compared; DIFF' $O/e4h.txt && ok "matfx2 perturbed uv scale wrong: DIFF" || bad "matfx2 perturbed-uv fixture h not caught"
  OUT=$O/e4j $T/rw_matfx2_check.sh $RD 20 300 552fbd:03 > $O/e4j.txt 2>&1; grep -q 'compared; DIFF' $O/e4j.txt && ok "matfx2 perturbed uv tristrip start wrong: DIFF" || bad "matfx2 perturbed-uv fixture j not caught"
fi
# d3d9dev (window 5, lane K batch 4): rw_d3d9dev_check.sh must pass and DIFF on (a) _rwD3D9SetRenderTarget cache hit test inverted (je -> jne at 0x4d7bbf), (c) RwD3D9SetRenderTarget
# cube face shift 4 -> 5 (0x4db141), (e) RwD3D9EngineSetRefreshRate rate compare jae -> jb (0x4da09f), (g) rwD3D9CameraEndUpdate cType test 5 -> 6 (0x4dac94), (i) device state reset
# matrix cache loop bound 0x410 -> 0x40c (0x4db07a).
if ! grep -q mflrw_d3d9dev_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9dev exports, d3d9dev fixtures skipped"; else
  OUT=$O/e5g $T/rw_d3d9dev_check.sh $RD 20 300 > $O/e5g.txt 2>&1 && ok "d3d9dev good: $(grep -c EQUAL $O/e5g.txt) run EQUAL" || bad "d3d9dev good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e5g.txt | head -2)"
  OUT=$O/e5a $T/rw_d3d9dev_check.sh $RD 20 300 4d7bbf:75 > $O/e5a.txt 2>&1; grep -q 'compared; DIFF' $O/e5a.txt && ok "d3d9dev render target cache test inverted: DIFF" || bad "d3d9dev fixture a not caught"
  OUT=$O/e5c $T/rw_d3d9dev_check.sh $RD 20 300 4db141:05 > $O/e5c.txt 2>&1; grep -q 'compared; DIFF' $O/e5c.txt && ok "d3d9dev cube face shift wrong: DIFF" || bad "d3d9dev fixture c not caught"
  OUT=$O/e5e $T/rw_d3d9dev_check.sh $RD 20 300 4da09f:72 > $O/e5e.txt 2>&1; grep -q 'compared; DIFF' $O/e5e.txt && ok "d3d9dev refresh rate compare wrong: DIFF" || bad "d3d9dev fixture e not caught"
  OUT=$O/e5g2 $T/rw_d3d9dev_check.sh $RD 20 300 4dac94:06 > $O/e5g2.txt 2>&1; grep -q 'compared; DIFF' $O/e5g2.txt && ok "d3d9dev camera end update raster type wrong: DIFF" || bad "d3d9dev fixture g not caught"
  OUT=$O/e5i $T/rw_d3d9dev_check.sh $RD 20 300 4db07a:0c > $O/e5i.txt 2>&1; grep -q 'compared; DIFF' $O/e5i.txt && ok "d3d9dev state reset matrix loop wrong: DIFF" || bad "d3d9dev fixture i not caught"
fi
# d3d9dev2 (window 5, lane K batch 5): rw_d3d9dev2_check.sh must pass and DIFF on mutations of the original (one per function family):
# (a) CreateVertexDeclaration element count compare jne -> je (4dba19), (b) CameraAttachWindow raster flags 0x82 -> 0x83 (4da1e5),
# (c) D3D9DeviceSystemStop matrix cache loop bound 0x410 -> 0x40c (4d8e82), (d) rwD3D9BeginScene reset-failed constant 0x8876086c -> 0x8876086d (4d9e96), (e) ReleaseVideoMemory matrix loop bound
# (4d9cdf), (f) D3D9CreateDisplayModesList duplicate mode compare jne -> je (4d9345), (g) ShowRaster DEVICENOTRESET constant (4daed7), (h) SetPresentParameters depth constant 0x20 -> 0x21 (4d8ad7),
# (i) CameraClear viewport MaxZ 1.0 -> 0.5 (4d95bb), (j) CameraBeginUpdate projection type compare 2 -> 1 (4da38e), (k) RasterLockPalette flag 0x18 -> 0x10 (4e032d), (l) RenderGeneric source type
# compare 5 -> 4 (4e0442), (m) ConvertToNonPalettized non palettised format class 0x600 -> 0x500 (4e3abb).
if ! grep -q mflrw_d3d9dev2_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9dev2 exports, d3d9dev2 fixtures skipped"; else
  OUT=$O/e5k $T/rw_d3d9dev2_check.sh $RD 20 300 > $O/e5k.txt 2>&1 && ok "d3d9dev2 good: $(grep -c EQUAL $O/e5k.txt) run EQUAL" || bad "d3d9dev2 good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e5k.txt | head -2)"
  OUT=$O/e5ka $T/rw_d3d9dev2_check.sh $RD 20 300 4dba19:74 > $O/e5ka.txt 2>&1; grep -q 'compared; DIFF' $O/e5ka.txt && ok "d3d9dev2 vertex declaration count compare inverted: DIFF" || bad "d3d9dev2 fixture a not caught"
  OUT=$O/e5kb $T/rw_d3d9dev2_check.sh $RD 20 300 4da1e5:83 > $O/e5kb.txt 2>&1; grep -q 'compared; DIFF' $O/e5kb.txt && ok "d3d9dev2 attach window raster flags wrong: DIFF" || bad "d3d9dev2 fixture b not caught"
  OUT=$O/e5kc $T/rw_d3d9dev2_check.sh $RD 20 300 4d8e82:0c > $O/e5kc.txt 2>&1; grep -q 'compared; DIFF' $O/e5kc.txt && ok "d3d9dev2 system stop matrix loop wrong: DIFF" || bad "d3d9dev2 fixture c not caught"
  OUT=$O/e5kd $T/rw_d3d9dev2_check.sh $RD 20 300 4d9e96:6d > $O/e5kd.txt 2>&1; grep -q 'compared; DIFF' $O/e5kd.txt && ok "d3d9dev2 begin scene reset constant wrong: DIFF" || bad "d3d9dev2 fixture d not caught"
  OUT=$O/e5ke $T/rw_d3d9dev2_check.sh $RD 20 300 4d9cdf:0c > $O/e5ke.txt 2>&1; grep -q 'compared; DIFF' $O/e5ke.txt && ok "d3d9dev2 release video memory matrix loop wrong: DIFF" || bad "d3d9dev2 fixture e not caught"
  OUT=$O/e5kf $T/rw_d3d9dev2_check.sh $RD 20 300 4d9345:74 > $O/e5kf.txt 2>&1; grep -q 'compared; DIFF' $O/e5kf.txt && ok "d3d9dev2 display mode duplicate test inverted: DIFF" || bad "d3d9dev2 fixture f not caught"
  OUT=$O/e5kg $T/rw_d3d9dev2_check.sh $RD 20 300 4daed7:6a > $O/e5kg.txt 2>&1; grep -q 'compared; DIFF' $O/e5kg.txt && ok "d3d9dev2 show raster not-reset constant wrong: DIFF" || bad "d3d9dev2 fixture g not caught"
  OUT=$O/e5kh $T/rw_d3d9dev2_check.sh $RD 20 300 4d8ad7:21 > $O/e5kh.txt 2>&1; grep -q 'compared; DIFF' $O/e5kh.txt && ok "d3d9dev2 present parameters depth test wrong: DIFF" || bad "d3d9dev2 fixture h not caught"
  OUT=$O/e5ki $T/rw_d3d9dev2_check.sh $RD 20 300 4d95bb:3e > $O/e5ki.txt 2>&1; grep -q 'compared; DIFF' $O/e5ki.txt && ok "d3d9dev2 camera clear viewport depth wrong: DIFF" || bad "d3d9dev2 fixture i not caught"
  OUT=$O/e5kj $T/rw_d3d9dev2_check.sh $RD 20 300 4da38e:01 > $O/e5kj.txt 2>&1; grep -q 'compared; DIFF' $O/e5kj.txt && ok "d3d9dev2 begin update projection type wrong: DIFF" || bad "d3d9dev2 fixture j not caught"
  OUT=$O/e5kk $T/rw_d3d9dev2_check.sh $RD 20 300 4e032d:10 > $O/e5kk.txt 2>&1; grep -q 'compared; DIFF' $O/e5kk.txt && ok "d3d9dev2 palette lock flag wrong: DIFF" || bad "d3d9dev2 fixture k not caught"
  OUT=$O/e5kl $T/rw_d3d9dev2_check.sh $RD 20 300 4e0442:04 > $O/e5kl.txt 2>&1; grep -q 'compared; DIFF' $O/e5kl.txt && ok "d3d9dev2 render generic source type wrong: DIFF" || bad "d3d9dev2 fixture l not caught"
  OUT=$O/e5km $T/rw_d3d9dev2_check.sh $RD 20 300 4e3abb:05 > $O/e5km.txt 2>&1; grep -q 'compared; DIFF' $O/e5km.txt && ok "d3d9dev2 convert to non palettized format class wrong: DIFF" || bad "d3d9dev2 fixture m not caught"
fi
# d3d9rasq / d3d9sys / noded3d9 groups (window 5, lane K batch 6): the three checks must pass and DIFF on mutations of the original (all DIFF at 20 seeds x 300 steps; the full
# list is in agents/k6_w5/notes.md; the blocks below take one or two per function).
# d3d9rasq: (a) plugin attach 8888 format constant 0x500 -> 0x600 (4df81a), (b) render quad u divisor width -> height (4e0717), (c) render quad StretchRect filter 2 -> 3 (4e0a39),
# (d) copy quad destination type compare 5 -> 4 (4e0c4a), (e) copy quad result setge -> setg (4e0d60), (f) clear generic pool test inverted (4e0ed3), (g) clear generic palette
# search first minimum -> last minimum (4e112a), (h) clear generic z clear flags 2 -> 3 (4e1326), (i) raster open free list count 0x7f -> 0x7e (4dfbd8), (j) raster render last
# argument 1 -> 0 (4e03f9).
if ! grep -q mflrw_d3d9rasq_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9rasq exports, d3d9rasq fixtures skipped"; else
  OUT=$O/e6r $T/rw_d3d9rasq_check.sh $RD 20 300 > $O/e6r.txt 2>&1 && ok "d3d9rasq good: $(grep -c EQUAL $O/e6r.txt) run EQUAL" || bad "d3d9rasq good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e6r.txt | head -2)"
  OUT=$O/e6ra $T/rw_d3d9rasq_check.sh $RD 20 300 4df81a:06 > $O/e6ra.txt 2>&1; grep -q 'compared; DIFF' $O/e6ra.txt && ok "d3d9rasq plugin attach format constant wrong: DIFF" || bad "d3d9rasq fixture a not caught"
  OUT=$O/e6rb $T/rw_d3d9rasq_check.sh $RD 20 300 4e0717:10 > $O/e6rb.txt 2>&1; grep -q 'compared; DIFF' $O/e6rb.txt && ok "d3d9rasq render quad u divisor wrong: DIFF" || bad "d3d9rasq fixture b not caught"
  OUT=$O/e6rc $T/rw_d3d9rasq_check.sh $RD 20 300 4e0a39:03 > $O/e6rc.txt 2>&1; grep -q 'compared; DIFF' $O/e6rc.txt && ok "d3d9rasq render quad stretch filter wrong: DIFF" || bad "d3d9rasq fixture c not caught"
  OUT=$O/e6rd $T/rw_d3d9rasq_check.sh $RD 20 300 4e0c4a:04 > $O/e6rd.txt 2>&1; grep -q 'compared; DIFF' $O/e6rd.txt && ok "d3d9rasq copy quad type compare wrong: DIFF" || bad "d3d9rasq fixture d not caught"
  OUT=$O/e6re $T/rw_d3d9rasq_check.sh $RD 20 300 4e0d60:9f > $O/e6re.txt 2>&1; grep -q 'compared; DIFF' $O/e6re.txt && ok "d3d9rasq copy quad result test wrong: DIFF" || bad "d3d9rasq fixture e not caught"
  OUT=$O/e6rf $T/rw_d3d9rasq_check.sh $RD 20 300 4e0ed3:84 > $O/e6rf.txt 2>&1; grep -q 'compared; DIFF' $O/e6rf.txt && ok "d3d9rasq clear generic pool test inverted: DIFF" || bad "d3d9rasq fixture f not caught"
  OUT=$O/e6rg $T/rw_d3d9rasq_check.sh $RD 20 300 4e112a:77 > $O/e6rg.txt 2>&1; grep -q 'compared; DIFF' $O/e6rg.txt && ok "d3d9rasq clear generic palette tie rule wrong: DIFF" || bad "d3d9rasq fixture g not caught"
  OUT=$O/e6rh $T/rw_d3d9rasq_check.sh $RD 20 300 4e1326:03 > $O/e6rh.txt 2>&1; grep -q 'compared; DIFF' $O/e6rh.txt && ok "d3d9rasq clear generic z clear flags wrong: DIFF" || bad "d3d9rasq fixture h not caught"
  OUT=$O/e6ri $T/rw_d3d9rasq_check.sh $RD 20 300 4dfbd8:7e > $O/e6ri.txt 2>&1; grep -q 'compared; DIFF' $O/e6ri.txt && ok "d3d9rasq raster open list count wrong: DIFF" || bad "d3d9rasq fixture i not caught"
  OUT=$O/e6rj $T/rw_d3d9rasq_check.sh $RD 20 300 4e03f9:00 > $O/e6rj.txt 2>&1; grep -q 'compared; DIFF' $O/e6rj.txt && ok "d3d9rasq raster render argument wrong: DIFF" || bad "d3d9rasq fixture j not caught"
fi
# d3d9sys: (a) request range 0x16 -> 0x15 (4d7c0c), (b) use mode 32 bit depth constant 0x20 -> 0x21 (4d7ca1), (c) multisampling loop bound 0x10 -> 0x0f (4d7d12), (d) mode info
# 8888 format 0x500 -> 0x600 (4d7e5a), (e) register: device block copy count 14 -> 13 words (4d7f63), (f) Direct3DCreate9 SDK version 0x1f -> 0x20 (4d7f93), (g) open: caps result
# test inverted (4d8014), (h) start: behaviour flag 0x20 -> 0x40 (4d83f1), (i) finalize start: MXCSR bit 0x8000 -> 0x4000 (4d85e2), (j) max texture size compare jb -> ja (4d86a2),
# (k) focus: ShowWindow command 9 -> 5 (4d7f28; caught only since the fixture sets Present.Windowed last).
if ! grep -q mflrw_d3d9sys_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9sys exports, d3d9sys fixtures skipped"; else
  OUT=$O/e6s $T/rw_d3d9sys_check.sh $RD 20 300 > $O/e6s.txt 2>&1 && ok "d3d9sys good: $(grep -c EQUAL $O/e6s.txt) run EQUAL" || bad "d3d9sys good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e6s.txt | head -2)"
  OUT=$O/e6sa $T/rw_d3d9sys_check.sh $RD 20 300 4d7c0c:15 > $O/e6sa.txt 2>&1; grep -q 'compared; DIFF' $O/e6sa.txt && ok "d3d9sys request range wrong: DIFF" || bad "d3d9sys fixture a not caught"
  OUT=$O/e6sb $T/rw_d3d9sys_check.sh $RD 20 300 4d7ca1:21 > $O/e6sb.txt 2>&1; grep -q 'compared; DIFF' $O/e6sb.txt && ok "d3d9sys use mode depth constant wrong: DIFF" || bad "d3d9sys fixture b not caught"
  OUT=$O/e6sc $T/rw_d3d9sys_check.sh $RD 20 300 4d7d12:0f > $O/e6sc.txt 2>&1; grep -q 'compared; DIFF' $O/e6sc.txt && ok "d3d9sys multisampling loop bound wrong: DIFF" || bad "d3d9sys fixture c not caught"
  OUT=$O/e6sd $T/rw_d3d9sys_check.sh $RD 20 300 4d7e5a:06 > $O/e6sd.txt 2>&1; grep -q 'compared; DIFF' $O/e6sd.txt && ok "d3d9sys mode info format wrong: DIFF" || bad "d3d9sys fixture d not caught"
  OUT=$O/e6se $T/rw_d3d9sys_check.sh $RD 20 300 4d7f63:0d > $O/e6se.txt 2>&1; grep -q 'compared; DIFF' $O/e6se.txt && ok "d3d9sys register block copy count wrong: DIFF" || bad "d3d9sys fixture e not caught"
  OUT=$O/e6sf $T/rw_d3d9sys_check.sh $RD 20 300 4d7f93:20 > $O/e6sf.txt 2>&1; grep -q 'compared; DIFF' $O/e6sf.txt && ok "d3d9sys Direct3DCreate9 version wrong: DIFF" || bad "d3d9sys fixture f not caught"
  OUT=$O/e6sg $T/rw_d3d9sys_check.sh $RD 20 300 4d8014:7c > $O/e6sg.txt 2>&1; grep -q 'compared; DIFF' $O/e6sg.txt && ok "d3d9sys open caps test inverted: DIFF" || bad "d3d9sys fixture g not caught"
  OUT=$O/e6sh $T/rw_d3d9sys_check.sh $RD 20 300 4d83f1:40 > $O/e6sh.txt 2>&1; grep -q 'compared; DIFF' $O/e6sh.txt && ok "d3d9sys start behaviour flag wrong: DIFF" || bad "d3d9sys fixture h not caught"
  OUT=$O/e6si $T/rw_d3d9sys_check.sh $RD 20 300 4d85e2:40 > $O/e6si.txt 2>&1; grep -q 'compared; DIFF' $O/e6si.txt && ok "d3d9sys finalize start MXCSR bit wrong: DIFF" || bad "d3d9sys fixture i not caught"
  OUT=$O/e6sj $T/rw_d3d9sys_check.sh $RD 20 300 4d86a2:77 > $O/e6sj.txt 2>&1; grep -q 'compared; DIFF' $O/e6sj.txt && ok "d3d9sys max texture size compare wrong: DIFF" || bad "d3d9sys fixture j not caught"
  OUT=$O/e6sk $T/rw_d3d9sys_check.sh $RD 20 300 4d7f28:05 > $O/e6sk.txt 2>&1; grep -q 'compared; DIFF' $O/e6sk.txt && ok "d3d9sys focus ShowWindow command wrong: DIFF" || bad "d3d9sys fixture k not caught"
fi
# noded3d9: (a) world sector init instance callback address (500519), (b) atomic init lighting callback address (4ff967), (c) sort compare stream test inverted (500326), (d) world
# sector lighting world flag 0x20 -> 0x10 (50054b), (e) reinstance lock bits 6 -> 4 (500363), (f) world sector node flag 0x1000000 -> 0x2000000 (50040c), (g) world sector node
# render state 0x8f -> 0x8e (5004de), (h) atomic node morph target compare 1 -> 2 (4ff6f7), (i) atomic node row length upper bound 1.1 -> 1.006 (4ff884), (j) atomic lighting
# directional subtype 1 -> 2 (4ff537), (k) atomic lighting sphere compare: equal now enables (4ff64c), (l) atomic node row length lower bound 0.9 -> 0.94 (4ff88c), (m) atomic node
# second row reads the first row's x (4ff8b9).  (i) (k) (l) (m) are caught only since the fixture draws unit length matrix rows and the exact 3-4-5 sphere case.
if ! grep -q mflrw_noded3d9_bind $RD 2>/dev/null; then echo "negtest note: $RD has no noded3d9 exports, noded3d9 fixtures skipped"; else
  OUT=$O/e6n $T/rw_noded3d9_check.sh $RD 20 300 > $O/e6n.txt 2>&1 && ok "noded3d9 good: $(grep -c EQUAL $O/e6n.txt) run EQUAL" || bad "noded3d9 good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e6n.txt | head -2)"
  OUT=$O/e6na $T/rw_noded3d9_check.sh $RD 20 300 500519:b1 > $O/e6na.txt 2>&1; grep -q 'compared; DIFF' $O/e6na.txt && ok "noded3d9 world sector init callback wrong: DIFF" || bad "noded3d9 fixture a not caught"
  OUT=$O/e6nb $T/rw_noded3d9_check.sh $RD 20 300 4ff967:c1 > $O/e6nb.txt 2>&1; grep -q 'compared; DIFF' $O/e6nb.txt && ok "noded3d9 atomic init callback wrong: DIFF" || bad "noded3d9 fixture b not caught"
  OUT=$O/e6nc $T/rw_noded3d9_check.sh $RD 20 300 500326:74 > $O/e6nc.txt 2>&1; grep -q 'compared; DIFF' $O/e6nc.txt && ok "noded3d9 sort compare inverted: DIFF" || bad "noded3d9 fixture c not caught"
  OUT=$O/e6nd $T/rw_noded3d9_check.sh $RD 20 300 50054b:10 > $O/e6nd.txt 2>&1; grep -q 'compared; DIFF' $O/e6nd.txt && ok "noded3d9 world sector lighting flag wrong: DIFF" || bad "noded3d9 fixture d not caught"
  OUT=$O/e6ne $T/rw_noded3d9_check.sh $RD 20 300 500363:04 > $O/e6ne.txt 2>&1; grep -q 'compared; DIFF' $O/e6ne.txt && ok "noded3d9 reinstance lock bits wrong: DIFF" || bad "noded3d9 fixture e not caught"
  OUT=$O/e6nf $T/rw_noded3d9_check.sh $RD 20 300 50040c:02 > $O/e6nf.txt 2>&1; grep -q 'compared; DIFF' $O/e6nf.txt && ok "noded3d9 world sector node flag wrong: DIFF" || bad "noded3d9 fixture f not caught"
  OUT=$O/e6ng $T/rw_noded3d9_check.sh $RD 20 300 5004de:8e > $O/e6ng.txt 2>&1; grep -q 'compared; DIFF' $O/e6ng.txt && ok "noded3d9 world sector node render state wrong: DIFF" || bad "noded3d9 fixture g not caught"
  OUT=$O/e6nh $T/rw_noded3d9_check.sh $RD 20 300 4ff6f7:02 > $O/e6nh.txt 2>&1; grep -q 'compared; DIFF' $O/e6nh.txt && ok "noded3d9 atomic node morph compare wrong: DIFF" || bad "noded3d9 fixture h not caught"
  OUT=$O/e6ni $T/rw_noded3d9_check.sh $RD 20 300 4ff884:80 > $O/e6ni.txt 2>&1; grep -q 'compared; DIFF' $O/e6ni.txt && ok "noded3d9 atomic node row length upper bound wrong: DIFF" || bad "noded3d9 fixture i not caught"
  OUT=$O/e6nj $T/rw_noded3d9_check.sh $RD 20 300 4ff537:02 > $O/e6nj.txt 2>&1; grep -q 'compared; DIFF' $O/e6nj.txt && ok "noded3d9 atomic lighting light subtype wrong: DIFF" || bad "noded3d9 fixture j not caught"
  OUT=$O/e6nk $T/rw_noded3d9_check.sh $RD 20 300 4ff64c:01 > $O/e6nk.txt 2>&1; grep -q 'compared; DIFF' $O/e6nk.txt && ok "noded3d9 atomic lighting sphere compare wrong: DIFF" || bad "noded3d9 fixture k not caught"
  OUT=$O/e6nl $T/rw_noded3d9_check.sh $RD 20 300 4ff88c:70 > $O/e6nl.txt 2>&1; grep -q 'compared; DIFF' $O/e6nl.txt && ok "noded3d9 atomic node row length lower bound wrong: DIFF" || bad "noded3d9 fixture l not caught"
  OUT=$O/e6nm $T/rw_noded3d9_check.sh $RD 20 300 4ff8b9:00 > $O/e6nm.txt 2>&1; grep -q 'compared; DIFF' $O/e6nm.txt && ok "noded3d9 atomic node second row operand wrong: DIFF" || bad "noded3d9 fixture m not caught"
fi
# noded3d9i group (window 6, lane K1): the check must pass and DIFF on mutations of the original (all DIFF at 20 seeds x 300 steps; full lists agents/k1_w6/
# mut_ws/summary.txt 145 DIFF, mut_at/summary.txt 323 DIFF; the lines below take the constants a port most easily gets wrong).
# World sector callback: (a) normals world flag 0x10 -> 0x11 (5005fa), (b) compressed normal declaration type 0xe -> 0xf (500642), (c) texture set 0 flag mask 0x84 -> 0x85
# (5006af), (d) tangent usage bit 0x10000 -> 0x10001 (500724), (e) vertex buffer lock flags 0 -> 1 (5007f8), (f) colour element size 4 -> 5 (5008d8).
# Atomic callback: (g) pipeline plugin id 0x12e -> 0x12f (4ffa66), (h) morphing geometry usage |= 6 -> 4 (4ffa76), (i) stream offset caps bit test 1 -> 0 (4ffd13),
# (j) dynamic buffer lock flags 0x2800 -> 0x2801 (4ffe97), (k) interpolator scale == 0 test mask 0x44 -> 0x45 (4fff0f; x87 compare), (l) position morph writer call ->
# the plain position writer (4fffd0 + 4fffd1).
if ! grep -q mflrw_noded3d9i_bind $RD 2>/dev/null; then echo "negtest note: $RD has no noded3d9i exports, noded3d9i fixtures skipped"; else
  OUT=$O/e7i $T/rw_noded3d9i_check.sh $RD 20 300 > $O/e7i.txt 2>&1 && ok "noded3d9i good: $(grep -c EQUAL $O/e7i.txt) run EQUAL" || bad "noded3d9i good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e7i.txt | head -2)"
  OUT=$O/e7ia $T/rw_noded3d9i_check.sh $RD 20 300 5005fa:11 > $O/e7ia.txt 2>&1; grep -q 'compared; DIFF' $O/e7ia.txt && ok "noded3d9i world sector normals flag wrong: DIFF" || bad "noded3d9i fixture a not caught"
  OUT=$O/e7ib $T/rw_noded3d9i_check.sh $RD 20 300 500642:0f > $O/e7ib.txt 2>&1; grep -q 'compared; DIFF' $O/e7ib.txt && ok "noded3d9i world sector compressed normal type wrong: DIFF" || bad "noded3d9i fixture b not caught"
  OUT=$O/e7ic $T/rw_noded3d9i_check.sh $RD 20 300 5006af:85 > $O/e7ic.txt 2>&1; grep -q 'compared; DIFF' $O/e7ic.txt && ok "noded3d9i world sector texture flag mask wrong: DIFF" || bad "noded3d9i fixture c not caught"
  OUT=$O/e7id $T/rw_noded3d9i_check.sh $RD 20 300 500724:01 > $O/e7id.txt 2>&1; grep -q 'compared; DIFF' $O/e7id.txt && ok "noded3d9i world sector tangent usage bit wrong: DIFF" || bad "noded3d9i fixture d not caught"
  OUT=$O/e7ie $T/rw_noded3d9i_check.sh $RD 20 300 5007f8:01 > $O/e7ie.txt 2>&1; grep -q 'compared; DIFF' $O/e7ie.txt && ok "noded3d9i world sector lock flags wrong: DIFF" || bad "noded3d9i fixture e not caught"
  OUT=$O/e7if $T/rw_noded3d9i_check.sh $RD 20 300 5008d8:05 > $O/e7if.txt 2>&1; grep -q 'compared; DIFF' $O/e7if.txt && ok "noded3d9i world sector colour size wrong: DIFF" || bad "noded3d9i fixture f not caught"
  OUT=$O/e7ig $T/rw_noded3d9i_check.sh $RD 20 300 4ffa66:2f > $O/e7ig.txt 2>&1; grep -q 'compared; DIFF' $O/e7ig.txt && ok "noded3d9i atomic pipeline plugin id wrong: DIFF" || bad "noded3d9i fixture g not caught"
  OUT=$O/e7ih $T/rw_noded3d9i_check.sh $RD 20 300 4ffa76:04 > $O/e7ih.txt 2>&1; grep -q 'compared; DIFF' $O/e7ih.txt && ok "noded3d9i atomic morph usage bits wrong: DIFF" || bad "noded3d9i fixture h not caught"
  OUT=$O/e7ii $T/rw_noded3d9i_check.sh $RD 20 300 4ffd13:00 > $O/e7ii.txt 2>&1; grep -q 'compared; DIFF' $O/e7ii.txt && ok "noded3d9i atomic stream offset caps test wrong: DIFF" || bad "noded3d9i fixture i not caught"
  OUT=$O/e7ij $T/rw_noded3d9i_check.sh $RD 20 300 4ffe97:01 > $O/e7ij.txt 2>&1; grep -q 'compared; DIFF' $O/e7ij.txt && ok "noded3d9i atomic dynamic lock flags wrong: DIFF" || bad "noded3d9i fixture j not caught"
  OUT=$O/e7ik $T/rw_noded3d9i_check.sh $RD 20 300 4fff0f:45 > $O/e7ik.txt 2>&1; grep -q 'compared; DIFF' $O/e7ik.txt && ok "noded3d9i atomic morph scale test wrong: DIFF" || bad "noded3d9i fixture k not caught"
  OUT=$O/e7il $T/rw_noded3d9i_check.sh $RD 20 300 4fffd0:1c 4fffd1:64 > $O/e7il.txt 2>&1; grep -q 'compared; DIFF' $O/e7il.txt && ok "noded3d9i atomic morph writer call wrong: DIFF" || bad "noded3d9i fixture l not caught"
fi
# d3d9submit group (window 6, lane K1): the check must pass and DIFF on mutations of the original (all DIFF at 20 seeds x 300 steps; full lists agents/k1_w6/
# mut_imm / mut_close / mut_open / mut_submit summary.txt).
# (a) ImmInstanceNode return 1 -> 0 (4f1397), (b) RenderClose old declaration address 0x7d5240 -> 0x7d5241 (4f0a19), (c) RenderOpen index buffer size 0x1fffe -> 0x1ffff
# (4f0a71), (d) RenderOpen usage 0x208 -> 0x209 (4f0a6c), (e) RenderOpen pool shift 0xf -> 0xe (4f0a55), (f) SubmitNode render state 0x88 -> 0x89 (4f0d17),
# (g) SubmitNode 8 index limit -> 9 (4f0e84), (h) SubmitNode index count limit 0xffff -> 0xfffe (4f0f9a), (i) index buffer lock flags 0x2800 -> 0x2801 (4f1069),
# (j) 0x1800 -> 0x1801 (4f108a), (k) DrawIndexedPrimitiveUP index format 0x65 -> 0x64 (4f11ca), (l) DrawPrimitiveUP stride 0x24 -> 0x25 (4f134a).
if ! grep -q mflrw_d3d9submit_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9submit exports, d3d9submit fixtures skipped"; else
  OUT=$O/e7s $T/rw_d3d9submit_check.sh $RD 20 300 > $O/e7s.txt 2>&1 && ok "d3d9submit good: $(grep -c EQUAL $O/e7s.txt) run EQUAL" || bad "d3d9submit good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH' $O/e7s.txt | head -2)"
  OUT=$O/e7sa $T/rw_d3d9submit_check.sh $RD 20 300 4f1397:00 > $O/e7sa.txt 2>&1; grep -q 'compared; DIFF' $O/e7sa.txt && ok "d3d9submit instance node result wrong: DIFF" || bad "d3d9submit fixture a not caught"
  OUT=$O/e7sb $T/rw_d3d9submit_check.sh $RD 20 300 4f0a19:41 > $O/e7sb.txt 2>&1; grep -q 'compared; DIFF' $O/e7sb.txt && ok "d3d9submit close declaration address wrong: DIFF" || bad "d3d9submit fixture b not caught"
  OUT=$O/e7sc $T/rw_d3d9submit_check.sh $RD 20 300 4f0a71:ff > $O/e7sc.txt 2>&1; grep -q 'compared; DIFF' $O/e7sc.txt && ok "d3d9submit open index buffer size wrong: DIFF" || bad "d3d9submit fixture c not caught"
  OUT=$O/e7sd $T/rw_d3d9submit_check.sh $RD 20 300 4f0a6c:09 > $O/e7sd.txt 2>&1; grep -q 'compared; DIFF' $O/e7sd.txt && ok "d3d9submit open index buffer usage wrong: DIFF" || bad "d3d9submit fixture d not caught"
  OUT=$O/e7se $T/rw_d3d9submit_check.sh $RD 20 300 4f0a55:0e > $O/e7se.txt 2>&1; grep -q 'compared; DIFF' $O/e7se.txt && ok "d3d9submit open pool test wrong: DIFF" || bad "d3d9submit fixture e not caught"
  OUT=$O/e7sf $T/rw_d3d9submit_check.sh $RD 20 300 4f0d17:89 > $O/e7sf.txt 2>&1; grep -q 'compared; DIFF' $O/e7sf.txt && ok "d3d9submit render state id wrong: DIFF" || bad "d3d9submit fixture f not caught"
  OUT=$O/e7sg $T/rw_d3d9submit_check.sh $RD 20 300 4f0e84:09 > $O/e7sg.txt 2>&1; grep -q 'compared; DIFF' $O/e7sg.txt && ok "d3d9submit 8 index limit wrong: DIFF" || bad "d3d9submit fixture g not caught"
  OUT=$O/e7sh $T/rw_d3d9submit_check.sh $RD 20 300 4f0f9a:fe > $O/e7sh.txt 2>&1; grep -q 'compared; DIFF' $O/e7sh.txt && ok "d3d9submit index count limit wrong: DIFF" || bad "d3d9submit fixture h not caught"
  OUT=$O/e7si $T/rw_d3d9submit_check.sh $RD 20 300 4f1069:01 > $O/e7si.txt 2>&1; grep -q 'compared; DIFF' $O/e7si.txt && ok "d3d9submit discard lock flags wrong: DIFF" || bad "d3d9submit fixture i not caught"
  OUT=$O/e7sj $T/rw_d3d9submit_check.sh $RD 20 300 4f108a:01 > $O/e7sj.txt 2>&1; grep -q 'compared; DIFF' $O/e7sj.txt && ok "d3d9submit append lock flags wrong: DIFF" || bad "d3d9submit fixture j not caught"
  OUT=$O/e7sk $T/rw_d3d9submit_check.sh $RD 20 300 4f11ca:64 > $O/e7sk.txt 2>&1; grep -q 'compared; DIFF' $O/e7sk.txt && ok "d3d9submit UP index format wrong: DIFF" || bad "d3d9submit fixture k not caught"
  OUT=$O/e7sl $T/rw_d3d9submit_check.sh $RD 20 300 4f134a:25 > $O/e7sl.txt 2>&1; grep -q 'compared; DIFF' $O/e7sl.txt && ok "d3d9submit UP stride wrong: DIFF" || bad "d3d9submit fixture l not caught"
fi
# d3d9patch group (lane K2 window 6): the check must pass and DIFF on mutations of the original (all DIFF at 20 seeds x 300 steps; verified with agents/k2_w6/mut_a.sh /
# mut_b.sh, full lists in agents/k2_w6/notes.md; the block takes five per function).  Insert after the noded3d9 block of tools/delink/negtest.sh; $RD must be a DLL built
# with d3d9patch.cpp (the bind export).  va:byte list (each must give DIFF):
# _rpD3D9PatchInstanceAtomic: (a) tri / quad compare: equal test mask 0x44 -> 0x40 (55a8a9:40), (b) mesh split limit 0xffff -> 0x7fff (55a8c5:7f), (c) strip join copies
# index -2 instead of -1 (55a915:fc), (d) quad colour byte 1 <-> 2 (55ae65:02), (e) quad texcoord corner index 12 -> 13 (55af2a:1a).
# rwD3D9PatchAtomicAllInOneNodeNode: (f) meshes per 0xffff indices: divisor shift 15 -> 14 (55b1a9:0e), (g) colour element usage 10 -> 11 (55b283:0b), (h) 32 bit index
# limit jbe -> jb (55b495:72), (i) primitive type 5 -> 4 (55b4e9:04), (j) row length upper bound 1.1 -> 1.006 (55b8a2:80).
if ! grep -q mflrw_d3d9patch_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9patch exports, d3d9patch fixtures skipped"; else
  OUT=$O/e7p $T/rw_d3d9patch_check.sh $RD 20 300 > $O/e7p.txt 2>&1 && ok "d3d9patch good: $(grep -c EQUAL $O/e7p.txt) run EQUAL" || bad "d3d9patch good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH|ABORT' $O/e7p.txt | head -2)"
  OUT=$O/e7pa $T/rw_d3d9patch_check.sh $RD 20 300 55a8a9:40 > $O/e7pa.txt 2>&1; grep -q 'compared; DIFF' $O/e7pa.txt && ok "d3d9patch instance tri / quad test wrong: DIFF" || bad "d3d9patch fixture a not caught"
  OUT=$O/e7pb $T/rw_d3d9patch_check.sh $RD 20 300 55a8c5:7f > $O/e7pb.txt 2>&1; grep -q 'compared; DIFF' $O/e7pb.txt && ok "d3d9patch instance mesh split limit wrong: DIFF" || bad "d3d9patch fixture b not caught"
  OUT=$O/e7pc $T/rw_d3d9patch_check.sh $RD 20 300 55a915:fc > $O/e7pc.txt 2>&1; grep -q 'compared; DIFF' $O/e7pc.txt && ok "d3d9patch instance strip join index wrong: DIFF" || bad "d3d9patch fixture c not caught"
  OUT=$O/e7pd $T/rw_d3d9patch_check.sh $RD 20 300 55ae65:02 > $O/e7pd.txt 2>&1; grep -q 'compared; DIFF' $O/e7pd.txt && ok "d3d9patch instance quad colour byte wrong: DIFF" || bad "d3d9patch fixture d not caught"
  OUT=$O/e7pe $T/rw_d3d9patch_check.sh $RD 20 300 55af2a:1a > $O/e7pe.txt 2>&1; grep -q 'compared; DIFF' $O/e7pe.txt && ok "d3d9patch instance quad texcoord corner wrong: DIFF" || bad "d3d9patch fixture e not caught"
  OUT=$O/e7pf $T/rw_d3d9patch_check.sh $RD 20 300 55b1a9:0e > $O/e7pf.txt 2>&1; grep -q 'compared; DIFF' $O/e7pf.txt && ok "d3d9patch node mesh count divisor wrong: DIFF" || bad "d3d9patch fixture f not caught"
  OUT=$O/e7pg $T/rw_d3d9patch_check.sh $RD 20 300 55b283:0b > $O/e7pg.txt 2>&1; grep -q 'compared; DIFF' $O/e7pg.txt && ok "d3d9patch node colour element usage wrong: DIFF" || bad "d3d9patch fixture g not caught"
  OUT=$O/e7ph $T/rw_d3d9patch_check.sh $RD 20 300 55b495:72 > $O/e7ph.txt 2>&1; grep -q 'compared; DIFF' $O/e7ph.txt && ok "d3d9patch node 32 bit index limit wrong: DIFF" || bad "d3d9patch fixture h not caught"
  OUT=$O/e7pi $T/rw_d3d9patch_check.sh $RD 20 300 55b4e9:04 > $O/e7pi.txt 2>&1; grep -q 'compared; DIFF' $O/e7pi.txt && ok "d3d9patch node primitive type wrong: DIFF" || bad "d3d9patch fixture i not caught"
  OUT=$O/e7pj $T/rw_d3d9patch_check.sh $RD 20 300 55b8a2:80 > $O/e7pj.txt 2>&1; grep -q 'compared; DIFF' $O/e7pj.txt && ok "d3d9patch node row length bound wrong: DIFF" || bad "d3d9patch fixture j not caught"
fi
# d3d9ptank group (lane K2 window 6): the check must pass and DIFF on mutations of the original (20 seeds x 300 steps; verified with agents/k2_w6/mut_c.sh, full
# list agents/k2_w6/mut_d3d9ptank.txt, results agents/k2_w6/mut_c/result.txt).  $RD must be a DLL built with d3d9ptank.cpp (the bind export).  va:byte list:
# (a) InsEnding unlocks block word +4 instead of the vertex buffer (545f51:c4), (b) InsSetup base vertex size 12 -> 16 (545fc8:10), (c) InsSetup rotated up axis uses
# the cosine twice (5460f0:3c), (d) InsSetup locks into another block word (54615a:c8), (e) Create: max point size == 1.0 no longer drops point sprites (5467ca:01),
# (f) Create index buffer regrow test jge -> jg (546887:8f), (g) Create last index of a quad +3 -> +4 (54690a:04), (h) Create stores another sprite position function
# (54697f:c1), (i) RenderCallBack texture factor render state 0x3c -> 0x3d (5463ac:3d), (j) RenderCallBack SetFVF goes to another device slot (5464f0:70),
# (k) RenderCallBack point run limit 0xffff -> 0xfeff (546659:fe), (l) RenderCallBack indexed run limit 0xfffc -> 0xfff8 (5466e7:f8).
if ! grep -q mflrw_d3d9ptank_bind $RD 2>/dev/null; then echo "negtest note: $RD has no d3d9ptank exports, d3d9ptank fixtures skipped"; else
  OUT=$O/e7t $T/rw_d3d9ptank_check.sh $RD 20 300 > $O/e7t.txt 2>&1 && ok "d3d9ptank good: $(grep -c EQUAL $O/e7t.txt) run EQUAL" || bad "d3d9ptank good rejected: $(grep -E 'DIFF|MISSING|TRAP|TIMEOUT|CRASH|ABORT' $O/e7t.txt | head -2)"
  OUT=$O/e7ta $T/rw_d3d9ptank_check.sh $RD 20 300 545f51:c4 > $O/e7ta.txt 2>&1; grep -q 'compared; DIFF' $O/e7ta.txt && ok "d3d9ptank ending buffer word wrong: DIFF" || bad "d3d9ptank fixture a not caught"
  OUT=$O/e7tb $T/rw_d3d9ptank_check.sh $RD 20 300 545fc8:10 > $O/e7tb.txt 2>&1; grep -q 'compared; DIFF' $O/e7tb.txt && ok "d3d9ptank setup vertex size wrong: DIFF" || bad "d3d9ptank fixture b not caught"
  OUT=$O/e7tc $T/rw_d3d9ptank_check.sh $RD 20 300 5460f0:3c > $O/e7tc.txt 2>&1; grep -q 'compared; DIFF' $O/e7tc.txt && ok "d3d9ptank setup rotation operand wrong: DIFF" || bad "d3d9ptank fixture c not caught"
  OUT=$O/e7td $T/rw_d3d9ptank_check.sh $RD 20 300 54615a:c8 > $O/e7td.txt 2>&1; grep -q 'compared; DIFF' $O/e7td.txt && ok "d3d9ptank setup lock target wrong: DIFF" || bad "d3d9ptank fixture d not caught"
  OUT=$O/e7te $T/rw_d3d9ptank_check.sh $RD 20 300 5467ca:01 > $O/e7te.txt 2>&1; grep -q 'compared; DIFF' $O/e7te.txt && ok "d3d9ptank create point size test wrong: DIFF" || bad "d3d9ptank fixture e not caught"
  OUT=$O/e7tf $T/rw_d3d9ptank_check.sh $RD 20 300 546887:8f > $O/e7tf.txt 2>&1; grep -q 'compared; DIFF' $O/e7tf.txt && ok "d3d9ptank create index buffer regrow test wrong: DIFF" || bad "d3d9ptank fixture f not caught"
  OUT=$O/e7tg $T/rw_d3d9ptank_check.sh $RD 20 300 54690a:04 > $O/e7tg.txt 2>&1; grep -q 'compared; DIFF' $O/e7tg.txt && ok "d3d9ptank create quad index wrong: DIFF" || bad "d3d9ptank fixture g not caught"
  OUT=$O/e7th $T/rw_d3d9ptank_check.sh $RD 20 300 54697f:c1 > $O/e7th.txt 2>&1; grep -q 'compared; DIFF' $O/e7th.txt && ok "d3d9ptank create sprite function wrong: DIFF" || bad "d3d9ptank fixture h not caught"
  OUT=$O/e7ti $T/rw_d3d9ptank_check.sh $RD 20 300 5463ac:3d > $O/e7ti.txt 2>&1; grep -q 'compared; DIFF' $O/e7ti.txt && ok "d3d9ptank render texture factor state wrong: DIFF" || bad "d3d9ptank fixture i not caught"
  OUT=$O/e7tj $T/rw_d3d9ptank_check.sh $RD 20 300 5464f0:70 > $O/e7tj.txt 2>&1; grep -q 'compared; DIFF' $O/e7tj.txt && ok "d3d9ptank render SetFVF slot wrong: DIFF" || bad "d3d9ptank fixture j not caught"
  OUT=$O/e7tk $T/rw_d3d9ptank_check.sh $RD 20 300 546659:fe > $O/e7tk.txt 2>&1; grep -q 'compared; DIFF' $O/e7tk.txt && ok "d3d9ptank render point run limit wrong: DIFF" || bad "d3d9ptank fixture k not caught"
  OUT=$O/e7tl $T/rw_d3d9ptank_check.sh $RD 20 300 5466e7:f8 > $O/e7tl.txt 2>&1; grep -q 'compared; DIFF' $O/e7tl.txt && ok "d3d9ptank render indexed run limit wrong: DIFF" || bad "d3d9ptank fixture l not caught"
fi
fi      # NEGTEST_SKIP_DLL
rm -r $O
exit $rc
