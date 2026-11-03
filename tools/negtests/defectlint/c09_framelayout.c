// expect: class 9
/* &local_N passed while its neighbour slot is used: one frame object in the original (clump_has_uv_anim) */
extern void neg_reads_two(void *p);
void negtest_fn(void)
{
  undefined4 local_10;
  undefined4 local_c;

  local_10 = 0;
  local_c = 1;
  neg_reads_two(&local_10);
}
