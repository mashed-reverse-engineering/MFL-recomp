#include "mfl_tier2.h"
/*004b2a60*/ extern int sprintf();
/*005de9b4*/ extern undefined g_frontend_5de9b4;
/*005de9b8*/ extern undefined g_ui_5de9b8;
/*005de9c0*/ extern undefined g_ui_5de9c0;


/* ui_format_number_string */

/* "formats a number below 10 with one format string, else with another" */

void ui_format_number_string(int first_number,undefined4 second_number_bits,undefined4 first_out,undefined4 second_out)

{
  int second_number;
  
  if (first_number < 10) {
    sprintf(first_out,&g_ui_5de9b8,&g_ui_5de9c0,first_number);
  }
  else {
    sprintf(first_out,&g_frontend_5de9b4,first_number);
  }
  second_number = (int)*(float *)&second_number_bits;
  if (second_number < 10) {
    sprintf(second_out,&g_ui_5de9b8,&g_ui_5de9c0,second_number);
    return;
  }
  sprintf(second_out,&g_frontend_5de9b4,second_number);
  return;
}
