#ifndef LUACOMPILER_H_
#define LUACOMPILER_H_

#ifdef WIN32
#define THN_EXPORT __declspec(dllexport)
#else
#define THN_EXPORT
#endif

THN_EXPORT int thn_compile(char *input, char *name, unsigned char **outputBuffer, int *outputSize);
THN_EXPORT void thn_free(unsigned char *outputBuffer);
THN_EXPORT char* thn_geterror();

#endif
