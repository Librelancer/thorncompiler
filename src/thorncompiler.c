#include "../lua/src/luac/luac_dump.h"
#include "thorncompiler.h"
#include "lparser.h"
#include "lstate.h"
#include "lzio.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
char *currentError;
THN_EXPORT int thn_compile(char *input, char *name, unsigned char **outputBuffer, int *outputSize)
{
    if(currentError)
        free(currentError);
    lua_open();
    ZIO z;
    zsopen(&z, input, name);
    TProtoFunc* Main = luaY_parser(&z);
    currentError = luac_geterror();
    if(currentError) {
        lua_close();
        return 0;
    } else {
        luaU_dumpchunk(Main, outputBuffer, outputSize);
        lua_close();
        return 1;
    }
}

THN_EXPORT char* thn_geterror()
{
    return strdup(currentError);
}

THN_EXPORT void thn_free(unsigned char *outputBuffer)
{
    luaU_freechunk(outputBuffer);
}
