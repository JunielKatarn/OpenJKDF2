#include "sithDplay.h"

#include "jk.h"

int sithDplay_Startup()
{
    if ( sithDplay_bInitted )
        return 0;

#ifdef TARGET_HAS_DPLAY
    DirectPlay_Initialize();
#endif

#ifndef WIN32_BLOBS
    jkGuiMultiplayer_numConnections = 1;
    jk_snwprintf(jkGuiMultiplayer_aConnections[0].name, 0x80, L"OpenJKDF2 TCP");
#endif
    sithDplay_bInitted = 1;

    return 1;
}

#ifndef WIN32_BLOBS
int sithDplay_EarlyInit()
{
    return 0;
}

int sithDplay_OpenConnection(void* a)
{
    return 0;
}

void sithDplay_CloseConnection()
{
}

int sithDplay_Open(int a, void* b)
{
    return 0;
}

int sithDplay_seed_idk(void* a)
{
    return 0;
}

int sithDplay_CreatePlayer(void* a)
{
    return 0;
}

void sithDplay_DoReceive()
{

}

void sithDplay_Close()
{
    
}
#endif
