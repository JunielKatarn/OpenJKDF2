#ifndef _STDPLATFORM_H
#define _STDPLATFORM_H

#define std_genBuffer ((char*)0x866880)
#define std_pHS (*((struct common_functions **)0x866C80))

#define stdFileFromPath_ADDR (0x427060)

static int (*stdPrintf)(int a1, char *a2, int line, char *fmt, ...) = 0x426D80;
static int (__cdecl *stdFileFromPath)(char *) = stdFileFromPath_ADDR;

#endif // _STDPLATFORM_H
