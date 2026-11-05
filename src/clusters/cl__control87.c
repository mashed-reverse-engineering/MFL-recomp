// cluster-entries: _control87 _controlfp
/* Installed cluster _control87 (B3 2026-11-04, hand-written; verified with diffemu cluster mode, spec output/cluster_pilot_b3/clusters.txt: the original's
   register helpers __abstract_cw / __hw_cw run for real inside the members).  Entries are patched at their original addresses by build_overlay.py --clusters. */
/* B3 (own CRT layer): ieee87.obj, the x87 control word interface.  _control87 (0x4be1a9) reads the hardware control word, translates it to the
   portable _MCW_* layout of float.h, merges the caller's bits under the mask, loads the translated result back and returns it; _controlfp
   (0x4be1db) is the same without the denormal mask bit.  The original's two translators (0x4be089 / 0x4be11b, register argument in ebx, only
   called from _control87) are the two loops below. */

/* float.h abstract control word: exception masks, rounding (_MCW_RC), precision (_MCW_PC), infinity control */
#define EM_DENORMAL 0x00080000
#define MCW_RC      0x00000300
#define MCW_PC      0x00030000
#define IC_AFFINE   0x00040000
/* x87 control word fields */
#define X87_RC_SHIFT 10
#define X87_PC_SHIFT 8
#define X87_IC       0x1000

/* exception mask bits: x87 bit -> float.h bit (IM->EM_INVALID, DM->EM_DENORMAL, ZM->EM_ZERODIVIDE, OM->EM_OVERFLOW, UM->EM_UNDERFLOW, PM->EM_INEXACT) */
static const unsigned int exception_bits[6][2] = {
    { 0x01, 0x00000010 }, { 0x02, 0x00080000 }, { 0x04, 0x00000008 }, { 0x08, 0x00000004 }, { 0x10, 0x00000002 }, { 0x20, 0x00000001 }
};
/* rounding: x87 RC field 0..3 (near, down, up, chop) <-> _RC_NEAR, _RC_DOWN, _RC_UP, _RC_CHOP = field << 8 */
/* precision: x87 PC field 0 = 24 bit, 2 = 53 bit, 3 = 64 bit (1 reserved); float.h _PC_24 0x20000, _PC_53 0x10000, _PC_64 0 */
static const unsigned int pc_abstract[4] = { 0x20000, 0, 0x10000, 0 };
static const unsigned int pc_hardware[4] = { 3, 2, 0, 0 };      /* indexed by abstract PC >> 16: _PC_64, _PC_53, _PC_24, invalid */

/* get / set the floating point control word: bits of `newctrl` under `mask` replace the current ones; returns the new abstract word */
unsigned int __cdecl _control87(unsigned int newctrl, unsigned int mask)
{
    unsigned short cw;
    unsigned int abstr, hw;
    int i;

    __asm fstcw cw
    abstr = 0;
    for (i = 0; i < 6; i++)
        if (cw & exception_bits[i][0])
            abstr |= exception_bits[i][1];
    abstr |= ((cw >> X87_RC_SHIFT) & 3) << 8;
    abstr |= pc_abstract[(cw >> X87_PC_SHIFT) & 3];
    if (cw & X87_IC)
        abstr |= IC_AFFINE;

    abstr = (abstr & ~mask) | (newctrl & mask);

    hw = 0;
    for (i = 0; i < 6; i++)
        if (abstr & exception_bits[i][1])
            hw |= exception_bits[i][0];
    hw |= ((abstr & MCW_RC) >> 8) << X87_RC_SHIFT;
    hw |= pc_hardware[(abstr & MCW_PC) >> 16] << X87_PC_SHIFT;
    if (abstr & IC_AFFINE)
        hw |= X87_IC;
    cw = (unsigned short)hw;
    __asm fldcw cw
    return abstr;
}

/* _control87 without the denormal exception mask (portable interface) */
unsigned int __cdecl _controlfp(unsigned int newctrl, unsigned int mask)
{
    return _control87(newctrl, mask & ~EM_DENORMAL);
}
