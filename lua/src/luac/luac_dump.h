#ifndef LUAC_DUMP
#define LUAC_DUMP
#include "lauxlib.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstring.h"
#include "lundump.h"
void luaU_dumpchunk(TProtoFunc* Main, unsigned char **output, int* outputSize);
void luaU_freechunk(unsigned char *chunk);
char *luac_geterror();

/* from opcode.c */
typedef struct
{
 char* name;				/* name of opcode */
 int op;				/* value of opcode */
 int class;				/* class of opcode (byte variant) */
 int args;				/* types of arguments (operands) */
 int arg;				/* arg #1 */
 int arg2;				/* arg #2 */
} Opcode;

int luaU_codesize(TProtoFunc* tf);

#define INFO(tf,p,I)	luaU_opcodeinfo(tf,p,I,__FILE__,__LINE__)

/* fake (but convenient) opcodes */
#define NOP	255
#define STACK	(-1)
#define ARGS	(-2)
#define VARARGS	(-3)

#endif
