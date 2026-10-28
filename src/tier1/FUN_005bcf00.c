#include "mfl_tier2.h"
/*005be600*/ extern int __cdecl guid_compare(int a1, int a2);
/*
 * FUN_005bcf00: returns 1 if `entry` matches `rule`, 0 otherwise.
 *   rule:  filter record. rule[0] is a key passed to guid_compare, rule[2]..rule[3] are
 *          inclusive bounds for entry[0], byte +0x5 and byte +0x4 are tag bytes (inferred).
 *   entry: record being tested. entry[0] is the value checked against the bounds,
 *          entry[1] is passed to guid_compare, byte +0xd and entry[3] low byte are tag bytes (inferred),
 *          bit 0 of entry[6] is a flag (inferred).
 */
undefined4 __cdecl FUN_005bcf00(undefined4 *rule,uint *entry)
{
  int callee_result;

  /* tag bytes must match and entry[0] must lie in [rule[2], rule[3]] */
  if ((((*(char *)((char *)entry + 0xd) == *(char *)((char *)rule + 5)) &&
       ((char)entry[3] == *(char *)(rule + 1))) && (*entry >= (uint)rule[2])) &&
     (*entry <= (uint)rule[3])) {
    /* callee must report 0, and the flag bit must agree between rule and entry */
    callee_result = guid_compare(entry[1],*rule);
    if ((callee_result == 0) && (((*(byte *)(rule + 4) ^ (byte)entry[6]) & 1) == 0)) {
      return 1;
    }
  }
  return 0;
}
