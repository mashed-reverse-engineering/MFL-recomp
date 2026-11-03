// expect: class 7
/* a register-input callee (ecx, regsigs.csv) called as cdecl (rwp_pipeline_00571200, an_rwp_00571200) */
/*005782f0*/ extern int rwp_batch_005782f0(void);
int negtest_fn(void)
{
  return rwp_batch_005782f0();
}
