// MQ2LootManager.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.



#include "../MQ2Plugin.h"
#include "MQ2LootManagerFunctions.h"

bool bPaused = false;
PreSetup("MQ2LootManager");

void LootMgrCmd(PSPAWNINFO pSpawn, char* szLine) {
    char szArg[MAX_STRING] = { 0 };
    GetArg(szArg, szLine, 1);
    if (!_stricmp(szArg, "pause")) {
        GetArg(szArg, szLine, 2);
        if (!strlen(szArg)) {
            bPaused = !bPaused;
            WriteChatf("%s", (bPaused ? "Paused" : "Unpaused"));
        }
        else if (!_stricmp(szArg, "on")) {
            bPaused = true;
            WriteChatf("Paused");
        }
        else if (!_stricmp(szArg, "off")) {
            bPaused = false;
            WriteChatf("Unpaused");
        }
    }
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    AddCommand("/lootmanager", LootMgrCmd);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    RemoveCommand("/lootmanager");
}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
    {//only pulse every 500 ticks. (every half second? Maybe too fast.)
        static uint64_t PulseTimer = 0;
        if (PulseTimer > GetTickCount64())
            return;

        PulseTimer = GetTickCount64() + 500;
    }

    //If I'm InGame()
    if (GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2()) {
        static bool IAmML = false;
        if (IAmMasterLooter()) {
            if (!IAmML) {
                WriteChatf("I should collect the loot.");
                IAmML = true;
            }
        }
        else if (IAmML) {
            WriteChatf("No longer collecting the loot.");
            IAmML = false;
        }

        if (IAmML && pAdvancedLootWnd) {//If I'm the master looter and AutoLootAllIsOn.
            if (PEQADVLOOTWND pAdvLoot = (PEQADVLOOTWND)pAdvancedLootWnd) {
                CListWnd* pPersonalList;
                CListWnd* pSharedList;
                if (!pAdvLoot->pPLootList || !pAdvLoot->pPLootList->PersonalLootList) {
                    WriteChatf("Avoid Nullptr");
                }
                else {
                    pPersonalList = (CListWnd*)pAdvancedLootWnd->GetChildItem("ADLW_PLLList");
                }

                if (!pAdvLoot->pCLootList || !pAdvLoot->pCLootList->SharedLootList) {
                    WriteChatf("Avoid Nullptr2");
                }
                else {
                    pSharedList = (CListWnd*)pAdvLoot->pCLootList->SharedLootList;
                }


                PCHARINFO pChar = GetCharInfo();
                if (!pChar)
                    return;

                PCHARINFO2 pChar2 = GetCharInfo2();
                if (!pChar2)
                    return;

                //Can't Loot if already Looting.
                if (LootInProgress(pAdvLoot, pPersonalList, pSharedList)) {
                    return;
                }
                //Do Looting shit here. Personal list first, shared list next.
                if (pPersonalList) {
                    if (HandlePersonalLoot(pChar, pChar2, pAdvLoot, pPersonalList, pSharedList)) {
                        return;
                    }
                }
                if (pSharedList) {
                    if (HandleSharedLoot(pChar, pChar2, pAdvLoot, pPersonalList, pSharedList)) {
                        return;
                    }
                }
            }
        }
    }//InGame()
}