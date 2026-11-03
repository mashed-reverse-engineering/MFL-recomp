// expect: class 6
/* COM method called as cdecl; the original callee pops its argument (gap_4af780, an_hk_1357) */
extern int *g_neg_com;
void gap_4af780(void)
{
  int *piVar1;
  piVar1 = g_neg_com;
  (**(code **)(*piVar1 + 4))(piVar1);
}
