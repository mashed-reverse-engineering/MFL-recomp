/* Struct layouts shared by several Rwp collision files (reconstructed; identical in every user). */
#ifndef RWP_COMMON_H
#define RWP_COMMON_H

typedef struct MflExt7 {
  int w[7];
} MflExt7;

typedef struct MflAxis3 {
  int w[3];
} MflAxis3;

typedef struct MflWorkArea {
  unsigned char pad00[8];
  unsigned int cur;   /* 0x8 */
} MflWorkArea;

typedef struct MflWorkBase {
  char *base;   /* 0x0 */
} MflWorkBase;

typedef struct MflSolveByVal34 {
  int w[34];
} MflSolveByVal34;

typedef struct MflPairFilter {
  int pad0;
  int stride;   /* 0x4 */
  int pad8;
  unsigned int bits[1];   /* 0xc */
} MflPairFilter;

typedef struct MflV3 {
  float v[3];
} MflV3;

typedef struct MflIndexList {
  int *items;
  unsigned int count;   /* 0x4 */
} MflIndexList;

typedef struct MflAggBox {
  float min[3];
  float pad0;
  float max[3];
  float pad1;
} MflAggBox;

typedef struct MflAggHandle {
  void *ptr;
  unsigned short id;   /* 0x4 */
  unsigned short pad;
} MflAggHandle;

typedef struct MflAxis {
  float n[3];
  float lo, hi;
} MflAxis;

/* The solver's row stream (the part used here; the row store is rwp_constraints_0057f8a0). */
typedef struct MflSolver {
  unsigned char pad00[0xac];
  int (*pairs)[2];   /* 0xac: body ids of the two sides per block (-1 = none) */
  unsigned char padb0[0x8];
  int *words;   /* 0xb8: rows used per block */
  int count;   /* 0xbc */
  unsigned char padc0[0x38];
  int cursor;   /* 0xf8 */
} MflSolver;

/* One constraint row, 27 words, initialised from the table g_rwp_constraint_padding. */
typedef struct MflRow {
  float lin1[3];   /* 0x0: Jacobian, linear part of side 1 */
  int pad0c;
  float ang1[3];   /* 0x10 */
  int pad1c;
  float lin2[3];   /* 0x20 */
  int pad2c;
  float ang2[3];   /* 0x30 */
  int pad3c;
  float rhs;   /* 0x40: target (error) */
  float vel;   /* 0x44: target velocity */
  float cfm;   /* 0x48 */
  float erp;   /* 0x4c */
  float lo2;   /* 0x50 */
  float hi2;   /* 0x54 */
  float lo;   /* 0x58 */
  float hi;   /* 0x5c */
  int tol;   /* 0x60: 0x3f4ccccd = 0.8f, or 0 */
  int pad64;
  int pad68;
} MflRow;

/* ---- rwp Dyn: bodies registered in a world and the bit sets that track them (rwp_dyn_0056a210, rwp_dyn_0056a7c0) ---- */
#ifndef RWP_DYN_TYPES
#define RWP_DYN_TYPES
#define BIT_SET(bits, id) ((bits)[(id) >> 5] |= 1 << ((id) & 0x1f))
#define BIT_CLR(bits, id) ((bits)[(id) >> 5] &= ~(1 << ((id) & 0x1f)))
#define BIT_TEST(bits, id) ((bits)[(id) >> 5] & (1 << ((id) & 0x1f)))

typedef struct MflDynBody MflDynBody;
typedef struct MflDynWorld MflDynWorld;

struct MflDynBody {
  int (**vtbl)();   /* 0x0: [4] add to its world, [5] remove, [6] query */
  unsigned char pad04[4];
  unsigned int flags;   /* 0x8 */
  unsigned char pad0c[0x14];
  unsigned short id;   /* 0x20 */
  unsigned char pad22[2];
  MflDynWorld *owner;   /* 0x24 */
};

typedef struct MflDynJoint {
  unsigned char pad00[0x50];
  MflDynBody *body_a;   /* 0x50 */
  unsigned short has_a;   /* 0x54: 0xffff = no body */
  unsigned char pad56[2];
  MflDynBody *body_b;   /* 0x58 */
  unsigned short has_b;   /* 0x5c */
} MflDynJoint;

typedef struct MflDynNode {
  MflDynJoint *joint;
  struct MflDynNode *next;   /* 0x4 */
} MflDynNode;

typedef struct MflDynPair {
  MflDynBody *a;
  short kind_a;   /* 0x4 */
  short pad6;
  MflDynBody *b;   /* 0x8 */
  short kind_b;   /* 0xc */
  short pad0e;
  int zero;   /* 0x10 */
} MflDynPair;

struct MflDynWorld {
  void *space;   /* 0x0 */
  unsigned int words;   /* 0x4: 32 bit words of every bit set */
  unsigned int flags;   /* 0x8 */
  int arg_c;   /* 0xc */
  void *tables;   /* 0x10: row tables (rwp_pipeline_00572a40) */
  unsigned int nlinks;   /* 0x14 */
  char *chain;   /* 0x18: 8 byte nodes chained through +4 */
  char *chain_head;   /* 0x1c */
  MflDynNode **links;   /* 0x20: body id -> list of joints */
  char *nodes_a;   /* 0x24: pool of 44 byte nodes */
  char **free_a;   /* 0x28 */
  unsigned int nfree_a;   /* 0x2c */
  unsigned int ncap_a;   /* 0x30 */
  char *nodes_b;   /* 0x34: pool of 156 byte nodes */
  char **free_b;   /* 0x38 */
  unsigned int nfree_b;   /* 0x3c */
  unsigned int ncap_b;   /* 0x40 */
  unsigned int idcap;   /* 0x44: capacity of the id list */
  unsigned int awake_max;   /* 0x48 */
  unsigned int awake_count;   /* 0x4c */
  MflDynBody **bodies;   /* 0x50: id -> body */
  void *graph;   /* 0x54 */
  unsigned int *bits_used;   /* 0x58: ids in use */
  unsigned int *bits_a;   /* 0x5c: wanted bodies */
  unsigned int *bits_awake;   /* 0x60 */
  unsigned int *bits_c;   /* 0x64 */
  unsigned int *bits_space;   /* 0x68: bodies registered with the space */
  unsigned int *bits_link;   /* 0x6c: nlinks bits */
  unsigned int *seen;   /* 0x70 */
  unsigned int *seen2;   /* 0x74 */
  unsigned int *added;   /* 0x78 */
  unsigned short *ids;   /* 0x7c */
  unsigned short *near_ids;   /* 0x80 */
};
#endif

#endif
