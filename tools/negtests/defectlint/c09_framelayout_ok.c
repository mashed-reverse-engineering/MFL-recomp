// expect: clean class 9
extern void neg_reads_two(void *p);
void negtest_fn(void)
{
  struct {
    undefined4 flag;
    undefined4 value;
  } frame;

  frame.flag = 0;
  frame.value = 1;
  neg_reads_two(&frame);
}
