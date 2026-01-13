/*
** $Id: dump.c,v 1.20 1999/07/02 19:34:26 lhf Exp $
** save bytecodes to file
** See Copyright Notice in lua.h
*/

//cmcging: Patched to not use FILE*, but a large buffer

#include <errno.h>
#include <stdlib.h>
#include "luac_dump.h"
#define ferror(e) (0)
#ifdef OLD_ANSI
#define strerror(e)     "(no error message provided by operating system)"
#endif

#define DumpBlock(b,size,D)	fswrite(b,size,1,D)
#define	DumpInt			DumpLong

typedef struct {
    unsigned char *chars;
    int capacity;
    int count;
} byteStream;
static void byteStream_init(byteStream *stream)
{
    stream->chars = (unsigned char*)malloc(500);
    stream->capacity = 500;
    stream->count = 0;
}

static void fsputc(int c, byteStream *stream)
{
    if((stream->count + 1) >= stream->capacity) {
        stream->capacity *= 2;
        stream->chars = realloc(stream->chars, stream->capacity);
    }
    stream->chars[stream->count++] = (unsigned char)c;
}
static void fswrite(void *ptr, size_t size, size_t count, byteStream *stream)
{
    unsigned char *bytes = (unsigned char*)ptr;
    for(size_t i = 0; i < (size * count); i++) {
        fsputc(bytes[i], stream);
    }
}
static void fsputs(const char *str, byteStream *stream)
{
    size_t len = strlen(str);
    for(size_t i = 0; i < len; i++)
        fsputc(str[i], stream);
}
static void byteStream_fini(byteStream *stream, unsigned char **outputBuffer, int *outputSize)
{
    *outputSize = stream->count;
    *outputBuffer = (unsigned char*)malloc(stream->count);
    memcpy(*outputBuffer, stream->chars, stream->count);
    free(stream->chars);
    stream->chars = NULL;
}


static void DumpWord(int i, byteStream* D)
{
 int hi= 0x0000FF & (i>>8);
 int lo= 0x0000FF &  i;
 fsputc(hi,D);
 fsputc(lo,D);
}

static void DumpLong(long i, byteStream* D)
{
 int hi= 0x00FFFF & (i>>16);
 int lo= 0x00FFFF & i;
 DumpWord(hi,D);
 DumpWord(lo,D);
}

static void DumpNumber(real x, byteStream* D, int native, TProtoFunc* tf)
{
 if (native)
  DumpBlock(&x,sizeof(x),D);
 else
 {
  char b[256];
  int n;
  sprintf(b,NUMBER_FMT"%n",x,&n);
  luaU_str2d(b,tf->source->str);	/* help lundump not to fail */
  fsputc(n,D);
  DumpBlock(b,n,D);
 }
}

static void DumpCode(TProtoFunc* tf, byteStream* D)
{
 int size=luaU_codesize(tf);
 DumpLong(size,D);
 DumpBlock(tf->code,size,D);
}

static void DumpString(char* s, int size, byteStream* D)
{
 if (s==NULL)
  DumpLong(0,D);
 else
 {
  DumpLong(size,D);
  DumpBlock(s,size,D);
 }
}

static void DumpTString(TaggedString* s, byteStream* D)
{
 if (s==NULL)
  DumpString(NULL,0,D);
 else
  DumpString(s->str,s->u.s.len+1,D);
}

static void DumpLocals(TProtoFunc* tf, byteStream* D)
{
 if (tf->locvars==NULL)
  DumpInt(0,D);
 else
 {
  LocVar* v;
  int n=0;
  for (v=tf->locvars; v->line>=0; v++)
   ++n;
  DumpInt(n,D);
  for (v=tf->locvars; v->line>=0; v++)
  {
   DumpInt(v->line,D);
   DumpTString(v->varname,D);
  }
 }
}

static void DumpFunction(TProtoFunc* tf, byteStream* D, int native);

static void DumpConstants(TProtoFunc* tf, byteStream* D, int native)
{
 int i,n=tf->nconsts;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  TObject* o=tf->consts+i;
  fsputc(-ttype(o),D);			/* ttype(o) is negative - ORDER LUA_T */
  switch (ttype(o))
  {
   case LUA_T_NUMBER:
	DumpNumber(nvalue(o),D,native,tf);
	break;
   case LUA_T_STRING:
	DumpTString(tsvalue(o),D);
	break;
   case LUA_T_PROTO:
	DumpFunction(tfvalue(o),D,native);
	break;
   case LUA_T_NIL:
	break;
   default:				/* cannot happen */
	luaU_badconstant("dump",i,o,tf);
	break;
  }
 }
}

static void DumpFunction(TProtoFunc* tf, byteStream* D, int native)
{
 DumpInt(tf->lineDefined,D);
 DumpTString(tf->source,D);
 DumpCode(tf,D);
 DumpLocals(tf,D);
 DumpConstants(tf,D,native);
 if (ferror(D))
  luaL_verror("write error" IN ": %s (errno=%d)",INLOC,strerror(errno),errno);
}

static void DumpHeader(TProtoFunc* Main, byteStream* D, int native)
{
 fsputc(ID_CHUNK,D);
 fsputs(SIGNATURE,D);
 fsputc(VERSION,D);
 if (native)
 {
  fsputc(sizeof(real),D);
  DumpNumber(TEST_NUMBER,D,native,Main);
 }
 else
  fsputc(0,D);
}

void luaU_dumpchunk(TProtoFunc* Main, unsigned char **output, int* outputSize)
{
    byteStream D;
    byteStream_init(&D);
    DumpHeader(Main, &D, 1);
    DumpFunction(Main, &D, 1);
    byteStream_fini(&D, output, outputSize);
}

void luaU_freechunk(unsigned char *chunk)
{
    free((void*)chunk);
}

