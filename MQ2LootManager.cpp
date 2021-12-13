// MQ2LootManager.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.



#include <mq/Plugin.h>
#include "MQ2LootManagerFunctions.h"

bool bPaused = false;
PreSetup("MQ2LootManager");
PLUGIN_VERSION(1.0);



void CheckForLoreEquipped() {
	if (CXWnd* ConfirmationWnd = FindMQ2Window("ConfirmationDialogBox")) {
		if (ConfirmationWnd->IsVisible()) {
			if (CStmlWnd* TextWnd = (CStmlWnd*)ConfirmationWnd->GetChildItem("CD_TextOutput")) {
				if (TextWnd->GetSTMLText().find("group with an item you already possess") != CXStr::npos) {
					if (CButtonWnd* YesBttn = (CButtonWnd*)ConfirmationWnd->GetChildItem("CD_Yes_Button")) {
						SendWndClick2(YesBttn, "leftmouseup");
					}
				}
			}
		}
	}
}

void LootMgrCmd(PSPAWNINFO pSpawn, char* szLine) {
    char szArg[MAX_STRING] = { 0 };
    GetArg(szArg, szLine, 1);
    if (!_stricmp(szArg, "pause")) {
        GetArg(szArg, szLine, 2);
        if (!strlen(szArg)) {
            bPaused = !bPaused;
            WriteChatf("%s%s", PluginMsg.c_str(), (bPaused ? "Paused" : "Unpaused"));
        }
        else if (!_stricmp(szArg, "on")) {
            bPaused = true;
            WriteChatf("%sPaused", PluginMsg.c_str());
        }
        else if (!_stricmp(szArg, "off")) {
            bPaused = false;
            WriteChatf("%sUnpaused", PluginMsg.c_str());
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
    if (bPaused)//if the plugin is paused, just wait.
        return;

    {//only pulse every 500 ticks. (every half second? Maybe too fast.)
        static uint64_t PulseTimer = 0;
        if (PulseTimer > GetTickCount64())
            return;

        PulseTimer = GetTickCount64() + 500;
    }

    //If I'm InGame()
    if (GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetPcProfile()) {
        //Default to assume if we don't have any giant slots left, we can't carry anything.
        //TODO: Make this smarter, based on the item we plan to pickup to see if we can hold that item.
        static bool OutOfSpace = false;
        if (!GetFreeInventory(Giant)) {
            if (!OutOfSpace) {
                WriteChatf("%s\arOut of Space!! Clean up your bags you slob!");
                OutOfSpace = true;
            }
            return;
        }
        else if (OutOfSpace) {
            WriteChatf("We got space again, resuming looting.");
            OutOfSpace = false;
        }

        static bool IAmML = false;
        if (IAmMasterLooter()) {
            if (!IAmML) {
                WriteChatf("%sI should collect the loot.", PluginMsg.c_str());
                IAmML = true;
            }
        }
        else if (IAmML) {
            WriteChatf("%sNo longer collecting the loot.", PluginMsg.c_str());
            IAmML = false;
        }

        if (pAdvancedLootWnd) {//If I'm the master looter and AutoLootAllIsOn.
            PCHARINFO pChar = GetCharInfo();
            if (!pChar)
                return;

            PcProfile* pChar2 = GetPcProfile();
            if (!pChar2)
                return;

            CAdvancedLootWnd* pAdvLoot = (CAdvancedLootWnd*)pAdvancedLootWnd;
            if (!pAdvLoot)
                return;

            CListWnd* pPersonalList;
            CListWnd* pSharedList;
            if (!pAdvLoot->pPLootList || !pAdvLoot->pPLootList->PersonalLootList) {
                return;
            }
            else {
                //pPersonalList = (CListWnd*)pAdvLoot->pPLootList->PersonalLootList; This doesn't work, but using this method works for SharedLootList Something broken?
                pPersonalList = (CListWnd*)pAdvancedLootWnd->GetChildItem("ADLW_PLLList");
            }

            if (!pPersonalList)
                return;

            if (!pAdvLoot->pCLootList || !pAdvLoot->pCLootList->SharedLootList) {
                return;
            }
            else {
                pSharedList = (CListWnd*)pAdvLoot->pCLootList->SharedLootList;
            }

            if (!pSharedList)
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
                if (IAmML && HandleSharedLoot(pChar, pChar2, pAdvLoot, pPersonalList, pSharedList)) {
                    return;
                }
            }
        }

		//Handle "Lore-Equipped Message.
		/*Example
		* Penumbra Pauldrons shares a LORE_EQUIP group with an item you already possess, are you sure you wish to loot it?
		*
		* Found window is ConfirmationDialogBox
		* Sub Window for above example text is CD_TextOutput
		*/
		CheckForLoreEquipped();
    }//InGame()
}