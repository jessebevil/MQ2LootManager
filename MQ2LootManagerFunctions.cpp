#pragma once
#include "MQ2LootManagerFunctions.h"

int GroupTotal() {
	int n = 0;
	if (!GetCharInfo()->pGroupInfo)
		return 0;

	for (int i = 1; i < 6; i++) {
		if (CGroupMember* pMember = GetCharInfo()->Group->GetGroupMember(i)) {
			if (pMember->pSpawn)
				n += 1;
		}
	}

	if (n)//count me too!
		n++;

	return n;
}

bool IAmMasterLooter() {
	PCHARINFO pChar = GetCharInfo();
	if (pRaid && pRaid->RaidMemberCount) {
		for (int i = 0; i < 72; i++) {
			if (pRaid->RaidMemberUsed[i] && pRaid->RaidMember[i].MasterLooter) {
				if (!_stricmp(pChar->Name, pRaid->RaidMember[i].Name))
				{
					return true;
				}
			}
		}
	}
	else {
		int partymembers = GroupTotal();
		for (int i = 0; i < partymembers; i++) {
			if (CGroupMember* pMember = GetCharInfo()->Group->GetGroupMember(i)) {
				if (pMember->MasterLooter) {
					if (!_stricmp(pChar->Name, pMember->Name.c_str())) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool WinState(CXWnd* pWnd) {
	return (pWnd && pWnd->IsVisible());
}

bool AutoLootAllIsOn() {

	if (CButtonWnd* pWndButton = (CButtonWnd*)FindMQ2Window("LootSettingsWnd")->GetChildItem("LS_AutoLootAllCheckbox")) {
		if (pWndButton->Checked) {
			return true;
		}
	}
	return false;
}

//For use with pAdvancedLootWnd->DoSharedAdvLootAction(PLOOTITEM, CXStr* Assignee, DWORD Action, DWORD Quantity)
enum LootAction : DWORD {
	Give,
	Leave
};

enum PersonalListOption : DWORD {
	Name,
	Item,
	Loot,
	eLeave,
	Never,
	AN,
	AG
};
// Handle personal loot
bool HandlePersonalLoot(PCHARINFO pChar, PcProfile* pChar2, CAdvancedLootWnd* pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList) {
	//Auto Loot All is on, so this will get handled by EQ.
	//If I'm the master looter, let's assume I'm watching that screen and not do anything.
	//If I don't have a group size, then manage my own personal list.
	if (AutoLootAllIsOn() || !GroupTotal() || IAmMasterLooter() ) {
		return false;
	}

	//If auto loot all isn't on and I'm not the Master Looter we should just collect anything going to the personal list while in a group and not the master looter.
	if (pAdvLoot->pPLootList) {
		for (long k = 0; k < pPersonalList->ItemsArray.Count; k++) {
			uint64_t listindex = pPersonalList->GetItemData(k);
			if (listindex != -1) {
				AdvancedLootItem* pPersonalItem = &pAdvLoot->pPLootList->Items[k];
				if (!pPersonalItem)
					return false;

				//Skip items with a setting, EQ will do this automatically.
				if (pPersonalItem->AlwaysNeed || pPersonalItem->AlwaysGreed)
					continue;

				//Will keep us from accidentally letting items rot. If it's made it to the personal list and I'm not the master looter, then I'm pretty sure I want to keep it.
				if (CXWnd* pwnd = GetAdvLootPersonalListItem((DWORD)listindex, Loot))
				{
					WriteChatf("%s\ayAccepting \ap%s\ax from personal list, because we didn't have a choice made and we didn't want it to rot.", PluginMsg.c_str(), pPersonalItem->Name);
					SendWndClick2(pwnd, "leftmouseup"); // Loot the item
					return true;
				}

			}
		}
	}

	return false;//Didn't handle any loot in here.
}

bool HandleSharedLoot(PCHARINFO pChar, PcProfile* pChar2, CAdvancedLootWnd* pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList) // Handle items in your shared loot window
{
	if (pSharedList)
	{
		if (!IAmMasterLooter()) // if it returns false, the plugin should stop doing loot stuff this pulse
		{
			return true;
		}

		//Plure and MQ2Commands.cpp do it this way. It's an odd way to handle it, but not sure what a better option would be provided the current state of things.
		for (int i = 0; i < pSharedList->ItemsArray.Count; i++) {
			uint64_t listindex = pSharedList->GetItemData(i);//returns uint64_t, but all uses of it here are DWORDs, so to avoid VS crying, I've casted all uses to DWORD lol.
			if (listindex != -1) {
				AdvancedLootItem* pSharedItem = &pAdvLoot->pCLootList->Items[(DWORD)listindex];
				if (!pSharedItem || pSharedItem->LootDetails.IsEmpty())
					continue;

				if (pAdvancedLootWnd) {
					if (!pSharedItem->bAutoRoll && pSharedItem->AskTimer > 0 || !pSharedItem->bAutoRoll) {//Let autoroll do autorolls.
						//Give the item to myself.
						if (pSharedItem->AlwaysNeed || pSharedItem->AlwaysGreed) {
							pAdvancedLootWnd->DoSharedAdvLootAction(*pSharedItem, pChar->Name, Give, pSharedItem->LootDetails[0].StackCount);
							WriteChatf("%s\agGiving \ap%s\ax to myself.", PluginMsg.c_str(), pSharedItem->Name);
							return true;
						}

						//Leave this on the corpse.
						if (pSharedItem->Never || pSharedItem->No) {
							pAdvancedLootWnd->DoSharedAdvLootAction(*pSharedItem, pChar->Name, Leave, pSharedItem->LootDetails[0].StackCount);
							WriteChatf("%s\arLeaving \ap%s\ax on the corpse.", PluginMsg.c_str(), pSharedItem->Name);
							return true;
						}
					}
				}
			}
		}
	}
	return false;//Didn't do any looting. So return false.
}