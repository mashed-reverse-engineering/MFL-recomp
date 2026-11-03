// cluster-entries: course_city_cb1_sub_455e60
// expect: clean class 20
// path: recomp/clusters/cl_negtest20.c
extern void neg_use(int *pos);
void course_city_cb1_sub_455e60_n(int *pos)
{
  neg_use(pos);
}
__declspec(naked) void course_city_cb1_sub_455e60(void)
{
  __asm {
    push esi
    call course_city_cb1_sub_455e60_n
    add esp, 4
    ret
  }
}
