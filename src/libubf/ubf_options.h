/**
 * microbf brainfuck interpreter
 * copyright (C) iLiquid, 2019
 * licensed under the MIT license
 */

#ifndef ubf_options_h
#define ubf_options_h

/// The underlying data type for memory cells.
/// Must be a number that can be incremented and decremented using ++ and --.
#define UBF_MEM_TYPE int8_t

/// Set this to 0 if you're compiling onto a platform without stdio.
#define UBF_USE_STDIO 1

/// Set to 0 if you don't want to use computed gotos.
#define UBF_USE_COMPUTED_GOTO 1

/* -------------------------------------------------------------------------- */
/* INTERNAL FLAGS - DO NOT MODIFY DIRECTLY                                    */
/* -------------------------------------------------------------------------- */

#if defined(__GNUC__) || defined(__GNUG__)
# define UBF_VM_SUPPORT_COMPUTED_GOTO 1
#else
# define UBF_VM_SUPPORT_COMPUTED_GOTO 0
#endif

#if UBF_VM_SUPPORT_COMPUTED_GOTO && UBF_USE_COMPUTED_GOTO
# define UBF_VM_USE_COMPUTED_GOTO
#else
# define UBF_VM_USE_CASE_STMT
#endif

#endif
