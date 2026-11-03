// expect: clean class 6
extern int *g_neg_com;
void gap_4af780(void)
{
  int *piVar1;
  piVar1 = g_neg_com;
  (*(int (__stdcall **)(int *))(*piVar1 + 4))(piVar1);
}
