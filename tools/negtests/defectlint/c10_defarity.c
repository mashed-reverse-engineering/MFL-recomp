// expect: class 10
/* call with fewer arguments than the callee's original body reads (time_trial_record_step -> btb_panel_set_visible_all, an_ok_008) */
/*0041da30*/ extern void btb_panel_set_visible_all(void);
void negtest_fn(void)
{
  btb_panel_set_visible_all();
}
