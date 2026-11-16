/* tools/globals/z1_types.h: the types of the library .bss rows globgen emits as typed zero definitions (W11 Z1; z1_types.txt names the rows).
 * Declarations only (no storage), default packing.  Only types from our own sources (librw-mfl capi); no RenderWare SDK struct definitions (export deny rule). */
/* librw-mfl capi/d3d9vs.cpp: one entry of the exe's vertex shader cache (0x14 B) */
typedef struct mfl_vertex_shader_cache_entry { unsigned int key; unsigned int frame_stamp; unsigned int shader; unsigned int flag0; unsigned int flag1; } mfl_vertex_shader_cache_entry;
