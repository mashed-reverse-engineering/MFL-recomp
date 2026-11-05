/* crt_i10_output = $I10_OUTPUT (0x004c25a9, libcmt x10fout.obj; C cannot spell the `$` name, the entry redirect binds it by address):
   80 bit long double -> decimal digit string for the printf / _fcvt family */
/* compile: /GS */
#include "mfl_tier2.h"

/* 12 byte internal long double of the CRT: 16 bit fill, 64 bit mantissa, 16 bit sign + exponent (little endian) */
typedef struct MflLd12 { unsigned char b[12]; } MflLd12;
#define LD12_EXP(x) (*(unsigned short *)&(x).b[10])

/* the floating output record (fltout.h FOS) */
typedef struct MflFos {
  short exp;   /* decimal exponent: value = 0.<man> * 10^exp */
  char sign;   /* '-' or ' ' */
  char manlen;   /* number of digits in man */
  char man[22];   /* digits, NUL terminated (21 + 1) */
} MflFos;

static const MflLd12 k_one_tenth = { { 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfb, 0x3f } };   /* 0.1, rounded up */
#define MAX_DIGITS 21
#define OUT_FIXED 1   /* output_flags: ndigits counts the digits behind the decimal point */

extern void __cdecl __add_12(MflLd12 *x, MflLd12 *y);
extern void __cdecl __shl_12(MflLd12 *x);
extern void __cdecl __shr_12(MflLd12 *x);
extern void __cdecl __ld12mul(MflLd12 *x, MflLd12 *y);
extern void __cdecl __multtenpow12(MflLd12 *x, int pow, unsigned mult12);
extern char *strcpy(char *d, const char *s);
#pragma function(strcpy)      /* a call to the CRT strcpy, as in libcmt (cl /O2 would expand the intrinsic) */

/* The long double comes by value as its three stack words: mantissa low / high and the 16 bit sign + exponent (upper half unused).
   Fills fos with up to ndigits significant digits (OUT_FIXED: digits behind the point), rounded, trailing zeros removed.
   Returns 1, or 0 for INF / NaN (fos->man = "1#INF", "1#IND", "1#QNAN" or "1#SNAN"). */
/* libcmt is built with frame pointers (/Oy-): the original runs on an ebp frame (/GS cookie xor [ebp+4], `leave`), and the callees
   get pointers into that frame */
#pragma optimize("y", off)
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int crt_i10_output(unsigned int manlo, unsigned int manhi, unsigned int sexp, int ndigits, unsigned int output_flags, MflFos *fos)
{
  /* the working set in the original's frame order (the callees get pointers into it) */
  struct {
    MflLd12 copy;
    MflLd12 one_tenth;   /* 0.1, rounded up */
    MflLd12 tmp;
    int left;   /* digits still to produce */
    int retval;
  } f;
  unsigned short expn, sign;
  int nexp, shift, n;
  char *p;

  sign = (unsigned short)(sexp & 0x8000);
  expn = (unsigned short)(sexp & 0x7fff);
  f.one_tenth = k_one_tenth;
  f.retval = 1;
  fos->sign = sign ? '-' : ' ';
  if (expn == 0 && manhi == 0 && manlo == 0)
    goto zero;

  if (expn == 0x7fff) {
    /* infinity or NaN */
    fos->exp = 1;
    if (!(manhi == 0x80000000 && manlo == 0) && !(manhi & 0x40000000)) {
      strcpy(fos->man, "1#SNAN");
      fos->manlen = 6;
    } else if (sign && manhi == 0xc0000000) {
      if (manlo == 0) {
        strcpy(fos->man, "1#IND");
        fos->manlen = 5;
      } else {
        strcpy(fos->man, "1#QNAN");
        fos->manlen = 6;
      }
    } else if (manhi == 0x80000000 && manlo == 0) {
      strcpy(fos->man, "1#INF");
      fos->manlen = 5;
    } else {
      strcpy(fos->man, "1#QNAN");
      fos->manlen = 6;
    }
    f.retval = 0;
    return f.retval;
  }

  /* decimal exponent estimate: log10(2) * binary exponent, from the exponent and the top mantissa byte (16.16 fixed point) */
  nexp = (short)((int)(expn * 0x4d10 + ((expn >> 8) + (manhi >> 24) * 2) * 0x4d - 0x134312f4) >> 16);
  *(unsigned short *)&f.tmp.b[0] = 0;
  *(unsigned int *)&f.tmp.b[2] = manlo;
  *(unsigned int *)&f.tmp.b[6] = manhi;
  LD12_EXP(f.tmp) = expn;
  __multtenpow12(&f.tmp, -nexp, 1);      /* f.tmp = value / 10^nexp */
  if (LD12_EXP(f.tmp) >= 0x3fff) {      /* >= 1: one more power of ten */
    nexp++;
    __ld12mul(&f.tmp, &f.one_tenth);
  }
  fos->exp = (short)nexp;

  if (output_flags & OUT_FIXED) {
    ndigits += nexp;
    if (ndigits <= 0)
      goto zero;
  }
  if (ndigits > MAX_DIGITS)
    ndigits = MAX_DIGITS;

  /* to a 0.64 fixed point fraction in the low 11 bytes */
  shift = LD12_EXP(f.tmp) - 0x3ffe;
  LD12_EXP(f.tmp) = 0;
  for (n = 8; n != 0; n--)
    __shl_12(&f.tmp);
  if (shift < 0)
    for (shift = -shift & 0xff; shift > 0; shift--)
      __shr_12(&f.tmp);

  /* digits: multiply by 10 (x * 4 + x, * 2), the top byte is the next digit */
  p = fos->man;
  for (f.left = ndigits + 1; f.left > 0; f.left--) {
    f.copy = f.tmp;
    __shl_12(&f.tmp);
    __shl_12(&f.tmp);
    __add_12(&f.tmp, &f.copy);
    __shl_12(&f.tmp);
    *p++ = f.tmp.b[11] + '0';
    f.tmp.b[11] = 0;
  }

  /* round on the extra digit */
  p--;
  if (*p-- >= '5') {
    while (p >= fos->man && *p == '9')
      *p-- = '0';
    if (p < fos->man) {
      p++;
      fos->exp++;
    }
    (*p)++;
  } else {
    while (p >= fos->man && *p == '0')
      p--;
    if (p < fos->man) {
      *fos->man = '0';
      goto zero_rest;
    }
  }
  fos->manlen = (char)(p - (char *)fos - 3);
  fos->man[fos->manlen] = 0;
  return f.retval;

zero:
  fos->man[0] = '0';
zero_rest:
  fos->exp = 0;
  fos->man[1] = 0;
  fos->sign = ' ';
  fos->manlen = 1;
  return 1;
}
