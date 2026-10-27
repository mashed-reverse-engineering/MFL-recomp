#include "mfl_tier2.h"
/*004a9380*/ extern int app_get_app_7e7704();


/* video_get_device_pointer */

/* Retrieves the device pointer from the application and stores it in the output location. */

void video_get_device_pointer(int *out_device_ptr)
{
  int device_ptr;
  
  device_ptr = app_get_app_7e7704();
  *out_device_ptr = device_ptr;
  return;
}
