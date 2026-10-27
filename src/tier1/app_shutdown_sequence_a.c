#include "mfl_tier2.h"
/*004a4290*/ extern int platform_init();
/* Shutdown sequence entry point (name inferred from the function name; body only forwards).
 * Passes its six arguments unchanged to platform_init() and returns that result as bool.
 * Parameters: platform_arg_1..platform_arg_6 are forwarded verbatim; their meaning is not in the dossier.
 * Return: the int from platform_init() is converted to unsigned char, so only its low byte survives
 * (inferred from the types, check disassembly). platform_init is declared with no parameter list
 * but is called with six arguments. */
bool app_shutdown_sequence_a(undefined4 platform_arg_1, undefined4 platform_arg_2, undefined4 platform_arg_3, undefined4 platform_arg_4, undefined4 platform_arg_5, undefined4 platform_arg_6)
{
  return platform_init(platform_arg_1, platform_arg_2, platform_arg_3, platform_arg_4, platform_arg_5, platform_arg_6);
}
