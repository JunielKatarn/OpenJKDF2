#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "hook.h"
#include "jk.h"
#include "types.h"

#include "Cog/sithCog.h"
#include "Cog/sithCogVm.h"
#include "Cog/jkCog.h"
#include "General/stdMath.h"
#include "Primitives/rdVector.h"
#include "General/stdConffile.h"
#include "General/stdFnames.h"
#include "General/stdHashTable.h"
#include "General/stdString.h"
#include "General/stdStrTable.h"
#include "General/sithStrTable.h"
#include "General/stdPcx.h"
#include "General/Darray.h"
#include "Gui/jkGUIRend.h"
#include "Gui/jkGUI.h"
#include "Gui/jkGUIForce.h"
#include "Gui/jkGUIEsc.h"
#include "Engine/rdroid.h"
#include "Engine/rdKeyframe.h"
#include "Engine/rdLight.h"
#include "Engine/rdMaterial.h"
#include "Engine/rdColormap.h"
#include "Engine/rdClip.h"
#include "Engine/rdCanvas.h"
#include "Engine/rdThing.h"
#include "Engine/sithTime.h"
#include "Engine/sithModel.h"
#include "Engine/sithParticle.h"
#include "Engine/sithSprite.h"
#include "Engine/sithSurface.h"
#include "Engine/sithTemplate.h"
#include "Engine/sithTimer.h"
#include "Engine/sithKeyFrame.h"
#include "Engine/sithSprite.h"
#include "Primitives/rdModel3.h"
#include "Primitives/rdPolyLine.h"
#include "Primitives/rdParticle.h"
#include "Primitives/rdSprite.h"
#include "Primitives/rdMatrix.h"
#include "Primitives/rdFace.h"
#include "Primitives/rdMath.h"
#include "World/sithWeapon.h"
#include "World/sithItem.h"
#include "World/sithWorld.h"
#include "World/sithInventory.h"
#include "World/jkPlayer.h"
#include "World/jkSaber.h"
#include "World/sithUnk3.h"
#include "Win95/std.h"
#include "Win95/stdGob.h"
#include "Win95/stdMci.h"
#include "Win95/stdConsole.h"
#include "AI/sithAI.h"
#include "AI/sithAIClass.h"
#include "AI/sithAICmd.h"
#include "Main/jkGob.h"
#include "Main/jkStrings.h"

int yyparse();

int jk_main(uint32_t hInstance, uint32_t hPrevInstance, char* lpCmdLine, int nShowCmd, char* lpWindowName)
{
    int result;
    WNDCLASSEXA wndClass;
    MSG msg;

    g_handler_count = 0;
    g_thing_two_some_dialog_count = 0;
    g_should_exit = 0;
    g_window_not_destroyed = 0;
    g_hInstance = hInstance;
    g_nShowCmd = nShowCmd;

    wndClass.cbSize = 48;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = "wKernel";
    wndClass.lpszMenuName = 0;
    wndClass.lpfnWndProc = wm_msg_main_handler;
    wndClass.style = 3;
    wndClass.hIcon = jk_LoadIconA(hInstance, "APPICON");
    if ( !wndClass.hIcon )
        wndClass.hIcon = jk_LoadIconA(0, 32512);
    wndClass.hIconSm = jk_LoadIconA(hInstance, "APPICON");
    if ( !wndClass.hIconSm )
        wndClass.hIconSm = jk_LoadIconA(0, 32512);
    wndClass.hCursor = jk_LoadCursorA(0, 0x7F00);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hbrBackground = jk_GetStockObject(4);

    if (jk_RegisterClassExA(&wndClass))
    {
        if ( jk_FindWindowA("wKernel", lpWindowName) )
            jk_exit(-1);

        uint32_t hres = jk_GetSystemMetrics(1);
        uint32_t vres = jk_GetSystemMetrics(0);
        g_hWnd = jk_CreateWindowExA(0x40000u, "wKernel", lpWindowName, 0x90000000, 0, 0, vres, hres, 0, 0, hInstance, 0);

        if (g_hWnd)
        {
            g_hInstance = (int)hInstance;
            jk_ShowWindow(g_hWnd, 1);
            jk_UpdateWindow(g_hWnd);
        }
    }

    sub_436D10(g_hWnd);
    sub_436D30(g_hInstance);
    jk_InitCommonControls();

    g_855E8C = 2 * jk_GetSystemMetrics(32);
    uint32_t metrics_32 = jk_GetSystemMetrics(32);
    g_855E90 = jk_GetSystemMetrics(15) + 2 * metrics_32;
    result = sub_401000(lpCmdLine);
    
    //jk_printf("aaa %x\n", &msg);

    if (!result) return result;

    
    g_window_not_destroyed = 1;

    while (1)
    {
        *(uint32_t*)0x8EE66C = 0x100;
        if (jk_PeekMessageA(&msg, 0, 0, 0, 0))
        {
            if (!jk_GetMessageA(&msg, 0, 0, 0))
            {
                result = msg.wParam;
                g_should_exit = 1;
                break;
            }

            uint32_t some_cnt = 0;
            if (g_thing_two_some_dialog_count > 0)
            {
#if 0
                v16 = &thing_three;
                do
                {
                    //TODO if ( jk_IsDialogMessageA(*v16, &msg) )
                    //  break;
                    ++some_cnt;
                    ++v16;
                }
                while ( some_cnt < g_thing_two_some_dialog_count );
#endif
            }

            if (some_cnt == g_thing_two_some_dialog_count)
            {
                jk_TranslateMessage(&msg);
                jk_DispatchMessageA(&msg);
            }

            if (!jk_PeekMessageA(&msg, 0, 0, 0, 0))
            {
                result = 0;
                if ( g_should_exit )
                    return result;
            }
        }

        //if (user32->stopping) break;

        other_window_stuff();
    }

    return result;
}

__declspec(dllexport) int WinMain_(uint32_t hInstance, uint32_t hPrevInstance, char* lpCmdLine, int nShowCmd)
{
    jk_main(hInstance, hPrevInstance, lpCmdLine, nShowCmd, "Jedi Knight");
    return 0;
}

__declspec(dllexport) void hook_init(void)
{
    jk_init();
    
    hook_function(WinMain_ADDR, WinMain_);
    
    // jkCog
    hook_function(jkCog_RegisterVerbs_ADDR, jkCog_RegisterVerbs);
    hook_function(jkCog_Initialize_ADDR, jkCog_Initialize);
    
    // sithCog
    hook_function(sithCog_Startup_ADDR, sithCog_Startup);
    hook_function(sithCog_SendMessageFromThing_ADDR, sithCog_SendMessageFromThing);
    hook_function(sithCogUtil_Initialize_ADDR, sithCogUtil_Initialize);
    hook_function(sithCogThing_Initialize_ADDR, sithCogThing_Initialize);
    hook_function(sithCogAI_Initialize_ADDR, sithCogAI_Initialize);
    hook_function(sithCogSurface_Initialize_ADDR, sithCogSurface_Initialize);
    hook_function(sithCogSound_Initialize_ADDR, sithCogSound_Initialize);
    hook_function(sithCogSector_Initialize_ADDR, sithCogSector_Initialize);
    hook_function(sithCogPlayer_Initialize_ADDR, sithCogPlayer_Initialize);
    
    // sithCogVm
    hook_function(sithCogVm_Startup_ADDR, sithCogVm_Startup);
    hook_function(sithCogVm_Shutdown_ADDR, sithCogVm_Shutdown);
    hook_function(sithCogVm_SetMsgFunc_ADDR, sithCogVm_SetMsgFunc);
    hook_function(sithCogVm_Set104_ADDR, sithCogVm_Set104);
    hook_function(sithCogVm_InvokeMsgByIdx_ADDR, sithCogVm_InvokeMsgByIdx);
    hook_function(sithCogVm_ClearMsgTmpBuf_ADDR, sithCogVm_ClearMsgTmpBuf);
    hook_function(sithCogVm_Exec_ADDR, sithCogVm_Exec);
    hook_function(sithCogVm_ExecCog_ADDR, sithCogVm_ExecCog);
    hook_function(sithCogVm_PopValue_ADDR, sithCogVm_PopValue);
    hook_function(sithCogVm_PopFlex_ADDR, sithCogVm_PopFlex);
    hook_function(sithCogVm_PopInt_ADDR, sithCogVm_PopInt);
    hook_function(sithCogVm_PopSymbolIdx_ADDR, sithCogVm_PopSymbolIdx);
    hook_function(sithCogVm_PopVector3_ADDR, sithCogVm_PopVector3);
    hook_function(sithCogVm_PopCog_ADDR, sithCogVm_PopCog);
    hook_function(sithCogVm_PopThing_ADDR, sithCogVm_PopThing);
    hook_function(sithCogVm_PopTemplate_ADDR, sithCogVm_PopTemplate);
    hook_function(sithCogVm_PopSound_ADDR, sithCogVm_PopSound);
    hook_function(sithCogVm_PopSector_ADDR, sithCogVm_PopSector);
    hook_function(sithCogVm_PopSurface_ADDR, sithCogVm_PopSurface);
    hook_function(sithCogVm_PopMaterial_ADDR, sithCogVm_PopMaterial);
    hook_function(sithCogVm_PopModel3_ADDR, sithCogVm_PopModel3);
    hook_function(sithCogVm_PopKeyframe_ADDR, sithCogVm_PopKeyframe);
    hook_function(sithCogVm_PopString_ADDR, sithCogVm_PopString);
    hook_function(sithCogVm_PushVar_ADDR, sithCogVm_PushVar);
    hook_function(sithCogVm_PushInt_ADDR, sithCogVm_PushInt);
    hook_function(sithCogVm_PushFlex_ADDR, sithCogVm_PushFlex);
    hook_function(sithCogVm_PushVector3_ADDR, sithCogVm_PushVector3);
    hook_function(sithCogVm_PopProgramVal_ADDR, sithCogVm_PopProgramVal);
    hook_function(sithCogVm_ResetStack_ADDR, sithCogVm_ResetStack);
    hook_function(sithCogVm_Call_ADDR, sithCogVm_Call);
    hook_function(sithCogVm_Ret_ADDR, sithCogVm_Ret);
    hook_function(sithCogVm_PopStackVar_ADDR, sithCogVm_PopStackVar);
    hook_function(sithCogVm_AssignStackVar_ADDR, sithCogVm_AssignStackVar);
    
    // stdMath
    hook_function(stdMath_FlexPower_ADDR, stdMath_FlexPower);
    hook_function(stdMath_NormalizeAngle_ADDR, stdMath_NormalizeAngle);
    hook_function(stdMath_NormalizeAngleAcute_ADDR, stdMath_NormalizeAngleAcute);
    hook_function(stdMath_NormalizeDeltaAngle_ADDR, stdMath_NormalizeDeltaAngle);
    //hook_function(stdMath_SinCos_ADDR, stdMath_SinCos);
    hook_function(stdMath_Dist2D1_ADDR, stdMath_Dist2D1);
    hook_function(stdMath_Dist2D2_ADDR, stdMath_Dist2D2);
    hook_function(stdMath_Dist2D3_ADDR, stdMath_Dist2D3);
    hook_function(stdMath_Dist2D4_ADDR, stdMath_Dist2D4);
    hook_function(stdMath_Dist3D1_ADDR, stdMath_Dist3D1);
    hook_function(stdMath_Dist3D2_ADDR, stdMath_Dist3D2);
    hook_function(stdMath_Dist3D3_ADDR, stdMath_Dist3D3);
    hook_function(stdMath_Floor_ADDR, stdMath_Floor);
    hook_function(stdMath_Sqrt_ADDR, stdMath_Sqrt);
    
    // rdVector
    hook_function(rdVector_Set2_ADDR, rdVector_Set2);
    hook_function(rdVector_Set3_ADDR, rdVector_Set3);
    hook_function(rdVector_Set4_ADDR, rdVector_Set4);
    hook_function(rdVector_Copy2_ADDR, rdVector_Copy2);
    hook_function(rdVector_Copy3_ADDR, rdVector_Copy3);
    hook_function(rdVector_Copy4_ADDR, rdVector_Copy4);
    hook_function(rdVector_Neg2_ADDR, rdVector_Neg2);
    hook_function(rdVector_Neg3_ADDR, rdVector_Neg3);
    hook_function(rdVector_Neg4_ADDR, rdVector_Neg4);
    hook_function(rdVector_Neg2Acc_ADDR, rdVector_Neg2Acc);
    hook_function(rdVector_Neg3Acc_ADDR, rdVector_Neg3Acc);
    hook_function(rdVector_Neg4Acc_ADDR, rdVector_Neg4Acc);
    hook_function(rdVector_Add2_ADDR, rdVector_Add2);
    hook_function(rdVector_Add3_ADDR, rdVector_Add3);
    hook_function(rdVector_Add4_ADDR, rdVector_Add4);
    hook_function(rdVector_Add2Acc_ADDR, rdVector_Add2Acc);
    hook_function(rdVector_Add3Acc_ADDR, rdVector_Add3Acc);
    hook_function(rdVector_Add4Acc_ADDR, rdVector_Add4Acc);
    hook_function(rdVector_Sub2_ADDR, rdVector_Sub2);
    hook_function(rdVector_Sub3_ADDR, rdVector_Sub3);
    hook_function(rdVector_Sub4_ADDR, rdVector_Sub4);
    hook_function(rdVector_Sub2Acc_ADDR, rdVector_Sub2Acc);
    hook_function(rdVector_Sub3Acc_ADDR, rdVector_Sub3Acc);
    hook_function(rdVector_Sub4Acc_ADDR, rdVector_Sub4Acc);
    hook_function(rdVector_Dot2_ADDR, rdVector_Dot2);
    hook_function(rdVector_Dot3_ADDR, rdVector_Dot3);
    hook_function(rdVector_Dot4_ADDR, rdVector_Dot4);
    hook_function(rdVector_Cross3_ADDR, rdVector_Cross3);
    hook_function(rdVector_Cross3Acc_ADDR, rdVector_Cross3Acc);
    hook_function(rdVector_Len2_ADDR, rdVector_Len2);
    hook_function(rdVector_Len3_ADDR, rdVector_Len3);
    hook_function(rdVector_Len4_ADDR, rdVector_Len4);
    hook_function(rdVector_Normalize2_ADDR, rdVector_Normalize2);
    hook_function(rdVector_Normalize3_ADDR, rdVector_Normalize3);
    hook_function(rdVector_Normalize3Quick_ADDR, rdVector_Normalize3Quick);
    hook_function(rdVector_Normalize4_ADDR, rdVector_Normalize4);
    hook_function(rdVector_Normalize2Acc_ADDR, rdVector_Normalize2Acc);
    hook_function(rdVector_Normalize3Acc_ADDR, rdVector_Normalize3Acc);
    hook_function(rdVector_Normalize3QuickAcc_ADDR, rdVector_Normalize3QuickAcc);
    hook_function(rdVector_Normalize4Acc_ADDR, rdVector_Normalize4Acc);
    hook_function(rdVector_Scale2_ADDR, rdVector_Scale2);
    hook_function(rdVector_Scale3_ADDR, rdVector_Scale3);
    hook_function(rdVector_Scale4_ADDR, rdVector_Scale4);
    hook_function(rdVector_Scale2Acc_ADDR, rdVector_Scale2Acc);
    hook_function(rdVector_Scale3Acc_ADDR, rdVector_Scale3Acc);
    hook_function(rdVector_Scale4Acc_ADDR, rdVector_Scale4Acc);
    hook_function(rdVector_InvScale2_ADDR, rdVector_InvScale2);
    hook_function(rdVector_InvScale3_ADDR, rdVector_InvScale3);
    hook_function(rdVector_InvScale4_ADDR, rdVector_InvScale4);
    hook_function(rdVector_InvScale2Acc_ADDR, rdVector_InvScale2Acc);
    hook_function(rdVector_InvScale3Acc_ADDR, rdVector_InvScale3Acc);
    hook_function(rdVector_InvScale4Acc_ADDR, rdVector_InvScale4Acc);
    hook_function(rdVector_Rotate3_ADDR, rdVector_Rotate3);
    hook_function(rdVector_Rotate3Acc_ADDR, rdVector_Rotate3Acc);
    hook_function(rdVector_ExtractAngle_ADDR, rdVector_ExtractAngle);
    
    // sithCogParse
    //hook_function(sithCogYACC_yyparse_ADDR, yyparse);
    
    // std
    hook_function(stdCalcBitPos_ADDR, stdCalcBitPos);
    hook_function(stdReadRaw_ADDR, stdReadRaw);
    hook_function(stdFGetc_ADDR, stdFGetc);
    hook_function(stdFPutc_ADDR, stdFPutc);
    
    // stdConffile
    hook_function(stdConffile_OpenRead_ADDR, stdConffile_OpenRead);
    hook_function(stdConffile_OpenWrite_ADDR, stdConffile_OpenWrite);
    hook_function(stdConffile_OpenMode_ADDR, stdConffile_OpenMode);
    hook_function(stdConffile_Close_ADDR, stdConffile_Close);
    hook_function(stdConffile_CloseWrite_ADDR, stdConffile_CloseWrite);
    hook_function(stdConffile_WriteLine_ADDR, stdConffile_WriteLine);
    hook_function(stdConffile_Write_ADDR, stdConffile_Write);
    hook_function(stdConffile_Printf_ADDR, stdConffile_Printf);
    hook_function(stdConffile_Read_ADDR, stdConffile_Read);
    hook_function(stdConffile_ReadArgsFromStr_ADDR, stdConffile_ReadArgsFromStr);
    hook_function(stdConffile_ReadArgs_ADDR, stdConffile_ReadArgs);
    hook_function(stdConffile_ReadLine_ADDR, stdConffile_ReadLine);
    hook_function(stdConffile_GetFileHandle_ADDR, stdConffile_GetFileHandle);
    
    // stdFnames
    hook_function(stdFnames_FindMedName_ADDR, stdFnames_FindMedName);
    hook_function(stdFnames_FindExt_ADDR, stdFnames_FindExt);
    hook_function(stdFnames_AddDefaultExt_ADDR, stdFnames_AddDefaultExt);
    hook_function(stdFnames_StripExt_ADDR, stdFnames_StripExt);
    hook_function(stdFnames_StripExtAndDot_ADDR, stdFnames_StripExtAndDot);
    hook_function(stdFnames_ChangeExt_ADDR, stdFnames_ChangeExt);
    hook_function(stdFnames_StripDirAndExt_ADDR, stdFnames_StripDirAndExt);
    hook_function(stdFnames_CopyExt_ADDR, stdFnames_CopyExt);
    hook_function(stdFnames_CopyMedName_ADDR, stdFnames_CopyMedName);
    hook_function(stdFnames_CopyDir_ADDR, stdFnames_CopyDir);
    hook_function(stdFnames_CopyShortName_ADDR, stdFnames_CopyShortName);
    hook_function(stdFnames_Concat_ADDR, stdFnames_Concat);
    hook_function(stdFnames_MakePath_ADDR, stdFnames_MakePath);
    hook_function(stdFnames_MakePath3_ADDR, stdFnames_MakePath3);
    
    // stdGob
    hook_function(stdGob_Startup_ADDR, stdGob_Startup);
    hook_function(stdGob_Shutdown_ADDR, stdGob_Shutdown);
    hook_function(stdGob_Load_ADDR, stdGob_Load);
    hook_function(stdGob_LoadEntry_ADDR, stdGob_LoadEntry);
    hook_function(stdGob_Free_ADDR, stdGob_Free);
    hook_function(stdGob_FreeEntry_ADDR, stdGob_FreeEntry);
    hook_function(stdGob_FileOpen_ADDR, stdGob_FileOpen);
    hook_function(stdGob_FileClose_ADDR, stdGob_FileClose);
    hook_function(stdGob_FSeek_ADDR, stdGob_FSeek);
    hook_function(stdGob_FTell_ADDR, stdGob_FTell);
    hook_function(stdGob_FEof_ADDR, stdGob_FEof);
    hook_function(stdGob_FileRead_ADDR, stdGob_FileRead);
    hook_function(stdGob_FileGets_ADDR, stdGob_FileGets);
    hook_function(stdGob_FileGetws_ADDR, stdGob_FileGetws);
    
    // stdMci
    hook_function(stdMci_Startup_ADDR, stdMci_Startup);
    hook_function(stdMci_Shutdown_ADDR, stdMci_Shutdown);
    hook_function(stdMci_Play_ADDR, stdMci_Play);
    hook_function(stdMci_SetVolume_ADDR, stdMci_SetVolume);
    hook_function(stdMci_Stop_ADDR, stdMci_Stop);
    hook_function(stdMci_CheckStatus_ADDR, stdMci_CheckStatus);
    hook_function(stdMci_GetTrackLength_ADDR, stdMci_GetTrackLength);
    
    // stdHashTable
    hook_function(stdHashTable_HashStringToIdx_ADDR, stdHashTable_HashStringToIdx);
    hook_function(stdHashTable_New_ADDR, stdHashTable_New);
    hook_function(stdHashTable_GetBucketTail_ADDR, stdHashTable_GetBucketTail);
    hook_function(stdHashTable_FreeBuckets_ADDR, stdHashTable_FreeBuckets);
    hook_function(stdHashTable_Free_ADDR, stdHashTable_Free);
    hook_function(stdHashTable_SetKeyVal_ADDR, stdHashTable_SetKeyVal);
    hook_function(stdHashTable_GetKeyVal_ADDR, stdHashTable_GetKeyVal);
    hook_function(stdHashTable_FreeKey_ADDR, stdHashTable_FreeKey);
    hook_function(stdHashTable_PrintDiagnostics_ADDR, stdHashTable_PrintDiagnostics);
    hook_function(stdHashTable_Dump_ADDR, stdHashTable_Dump);
    hook_function(stdHashKey_AddLink_ADDR, stdHashKey_AddLink);
    hook_function(stdHashKey_InsertAtTop_ADDR, stdHashKey_InsertAtTop);
    hook_function(stdHashKey_InsertAtEnd_ADDR, stdHashKey_InsertAtEnd);
    hook_function(stdHashKey_UnlinkChild_ADDR, stdHashKey_UnlinkChild);
    hook_function(stdHashKey_NumChildren_ADDR, stdHashKey_NumChildren);
    hook_function(stdHashKey_DisownMaybe_ADDR, stdHashKey_DisownMaybe);
    hook_function(stdHashKey_OrphanAndDisown_ADDR, stdHashKey_OrphanAndDisown);
    hook_function(stdHashKey_GetNthChild_ADDR, stdHashKey_GetNthChild);
    hook_function(stdHashKey_GetFirstParent_ADDR, stdHashKey_GetFirstParent);
    
    // stdString
    hook_function(stdString_FastCopy_ADDR, stdString_FastCopy);
    hook_function(stdString_snprintf_ADDR, stdString_snprintf);
    hook_function(stdString_CopyBetweenDelimiter_ADDR, stdString_CopyBetweenDelimiter);
    hook_function(stdString_GetQuotedStringContents_ADDR, stdString_GetQuotedStringContents);
    hook_function(stdString_CharToWchar_ADDR, stdString_CharToWchar);
    hook_function(stdString_WcharToChar_ADDR, stdString_WcharToChar);
    hook_function(stdString_wstrncpy_ADDR, stdString_wstrncpy);
    hook_function(stdString_wstrncat_ADDR, stdString_wstrncat);
    hook_function(stdString_CstrCopy_ADDR, stdString_CstrCopy);
    hook_function(stdString_WcharCopy_ADDR, stdString_WcharCopy);
    hook_function(stdString_CStrToLower_ADDR, stdString_CStrToLower);
    
    // stdStrTable
    hook_function(stdStrTable_Free_ADDR, stdStrTable_Free);
    hook_function(stdStrTable_GetUniString_ADDR, stdStrTable_GetUniString);
    hook_function(stdStrTable_GetString_ADDR, stdStrTable_GetString);
    
    // stdPcx
    hook_function(stdPcx_Load_ADDR, stdPcx_Load);
    hook_function(stdPcx_Write_ADDR, stdPcx_Write);
    
    // sithStrTable
    hook_function(sithStrTable_Startup_ADDR, sithStrTable_Startup);
    hook_function(sithStrTable_Shutdown_ADDR, sithStrTable_Shutdown);
    hook_function(sithStrTable_GetUniString_ADDR, sithStrTable_GetUniString);
    hook_function(sithStrTable_GetString_ADDR, sithStrTable_GetString);
    
    // stdConsole
    hook_function(stdConsole_Startup_ADDR, stdConsole_Startup);
    hook_function(stdConsole_Shutdown_ADDR, stdConsole_Shutdown);
    hook_function(stdConsole_New_ADDR, stdConsole_New);
    hook_function(stdConsole_Free_ADDR, stdConsole_Free);
    hook_function(stdConsole_SetCursorPos_ADDR, stdConsole_SetCursorPos);
    hook_function(stdConsole_GetCursorPos_ADDR, stdConsole_GetCursorPos);
    hook_function(stdConsole_ToggleCursor_ADDR, stdConsole_ToggleCursor);
    hook_function(stdConsole_GetTextAttribute_ADDR, stdConsole_GetTextAttribute);
    hook_function(stdConsole_SetTextAttribute_ADDR, stdConsole_SetTextAttribute);
    hook_function(stdConsole_Flush_ADDR, stdConsole_Flush);
    hook_function(stdConsole_Clear_ADDR, stdConsole_Clear);
    hook_function(stdConsole_Reset_ADDR, stdConsole_Reset);
    hook_function(stdConsole_Putc_ADDR, stdConsole_Putc);
    hook_function(stdConsole_Puts_ADDR, stdConsole_Puts);
    hook_function(stdConsole_ClearBuf_ADDR, stdConsole_ClearBuf);
    hook_function(stdConsole_ClearBuf2_ADDR, stdConsole_ClearBuf2);
    hook_function(stdConsole_WriteBorderMaybe_ADDR, stdConsole_WriteBorderMaybe);
    hook_function(stdConsole_WriteBorderMaybe2_ADDR, stdConsole_WriteBorderMaybe2);
    hook_function(stdConsole_WriteBorderMaybe3_ADDR, stdConsole_WriteBorderMaybe3);
    hook_function(stdConsole_WriteBorderMaybe4_ADDR, stdConsole_WriteBorderMaybe4);
    
    // rdroid
    hook_function(rdStartup_ADDR, rdStartup);
    hook_function(rdShutdown_ADDR, rdShutdown);
    hook_function(rdOpen_ADDR, rdOpen);
    hook_function(rdClose_ADDR, rdClose);
    hook_function(rdSetRenderOptions_ADDR, rdSetRenderOptions);
    hook_function(rdSetGeometryMode_ADDR, rdSetGeometryMode);
    hook_function(rdSetLightingMode_ADDR, rdSetLightingMode);
    hook_function(rdSetTextureMode_ADDR, rdSetTextureMode);
    hook_function(rdSetSortingMethod_ADDR, rdSetSortingMethod);
    hook_function(rdSetOcclusionMethod_ADDR, rdSetOcclusionMethod);
    hook_function(rdSetZBufferMethod_ADDR, rdSetZBufferMethod);
    hook_function(rdSetCullFlags_ADDR, rdSetCullFlags);
    hook_function(rdSetProcFaceUserData_ADDR, rdSetProcFaceUserData);
    hook_function(rdGetRenderOptions_ADDR, rdGetRenderOptions);
    hook_function(rdGetGeometryMode_ADDR, rdGetGeometryMode);
    hook_function(rdGetLightingMode_ADDR, rdGetLightingMode);
    hook_function(rdGetTextureMode_ADDR, rdGetTextureMode);
    hook_function(rdGetSortingMethod_ADDR, rdGetSortingMethod);
    hook_function(rdGetOcclusionMethod_ADDR, rdGetOcclusionMethod);
    hook_function(rdGetZBufferMethod_ADDR, rdGetZBufferMethod);
    hook_function(rdGetCullFlags_ADDR, rdGetCullFlags);
    hook_function(rdGetProcFaceUserData_ADDR, rdGetProcFaceUserData);
    hook_function(rdSetMipDistances_ADDR, rdSetMipDistances);
    hook_function(rdSetColorEffects_ADDR, rdSetColorEffects);
    hook_function(rdAdvanceFrame_ADDR, rdAdvanceFrame);
    hook_function(rdFinishFrame_ADDR, rdFinishFrame);
    hook_function(rdClearPostStatistics_ADDR, rdClearPostStatistics);
    
    // rdKeyframe
    hook_function(rdKeyframe_RegisterLoader_ADDR, rdKeyframe_RegisterLoader);
    hook_function(rdKeyframe_RegisterUnloader_ADDR, rdKeyframe_RegisterUnloader);
    hook_function(rdKeyframe_NewEntry_ADDR, rdKeyframe_NewEntry);
    hook_function(rdKeyframe_Load_ADDR, rdKeyframe_Load);
    hook_function(rdKeyframe_LoadEntry_ADDR, rdKeyframe_LoadEntry);
    hook_function(rdKeyframe_Write_ADDR, rdKeyframe_Write);
    hook_function(rdKeyframe_FreeEntry_ADDR, rdKeyframe_FreeEntry);
    hook_function(rdKeyframe_FreeJoints_ADDR, rdKeyframe_FreeJoints);
    
    // rdLight
    hook_function(rdLight_New_ADDR, rdLight_New);
    hook_function(rdLight_NewEntry_ADDR, rdLight_NewEntry);
    hook_function(rdLight_Free_ADDR, rdLight_Free);
    hook_function(rdLight_FreeEntry_ADDR, rdLight_FreeEntry);
    hook_function(rdLight_CalcVertexIntensities_ADDR, rdLight_CalcVertexIntensities);
    hook_function(rdLight_CalcFaceIntensity_ADDR, rdLight_CalcFaceIntensity);
    
    // rdMaterial
    hook_function(rdMaterial_RegisterLoader_ADDR, rdMaterial_RegisterLoader);
    hook_function(rdMaterial_RegisterUnloader_ADDR, rdMaterial_RegisterUnloader);
    hook_function(rdMaterial_Load_ADDR, rdMaterial_Load);
    hook_function(rdMaterial_LoadEntry_ADDR, rdMaterial_LoadEntry);
    hook_function(rdMaterial_Free_ADDR, rdMaterial_Free);
    hook_function(rdMaterial_FreeEntry_ADDR, rdMaterial_FreeEntry);
    
    // rdPolyLine
    hook_function(rdPolyLine_New_ADDR, rdPolyLine_New);
    hook_function(rdPolyLine_NewEntry_ADDR, rdPolyLine_NewEntry);
    hook_function(rdPolyLine_Free_ADDR, rdPolyLine_Free);
    hook_function(rdPolyLine_FreeEntry_ADDR, rdPolyLine_FreeEntry);
    hook_function(rdPolyLine_Draw_ADDR, rdPolyLine_Draw);
    hook_function(rdPolyLine_DrawFace_ADDR, rdPolyLine_DrawFace);
    
    // rdColormap
    hook_function(rdColormap_SetCurrent_ADDR, rdColormap_SetCurrent);
    hook_function(rdColormap_SetIdentity_ADDR, rdColormap_SetIdentity);
    hook_function(rdColormap_Load_ADDR, rdColormap_Load);
    hook_function(rdColormap_Free_ADDR, rdColormap_Free);
    hook_function(rdColormap_FreeEntry_ADDR, rdColormap_FreeEntry);
    hook_function(rdColormap_Write_ADDR, rdColormap_Write);
    
    // rdClip
    hook_function(rdClip_Line2_ADDR, rdClip_Line2);
    hook_function(rdClip_CalcOutcode2_ADDR, rdClip_CalcOutcode2);
    
    // rdFace
    hook_function(rdFace_New_ADDR, rdFace_New);
    hook_function(rdFace_NewEntry_ADDR, rdFace_NewEntry);
    hook_function(rdFace_Free_ADDR, rdFace_Free);
    hook_function(rdFace_FreeEntry_ADDR, rdFace_FreeEntry);
    
    // rdMath
    hook_function(rdMath_CalcSurfaceNormal_ADDR, rdMath_CalcSurfaceNormal);
    hook_function(rdMath_DistancePointToPlane_ADDR, rdMath_DistancePointToPlane);
    hook_function(rdMath_DeltaAngleNormalizedAbs_ADDR, rdMath_DeltaAngleNormalizedAbs);
    hook_function(rdMath_DeltaAngleNormalized_ADDR, rdMath_DeltaAngleNormalized);
    hook_function(rdMath_ClampVector_ADDR, rdMath_ClampVector);
    hook_function(rdMath_PointsCollinear_ADDR, rdMath_PointsCollinear);
    
    // rdCanvas
    hook_function(rdCanvas_New_ADDR, rdCanvas_New);
    hook_function(rdCanvas_NewEntry_ADDR, rdCanvas_NewEntry);
    hook_function(rdCanvas_Free_ADDR, rdCanvas_Free);
    hook_function(rdCanvas_FreeEntry_ADDR, rdCanvas_FreeEntry);
    
    // rdModel3
    hook_function(rdModel3_RegisterLoader_ADDR, rdModel3_RegisterLoader);
    hook_function(rdModel3_RegisterUnloader_ADDR, rdModel3_RegisterUnloader);
    hook_function(rdModel3_ClearFrameCounters_ADDR, rdModel3_ClearFrameCounters);
    hook_function(rdModel3_NewEntry_ADDR, rdModel3_NewEntry);
    hook_function(rdModel3_New_ADDR, rdModel3_New);
    hook_function(rdModel3_Load_ADDR, rdModel3_Load);
    hook_function(rdModel3_LoadPostProcess_ADDR, rdModel3_LoadPostProcess);
    hook_function(rdModel3_WriteText_ADDR, rdModel3_WriteText);
    hook_function(rdModel3_Free_ADDR, rdModel3_Free);
    hook_function(rdModel3_FreeEntry_ADDR, rdModel3_FreeEntry);
    hook_function(rdModel3_FreeEntryGeometryOnly_ADDR, rdModel3_FreeEntryGeometryOnly);
    hook_function(rdModel3_Validate_ADDR, rdModel3_Validate);
    hook_function(rdModel3_CalcBoundingBoxes_ADDR, rdModel3_CalcBoundingBoxes);
    hook_function(rdModel3_BuildExpandedRadius_ADDR, rdModel3_BuildExpandedRadius);
    hook_function(rdModel3_CalcFaceNormals_ADDR, rdModel3_CalcFaceNormals);
    //hook_function(rdModel3_CalcVertexNormals_ADDR, rdModel3_CalcVertexNormals);
    hook_function(rdModel3_FindNamedNode_ADDR, rdModel3_FindNamedNode);
    hook_function(rdModel3_GetMeshMatrix_ADDR, rdModel3_GetMeshMatrix);
    hook_function(rdModel3_ReplaceMesh_ADDR, rdModel3_ReplaceMesh);
    hook_function(rdModel3_Draw_ADDR, rdModel3_Draw);
    hook_function(rdModel3_DrawHNode_ADDR, rdModel3_DrawHNode);
    hook_function(rdModel3_DrawMesh_ADDR,rdModel3_DrawMesh);
    hook_function(rdModel3_DrawFace_ADDR,rdModel3_DrawFace);
    
    // rdParticle
    hook_function(rdParticle_RegisterLoader_ADDR, rdParticle_RegisterLoader);
    hook_function(rdParticle_New_ADDR, rdParticle_New);
    hook_function(rdParticle_NewEntry_ADDR, rdParticle_NewEntry);
    hook_function(rdParticle_Clone_ADDR, rdParticle_Clone);
    hook_function(rdParticle_Free_ADDR, rdParticle_Free);
    hook_function(rdParticle_FreeEntry_ADDR, rdParticle_FreeEntry);
    hook_function(rdParticle_Load_ADDR, rdParticle_Load);
    hook_function(rdParticle_LoadEntry_ADDR, rdParticle_LoadEntry);
    hook_function(rdParticle_Write_ADDR, rdParticle_Write);
    hook_function(rdParticle_Draw_ADDR, rdParticle_Draw);
    
    // rdSprite
    hook_function(rdSprite_New_ADDR, rdSprite_New);
    hook_function(rdSprite_NewEntry_ADDR, rdSprite_NewEntry);
    hook_function(rdSprite_Free_ADDR, rdSprite_Free);
    hook_function(rdSprite_FreeEntry_ADDR, rdSprite_FreeEntry);
    hook_function(rdSprite_Draw_ADDR, rdSprite_Draw);
    
    // rdThing
    hook_function(rdThing_New_ADDR, rdThing_New);
    hook_function(rdThing_NewEntry_ADDR, rdThing_NewEntry);
    hook_function(rdThing_Free_ADDR, rdThing_Free);
    hook_function(rdThing_FreeEntry_ADDR, rdThing_FreeEntry);
    hook_function(rdThing_SetModel3_ADDR, rdThing_SetModel3);
    hook_function(rdThing_SetCamera_ADDR, rdThing_SetCamera);
    hook_function(rdThing_SetLight_ADDR, rdThing_SetLight);
    hook_function(rdThing_SetSprite3_ADDR, rdThing_SetSprite3);
    hook_function(rdThing_SetPolyline_ADDR, rdThing_SetPolyline);
    hook_function(rdThing_Draw_ADDR, rdThing_Draw);
    hook_function(rdThing_AccumulateMatrices_ADDR, rdThing_AccumulateMatrices);
    
    // rdMatrix
    hook_function(rdMatrix_Build34_ADDR, rdMatrix_Build34);
    hook_function(rdMatrix_BuildFromLook34_ADDR, rdMatrix_BuildFromLook34);
    hook_function(rdMatrix_BuildCamera34_ADDR, rdMatrix_BuildCamera34);
    hook_function(rdMatrix_InvertOrtho34_ADDR, rdMatrix_InvertOrtho34);
    hook_function(rdMatrix_Build44_ADDR, rdMatrix_Build44);
    hook_function(rdMatrix_BuildRotate34_ADDR, rdMatrix_BuildRotate34);
    hook_function(rdMatrix_BuildInverseRotate34_ADDR, rdMatrix_BuildInverseRotate34);
    hook_function(rdMatrix_BuildRotate44_ADDR, rdMatrix_BuildRotate44);
    hook_function(rdMatrix_BuildTranslate34_ADDR, rdMatrix_BuildTranslate34);
    hook_function(rdMatrix_BuildTranslate44_ADDR, rdMatrix_BuildTranslate44);
    hook_function(rdMatrix_BuildScale34_ADDR, rdMatrix_BuildScale34);
    hook_function(rdMatrix_BuildScale44_ADDR, rdMatrix_BuildScale44);
    hook_function(rdMatrix_BuildFromVectorAngle34_ADDR, rdMatrix_BuildFromVectorAngle34);
    hook_function(rdMatrix_LookAt_ADDR, rdMatrix_LookAt);
    //hook_function(rdMatrix_ExtractAngles34_ADDR, rdMatrix_ExtractAngles34); // TODO fix
    hook_function(rdMatrix_Normalize34_ADDR, rdMatrix_Normalize34);
    hook_function(rdMatrix_Identity34_ADDR, rdMatrix_Identity34);
    hook_function(rdMatrix_Identity44_ADDR, rdMatrix_Identity44);
    hook_function(rdMatrix_Copy34_ADDR, rdMatrix_Copy34);
    hook_function(rdMatrix_Copy44_ADDR, rdMatrix_Copy44);
    hook_function(rdMatrix_Copy34to44_ADDR, rdMatrix_Copy34to44);
    hook_function(rdMatrix_Copy44to34_ADDR, rdMatrix_Copy44to34);
    hook_function(rdMatrix_Transpose44_ADDR, rdMatrix_Transpose44);
    hook_function(rdMatrix_Multiply34_ADDR, rdMatrix_Multiply34);
    hook_function(rdMatrix_Multiply44_ADDR, rdMatrix_Multiply44);
    hook_function(rdMatrix_PreMultiply34_ADDR, rdMatrix_PreMultiply34);
    hook_function(rdMatrix_PreMultiply44_ADDR, rdMatrix_PreMultiply44);
    hook_function(rdMatrix_PostMultiply34_ADDR, rdMatrix_PostMultiply34);
    hook_function(rdMatrix_PostMultiply44_ADDR, rdMatrix_PostMultiply44);
    hook_function(rdMatrix_PreRotate34_ADDR, rdMatrix_PreRotate34);
    hook_function(rdMatrix_PreRotate44_ADDR, rdMatrix_PreRotate44);
    hook_function(rdMatrix_PostRotate34_ADDR, rdMatrix_PostRotate34);
    hook_function(rdMatrix_PostRotate44_ADDR, rdMatrix_PostRotate44);
    hook_function(rdMatrix_PreTranslate34_ADDR, rdMatrix_PreTranslate34);
    hook_function(rdMatrix_PreTranslate44_ADDR, rdMatrix_PreTranslate44);
    hook_function(rdMatrix_PostTranslate34_ADDR, rdMatrix_PostTranslate34);
    hook_function(rdMatrix_PostTranslate44_ADDR, rdMatrix_PostTranslate44);
    hook_function(rdMatrix_PreScale34_ADDR, rdMatrix_PreScale34);
    hook_function(rdMatrix_PreScale44_ADDR, rdMatrix_PreScale44);
    hook_function(rdMatrix_PostScale34_ADDR, rdMatrix_PostScale34);
    hook_function(rdMatrix_PostScale44_ADDR, rdMatrix_PostScale44);
    hook_function(rdMatrix_SetRowVector34_ADDR, rdMatrix_SetRowVector34);
    hook_function(rdMatrix_SetRowVector44_ADDR, rdMatrix_SetRowVector44);
    hook_function(rdMatrix_GetRowVector34_ADDR, rdMatrix_GetRowVector34);
    hook_function(rdMatrix_GetRowVector44_ADDR, rdMatrix_GetRowVector44);
    hook_function(rdMatrix_TransformVector34_ADDR, rdMatrix_TransformVector34);
    hook_function(rdMatrix_TransformVector34Acc_0_ADDR, rdMatrix_TransformVector34Acc_0);
    hook_function(rdMatrix_TransformVector34Acc_ADDR, rdMatrix_TransformVector34Acc);
    hook_function(rdMatrix_TransformVector44_ADDR, rdMatrix_TransformVector44);
    hook_function(rdMatrix_TransformVector44Acc_ADDR, rdMatrix_TransformVector44Acc);
    hook_function(rdMatrix_TransformPoint34_ADDR, rdMatrix_TransformPoint34);
    hook_function(rdMatrix_TransformPoint34Acc_ADDR, rdMatrix_TransformPoint34Acc);
    hook_function(rdMatrix_TransformPoint44_ADDR, rdMatrix_TransformPoint44);
    hook_function(rdMatrix_TransformPoint44Acc_ADDR, rdMatrix_TransformPoint44Acc);
    hook_function(rdMatrix_TransformPointLst34_ADDR, rdMatrix_TransformPointLst34);
    hook_function(rdMatrix_TransformPointLst44_ADDR, rdMatrix_TransformPointLst44);
    
    // sithWeapon
    hook_function(sithWeapon_InitDefaults_ADDR, sithWeapon_InitDefaults);
    hook_function(sithWeapon_InitDefaults2_ADDR, sithWeapon_InitDefaults2);

    // sithTime
    hook_function(sithTime_Tick_ADDR, sithTime_Tick);
    hook_function(sithTime_Pause_ADDR, sithTime_Pause);
    hook_function(sithTime_Resume_ADDR, sithTime_Resume);
    hook_function(sithTime_SetDelta_ADDR, sithTime_SetDelta);
    hook_function(sithTime_Startup_ADDR, sithTime_Startup);
    hook_function(sithTime_SetMs_ADDR, sithTime_SetMs);
    
    // sithModel
    hook_function(sithModel_Startup_ADDR, sithModel_Startup);
    hook_function(sithModel_Shutdown_ADDR, sithModel_Shutdown);
    hook_function(sithModel_Load_ADDR, sithModel_Load);
    hook_function(sithModel_Free_ADDR, sithModel_Free);
    hook_function(sithModel_LoadEntry_ADDR, sithModel_LoadEntry);
    hook_function(sithModel_GetMemorySize_ADDR, sithModel_GetMemorySize);
    hook_function(sithModel_New_ADDR, sithModel_New);
    hook_function(sithModel_GetByIdx_ADDR, sithModel_GetByIdx);
    
    // sithWorld
    hook_function(sithWorld_Startup_ADDR, sithWorld_Startup);
    hook_function(sithWorld_Shutdown_ADDR, sithWorld_Shutdown);
    hook_function(sithWorld_SetLoadPercentCallback_ADDR, sithWorld_SetLoadPercentCallback);
    hook_function(sithWorld_UpdateLoadPercent_ADDR, sithWorld_UpdateLoadPercent);
    hook_function(sithWorld_SetSectionParser_ADDR, sithWorld_SetSectionParser);
    hook_function(sithWorld_FindSectionParser_ADDR, sithWorld_FindSectionParser);

    // sithInventory
    hook_function(sithInventory_NewEntry_ADDR, sithInventory_NewEntry);
    hook_function(sithInventory_GetNumBinsWithFlag_ADDR, sithInventory_GetNumBinsWithFlag);
    hook_function(sithInventory_GetNumBinsWithFlagRev_ADDR, sithInventory_GetNumBinsWithFlagRev);
    hook_function(sithInventory_GetNumItemsPriorToIdx_ADDR, sithInventory_GetNumItemsPriorToIdx);
    hook_function(sithInventory_GetNumItemsFollowingIdx_ADDR, sithInventory_GetNumItemsFollowingIdx);
    hook_function(sithInventory_SelectItem_ADDR, sithInventory_SelectItem);
    hook_function(sithInventory_SelectItemPrior_ADDR, sithInventory_SelectItemPrior);
    hook_function(sithInventory_SelectItemFollowing_ADDR, sithInventory_SelectItemFollowing);
    hook_function(sithInventory_SelectWeaponFollowing_ADDR, sithInventory_SelectWeaponFollowing);
    hook_function(sithInventory_GetBinByIdx_ADDR, sithInventory_GetBinByIdx);
    hook_function(sithInventory_GetCurWeapon_ADDR, sithInventory_GetCurWeapon);
    hook_function(sithInventory_SetCurWeapon_ADDR, sithInventory_SetCurWeapon);
    hook_function(sithInventory_GetCurItem_ADDR, sithInventory_GetCurItem);
    hook_function(sithInventory_SetCurItem_ADDR, sithInventory_SetCurItem);
    hook_function(sithInventory_GetCurPower_ADDR, sithInventory_GetCurPower);
    hook_function(sithInventory_SetCurPower_ADDR, sithInventory_SetCurPower);
    hook_function(sithInventory_GetWeaponPrior_ADDR, sithInventory_GetWeaponPrior);
    hook_function(sithInventory_GetWeaponFollowing_ADDR, sithInventory_GetWeaponFollowing);
    hook_function(sithInventory_GetPowerPrior_ADDR, sithInventory_GetPowerPrior);
    hook_function(sithInventory_GetPowerFollowing_ADDR, sithInventory_GetPowerFollowing);
    hook_function(sithInventory_SelectPower_ADDR, sithInventory_SelectPower);
    hook_function(sithInventory_SelectPowerPrior_ADDR, sithInventory_SelectPowerPrior);
    hook_function(sithInventory_SelectPowerFollowing_ADDR, sithInventory_SelectPowerFollowing);
    hook_function(sithInventory_ActivateBin_ADDR, sithInventory_ActivateBin);
    hook_function(sithInventory_DeactivateBin_ADDR, sithInventory_DeactivateBin);
    hook_function(sithInventory_BinSendActivate_ADDR, sithInventory_BinSendActivate);
    hook_function(sithInventory_BinSendDeactivate_ADDR, sithInventory_BinSendDeactivate);
    hook_function(sithInventory_ChangeInv_ADDR, sithInventory_ChangeInv);
    hook_function(sithInventory_GetBinAmount_ADDR, sithInventory_GetBinAmount);
    hook_function(sithInventory_SetBinAmount_ADDR, sithInventory_SetBinAmount);
    hook_function(sithInventory_SetActivate_ADDR, sithInventory_SetActivate);
    hook_function(sithInventory_GetActivate_ADDR, sithInventory_GetActivate);
    hook_function(sithInventory_SetAvailable_ADDR, sithInventory_SetAvailable);
    hook_function(sithInventory_GetAvailable_ADDR, sithInventory_GetAvailable);
    hook_function(sithInventory_SetCarries_ADDR, sithInventory_SetCarries);
    hook_function(sithInventory_GetCarries_ADDR, sithInventory_GetCarries);
    hook_function(sithInventory_IsBackpackable_ADDR, sithInventory_IsBackpackable);
    hook_function(sithInventory_SerializedWrite_ADDR, sithInventory_SerializedWrite);
    hook_function(sithInventory_GetMin_ADDR, sithInventory_GetMin);
    hook_function(sithInventory_GetMax_ADDR, sithInventory_GetMax);
    hook_function(sithInventory_SetFlags_ADDR, sithInventory_SetFlags);
    hook_function(sithInventory_GetFlags_ADDR, sithInventory_GetFlags);
    hook_function(sithInventory_UnsetFlags_ADDR, sithInventory_UnsetFlags);
    hook_function(sithInventory_SendMessageToAllWithState_ADDR, sithInventory_SendMessageToAllWithState);
    hook_function(sithInventory_SendMessageToAllWithFlag_ADDR, sithInventory_SendMessageToAllWithFlag);
    hook_function(sithInventory_ClearUncarried_ADDR, sithInventory_ClearUncarried);
    hook_function(sithInventory_CreateBackpack_ADDR, sithInventory_CreateBackpack);
    hook_function(sithInventory_PickupBackpack_ADDR, sithInventory_PickupBackpack);
    hook_function(sithInventory_NthBackpackBin_ADDR, sithInventory_NthBackpackBin);
    hook_function(sithInventory_NthBackpackValue_ADDR, sithInventory_NthBackpackValue);
    hook_function(sithInventory_NumBackpackItems_ADDR, sithInventory_NumBackpackItems);
    // handleinvskillkeys
    hook_function(sithInventory_SendFire_ADDR, sithInventory_SendFire);
    hook_function(sithInventory_GetBin_ADDR, sithInventory_GetBin);
    hook_function(sithInventory_GetItemDesc_ADDR, sithInventory_GetItemDesc);
    hook_function(sithInventory_ClearInventory_ADDR, sithInventory_ClearInventory);
    hook_function(sithInventory_SendKilledMessageToAll_ADDR, sithInventory_SendKilledMessageToAll);
    hook_function(sithInventory_SetBinWait_ADDR, sithInventory_SetBinWait);

    // sithTemplate
    hook_function(sithTemplate_Startup_ADDR, sithTemplate_Startup);
    hook_function(sithTemplate_Shutdown_ADDR, sithTemplate_Shutdown);
    hook_function(sithTemplate_New_ADDR, sithTemplate_New);
    hook_function(sithTemplate_GetEntryByIdx_ADDR, sithTemplate_GetEntryByIdx);
    hook_function(sithTemplate_Load_ADDR, sithTemplate_Load);
    hook_function(sithTemplate_OldNew_ADDR, sithTemplate_OldNew);
    hook_function(sithTemplate_OldFree_ADDR, sithTemplate_OldFree);
    hook_function(sithTemplate_FreeWorld_ADDR, sithTemplate_FreeWorld);
    hook_function(sithTemplate_GetEntryByName_ADDR, sithTemplate_GetEntryByName);
    hook_function(sithTemplate_CreateEntry_ADDR, sithTemplate_CreateEntry);
    
    // jkPlayer
    hook_function(jkPlayer_renderSaberWeaponMesh_ADDR, jkPlayer_renderSaberWeaponMesh);
    
    // jkSaber
    hook_function(jkSaber_InitializeSaberInfo_ADDR, jkSaber_InitializeSaberInfo);
    hook_function(jkSaber_PolylineRand_ADDR, jkSaber_PolylineRand);
    hook_function(jkSaber_Draw_ADDR, jkSaber_Draw);
    hook_function(jkSaber_UpdateLength_ADDR, jkSaber_UpdateLength);
    hook_function(jkSaber_UpdateCollision_ADDR, jkSaber_UpdateCollision);
    
    // jkGob
    hook_function(jkGob_Startup_ADDR, jkGob_Startup);
    hook_function(jkGob_Shutdown_ADDR, jkGob_Shutdown);
    
    // jkStrings
    hook_function(jkStrings_Initialize_ADDR, jkStrings_Initialize);
    hook_function(jkStrings_Shutdown_ADDR, jkStrings_Shutdown);
    hook_function(jkStrings_GetText2_ADDR, jkStrings_GetText2);
    hook_function(jkStrings_GetText_ADDR, jkStrings_GetText);
    hook_function(jkStrings_unused_sub_40B490_ADDR, jkStrings_unused_sub_40B490);
    
    // sithUnk3
    hook_function(sithUnk3_Startup_ADDR, sithUnk3_Startup);
    hook_function(sithUnk3_RegisterCollisionHandler_ADDR, sithUnk3_RegisterCollisionHandler);
    hook_function(sithUnk3_NextSearchResult_ADDR, sithUnk3_NextSearchResult);
    
    // sithItem
    hook_function(sithItem_Collide_ADDR, sithItem_Collide);
    hook_function(sithItem_New_ADDR, sithItem_New);
    hook_function(sithItem_Take_ADDR, sithItem_Take);
    hook_function(sithItem_Remove_ADDR, sithItem_Remove);
    hook_function(sithItem_LoadThingParams_ADDR, sithItem_LoadThingParams);
    hook_function(sithItem_LoadThingParams_ADDR, sithItem_LoadThingParams);
    
    // sithTimer
    hook_function(sithTimer_Startup_ADDR, sithTimer_Startup);
    hook_function(sithTimer_Shutdown_ADDR, sithTimer_Shutdown);
    hook_function(sithTimer_Open_ADDR, sithTimer_Open);
    hook_function(sithTimer_Close_ADDR, sithTimer_Close);
    hook_function(sithTimer_Reset_ADDR, sithTimer_Reset);
    hook_function(sithTimer_Set_ADDR, sithTimer_Set);
    hook_function(sithTimer_Kill_ADDR, sithTimer_Kill);
    hook_function(sithTimer_RegisterFunc_ADDR, sithTimer_RegisterFunc);
    hook_function(sithTimer_Advance_ADDR, sithTimer_Advance);
    
    // sithKeyFrame
    hook_function(sithKeyFrame_Load_ADDR, sithKeyFrame_Load);
    hook_function(sithKeyFrame_GetByIdx_ADDR, sithKeyFrame_GetByIdx);
    hook_function(sithKeyFrame_LoadEntry_ADDR, sithKeyFrame_LoadEntry);
    hook_function(sithKeyFrame_New_ADDR, sithKeyFrame_New);
    hook_function(sithKeyFrame_Free_ADDR, sithKeyFrame_Free);
    
    // sithSprite
    hook_function(sithSprite_Startup_ADDR, sithSprite_Startup);
    hook_function(sithSprite_Shutdown_ADDR, sithSprite_Shutdown);
    hook_function(sithSprite_Load_ADDR, sithSprite_Load);
    hook_function(sithSprite_FreeEntry_ADDR, sithSprite_FreeEntry);
    hook_function(sithSprite_LoadEntry_ADDR, sithSprite_LoadEntry);
    hook_function(sithSprite_New_ADDR, sithSprite_New);
    
    // sithAI
    hook_function(sithAI_RegisterCommand_ADDR, sithAI_RegisterCommand);
    hook_function(sithAI_FindCommand_ADDR, sithAI_FindCommand);

    // sithAIClass
    
    // sithAICmd
    hook_function(sithAICmd_Startup_ADDR, sithAICmd_Startup);

    // jkGUIRend
    hook_function(jkGuiRend_CopyVBuffer_ADDR, jkGuiRend_CopyVBuffer);
    hook_function(jkGuiRend_SetPalette_ADDR, jkGuiRend_SetPalette);
    hook_function(jkGuiRend_DrawRect_ADDR, jkGuiRend_DrawRect);
    hook_function(jkGuiRend_UpdateDrawMenu_ADDR, jkGuiRend_UpdateDrawMenu);
    hook_function(jkGuiRend_Paint_ADDR, jkGuiRend_Paint);
    hook_function(jkGuiRend_SetElementIdk_ADDR, jkGuiRend_SetElementIdk);
    hook_function(jkGuiRend_MenuSetLastElement_ADDR, jkGuiRend_MenuSetLastElement);
    hook_function(jkGuiRend_SetDisplayingStruct_ADDR, jkGuiRend_SetDisplayingStruct);
    hook_function(jkGuiRend_DisplayAndReturnClicked_ADDR, jkGuiRend_DisplayAndReturnClicked);
    hook_function(jkGuiRend_sub_50FAD0_ADDR, jkGuiRend_sub_50FAD0);
    hook_function(jkGuiRend_gui_sets_handler_framebufs_ADDR, jkGuiRend_gui_sets_handler_framebufs);
    hook_function(jkGuiRend_Menuidk_ADDR, jkGuiRend_Menuidk);
    hook_function(jkGuiRend_sub_50FDB0_ADDR, jkGuiRend_sub_50FDB0);
    hook_function(jkGuiRend_Initialize_ADDR, jkGuiRend_Initialize);
    hook_function(jkGuiRend_Shutdown_ADDR, jkGuiRend_Shutdown);
    hook_function(jkGuiRend_Open_ADDR, jkGuiRend_Open);
    hook_function(jkGuiRend_Close_ADDR, jkGuiRend_Close);
    hook_function(jkGuiRend_MenuGetClickableById_ADDR, jkGuiRend_MenuGetClickableById);
    //hook_function();
    hook_function(jkGuiRend_SetCursorVisible_ADDR, jkGuiRend_SetCursorVisible);
    hook_function(jkGuiRend_UpdateCursor_ADDR, jkGuiRend_UpdateCursor);
    hook_function(jkGuiRend_UpdateSurface_ADDR, jkGuiRend_UpdateSurface);
    hook_function(jkGuiRend_DrawAndFlip_ADDR, jkGuiRend_DrawAndFlip);
    hook_function(jkGuiRend_Invalidate_ADDR, jkGuiRend_Invalidate);
    hook_function(jkGuiRend_DarrayNewStr_ADDR, jkGuiRend_DarrayNewStr);
    hook_function(jkGuiRend_DarrayReallocStr_ADDR, jkGuiRend_DarrayReallocStr);
    hook_function(jkGuiRend_AddStringEntry_ADDR, jkGuiRend_AddStringEntry);
    hook_function(jkGuiRend_SetClickableString_ADDR, jkGuiRend_SetClickableString);
    hook_function(jkGuiRend_GetString_ADDR, jkGuiRend_GetString);
    hook_function(jkGuiRend_GetId_ADDR, jkGuiRend_GetId);
    hook_function(jkGuiRend_GetStringEntry_ADDR, jkGuiRend_GetStringEntry);
    hook_function(jkGuiRend_DarrayFree_ADDR, jkGuiRend_DarrayFree);
    hook_function(jkGuiRend_DarrayFreeEntry_ADDR, jkGuiRend_DarrayFreeEntry);
    hook_function(jkGuiRend_sub_5103E0_ADDR, jkGuiRend_sub_5103E0);
    hook_function(jkGuiRend_ElementHasHoverSound_ADDR, jkGuiRend_ElementHasHoverSound);
    hook_function(jkGuiRend_UpdateAndDrawClickable_ADDR, jkGuiRend_UpdateAndDrawClickable);
    hook_function(jkGuiRend_InvokeButtonDown_ADDR, jkGuiRend_InvokeButtonDown);
    hook_function(jkGuiRend_InvokeButtonUp_ADDR, jkGuiRend_InvokeButtonUp);
    hook_function(jkGuiRend_PlayClickSound_ADDR, jkGuiRend_PlayClickSound);
    hook_function(jkGuiRend_RenderFocused_ADDR, jkGuiRend_RenderFocused);
    hook_function(jkGuiRend_RenderIdk2_ADDR, jkGuiRend_RenderIdk2);
    hook_function(jkGuiRend_RenderAll_ADDR, jkGuiRend_RenderAll);
    hook_function(jkGuiRend_ClickableMouseover_ADDR, jkGuiRend_ClickableMouseover);
    hook_function(jkGuiRend_MouseMovedCallback_ADDR, jkGuiRend_MouseMovedCallback);
    hook_function(jkGuiRend_SetVisibleAndDraw_ADDR, jkGuiRend_SetVisibleAndDraw);
    hook_function(jkGuiRend_ClickableHover_ADDR, jkGuiRend_ClickableHover);
    hook_function(jkGuiRend_sub_510C60_ADDR, jkGuiRend_sub_510C60);
    hook_function(jkGuiRend_ClickSound_ADDR, jkGuiRend_ClickSound);
    hook_function(jkGuiRend_HoverOn_ADDR, jkGuiRend_HoverOn);
    hook_function(jkGuiRend_ListBoxButtonDown_ADDR, jkGuiRend_ListBoxButtonDown);
    hook_function(jkGuiRend_ListBoxDraw_ADDR, jkGuiRend_ListBoxDraw);
    hook_function(jkGuiRend_CheckBoxDraw_ADDR, jkGuiRend_CheckBoxDraw);
    hook_function(jkGuiRend_DrawClickableAndUpdatebool_ADDR, jkGuiRend_DrawClickableAndUpdatebool);
    hook_function(jkGuiRend_WindowHandler_ADDR, jkGuiRend_WindowHandler);
    hook_function(jkGuiRend_UpdateMouse_ADDR, jkGuiRend_UpdateMouse);
    hook_function(jkGuiRend_FlipAndDraw_ADDR, jkGuiRend_FlipAndDraw);
    hook_function(jkGuiRend_GetMousePos_ADDR, jkGuiRend_GetMousePos);
    hook_function(jkGuiRend_ResetMouseLatestMs_ADDR, jkGuiRend_ResetMouseLatestMs);
    hook_function(jkGuiRend_InvalidateGdi_ADDR, jkGuiRend_InvalidateGdi);
    hook_function(jkGuiRend_SliderButtonDown_ADDR, jkGuiRend_SliderButtonDown);
    hook_function(jkGuiRend_SliderDraw_ADDR, jkGuiRend_SliderDraw);
    hook_function(jkGuiRend_TextBoxButtonDown_ADDR, jkGuiRend_TextBoxButtonDown);
    hook_function(jkGuiRend_TextBoxDraw_ADDR, jkGuiRend_TextBoxDraw);
    hook_function(jkGuiRend_TextDraw_ADDR, jkGuiRend_TextDraw);
    hook_function(jkGuiRend_PicButtonButtonDown_ADDR, jkGuiRend_PicButtonButtonDown);
    hook_function(jkGuiRend_PicButtonDraw_ADDR, jkGuiRend_PicButtonDraw);
    hook_function(jkGuiRend_TextButtonButtonDown_ADDR, jkGuiRend_TextButtonButtonDown);
    hook_function(jkGuiRend_TextButtonDraw_ADDR, jkGuiRend_TextButtonDraw);
    
    // jkGUI
    hook_function(jkGui_InitMenu_ADDR, jkGui_InitMenu);
    hook_function(jkGui_MessageBeep_ADDR, jkGui_MessageBeep);
    hook_function(jkGui_Initialize_ADDR, jkGui_Initialize);
    hook_function(jkGui_Shutdown_ADDR, jkGui_Shutdown);
    hook_function(jkGui_SetModeMenu_ADDR, jkGui_SetModeMenu);
    hook_function(jkGui_SetModeGame_ADDR, jkGui_SetModeGame);
    hook_function(jkGui_sub_412E20_ADDR, jkGui_sub_412E20);
    hook_function(jkGui_copies_string_ADDR, jkGui_copies_string);
    hook_function(jkGui_sub_412EC0_ADDR, jkGui_sub_412EC0);
    hook_function(jkGui_sub_412ED0_ADDR, jkGui_sub_412ED0);
    
    // jkGUIForce
    hook_function(jkGuiForce_ChoiceRemoveStar_ADDR, jkGuiForce_ChoiceRemoveStar);
    hook_function(jkGuiForce_ChoiceRemoveStars_ADDR, jkGuiForce_ChoiceRemoveStars);
    hook_function(jkGuiForce_ForceStarsDraw_ADDR, jkGuiForce_ForceStarsDraw);
    hook_function(jkGuiForce_ExtraClick_ADDR, jkGuiForce_ExtraClick);
    hook_function(jkGuiForce_ButtonClick_ADDR, jkGuiForce_ButtonClick);
    hook_function(jkGuiForce_ButtonClick_ADDR, jkGuiForce_ButtonClick);
    hook_function(jkGuiForce_ResetClick_ADDR, jkGuiForce_ResetClick);
    hook_function(jkGuiForce_Show_ADDR, jkGuiForce_Show);
    hook_function(jkGuiForce_Initialize_ADDR, jkGuiForce_Initialize);
    hook_function(jkGuiForce_Shutdown_ADDR, jkGuiForce_Shutdown);
    hook_function(jkGuiForce_UpdateViewForRank_ADDR, jkGuiForce_UpdateViewForRank);
    hook_function(jkGuiForce_DarkLightHoverDraw_ADDR, jkGuiForce_DarkLightHoverDraw);
    
    // jkGUIEsc
    hook_function(jkGuiEsc_Startup_ADDR, jkGuiEsc_Startup);
    hook_function(jkGuiEsc_Shutdown_ADDR, jkGuiEsc_Shutdown);
    hook_function(jkGuiEsc_Show_ADDR, jkGuiEsc_Show);
    
    // Darray
    hook_function(Darray_New_ADDR, Darray_New);
    hook_function(Darray_Free_ADDR, Darray_Free);
    hook_function(Darray_NewEntry_ADDR, Darray_NewEntry);
    hook_function(Darray_GetIndex_ADDR, Darray_GetIndex);
    hook_function(Darray_ClearAll_ADDR, Darray_ClearAll);
    hook_function(Darray_sub_520CB0_ADDR, Darray_sub_520CB0);

    // test saber time
    //*(float*)0x5220C4 = 0.01f;

    
    
    //hook_function();
}
