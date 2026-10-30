/* compile: /G5 */
#define ALIGN16(x) (((x) + 15) & ~15)
typedef struct { char pad[0x30]; int a; int b; int c; } RwpCounts;
/* Memory size of a batch arena: eight 16-byte aligned blocks (96 and 76 bytes per c, 128, 32 + 32, 20 + 20 and 16 per a, 32 + 256 and 288 per b). */
int rwp_batch_00578e80(RwpCounts *s)
{
  return ALIGN16(s->c * 96) + ALIGN16(s->c * 76)
       + ALIGN16(s->a * 128) + ALIGN16(s->a * 32 + 32) + ALIGN16(s->a * 20 + 20) + ALIGN16(s->a * 16)
       + ALIGN16(s->b * 32 + 256) + ALIGN16(s->b * 288);
}
