// cluster-entries: course_city_cb1_sub_455e60
// expect: class 20
// path: recomp/clusters/cl_negtest20.c
/* T-fix14: the original 455e60 takes its position in ESI; a plain cdecl definition reads [esp+4] (saved ESI 0x24, course 14 crash) */
extern void neg_use(int *pos);
void course_city_cb1_sub_455e60(int *pos)
{
  neg_use(pos);
}
