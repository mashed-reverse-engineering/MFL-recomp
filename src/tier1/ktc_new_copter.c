#include "mfl_tier2.h"
/*004b2a60*/ extern int sprintf();
/*00651250*/ extern undefined g_scene_651250;
/* layout of *rec in ktc_new_copter, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct ktc_new_copter_rec_layout {
    char pad_0x0[20];
    undefined4 f_0x14;
    char pad_0x18[48];
    undefined4 f_0x48;
    char pad_0x4c[16];
    undefined4 f_0x5c;
    char pad_0x60[8];
    undefined4 f_0x68;
    undefined4 f_0x6c;
    undefined4 f_0x70;
    undefined4 f_0x74;
    undefined4 f_0x78;
    char pad_0x7c[24];
    undefined4 f_0x94;
    undefined4 f_0x98;
    undefined4 f_0x9c;
    undefined4 f_0xa0;
    undefined4 f_0xa4;
    undefined4 f_0xa8;
    undefined4 f_0xac;
    undefined4 f_0xb0;
    undefined4 f_0xb4;
    char pad_0xb8[20];
    undefined4 f_0xcc;
    undefined4 f_0xd0;
    undefined4 f_0xd4;
    undefined4 f_0xd8;
    undefined4 f_0xdc;
    undefined4 f_0xe0;
    undefined4 f_0xe4;
    undefined4 f_0xe8;
} ktc_new_copter_rec_layout;

/*00651aa0*/ extern undefined4 g_fx_object_count;
#define FX_RECORD_DWORDS (0x3b)          /* dwords per fx object record (stride of the g_fx_* arrays) */
#define FX_RECORD_SIZE (0xec)            /* bytes per fx object record: FX_RECORD_DWORDS * 4 */
#define FX_TIMER_START (0x42c80000)      /* float 100.0: initial +0x5C timer (inferred) */
/* ktc_new_copter */
/* "(name, args, count): helper of the Lua KTC_NewCopter" */
/* Appends one fx object record to the table at g_scene_651250 (records are FX_RECORD_SIZE bytes)
   and returns its index. The record gets fixed defaults; the caller's values are:
   name: printed into the record's name buffer at +0x00 with sprintf, so it is used as the
         format string (inferred from the single-argument call);
   args: count dwords copied to the record starting at +0xB4;
   count: number of dwords taken from args (0 = none), stored at +0xB0.
   Returns the index of the new record (the old g_fx_object_count), then increments the count. */
int ktc_new_copter(undefined4 name,char *args,int count)
{
  undefined4 *dst;
  int object_index;
  ktc_new_copter_rec_layout *rec;
  int i;

  object_index = g_fx_object_count;
  rec = (ktc_new_copter_rec_layout *)((char *)&g_scene_651250 + g_fx_object_count * FX_RECORD_SIZE);
  i = 0;
  /* name buffer of the new record */
  sprintf(rec,name);
  /* clear the leading fields */
  rec->f_0xd0 = 0;
  rec->f_0x5c = FX_TIMER_START;
  rec->f_0x14 = 0;
  /* copy count dwords from args into the record's data area, one per iteration */
  if (0 < count) {
    dst = &rec->f_0xb4;
    do {
      *dst = *(undefined4 *)(args + i * 4);
      i = i + 1;
      dst = dst + 1;
    } while (i < count);
  }
  rec->f_0xb0 = count;
  rec->f_0xcc = 0;
  rec->f_0xe4 = 0;
  rec->f_0xe0 = 0;
  rec->f_0xdc = 0;
  rec->f_0xd4 = 0;
  rec->f_0x48 = 0;
  rec->f_0xa4 = 0;
  rec->f_0xa8 = 0;
  rec->f_0xac = 0;
  rec->f_0x6c = 0;
  rec->f_0x68 = 0;
  /* float defaults as bit patterns (field meanings not known, inferred from the values) */
  rec->f_0xd8 = 0x3f800000;
  rec->f_0xe8 = 0x3f733333;
  rec->f_0x70 = 0x3f19999a;
  rec->f_0x74 = 0x3fb33333;
  rec->f_0x78 = 0x3d4ccccd;
  rec->f_0x94 = 0x3db851ec;
  rec->f_0x98 = 0x41200000;
  rec->f_0x9c = 0x3dcccccd;
  rec->f_0xa0 = 0x3e4ccccd;
  object_index = g_fx_object_count;
  g_fx_object_count = g_fx_object_count + 1;
  return object_index;
}
