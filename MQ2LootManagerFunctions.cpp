#pragma once
#include "MQ2LootManagerFunctions.h"

int GroupTotal() {
	int n = 0;
	if (!GetCharInfo()->pGroupInfo)
		return 0;

	for (int i = 1; i < 6; i++) {
		if (GetCharInfo()->pGroupInfo->pMember[i])
			n++;
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
			if (pChar->pGroupInfo->pMember[i] && pChar->pGroupInfo->pMember[i]->MasterLooter) {
				if (pChar->pGroupInfo->pMember[i]->pName) {
					pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID;
					char Name[64] = { 0 };
					GetCXStr(pChar->pGroupInfo->pMember[i]->pName, Name, MAX_STRING);
					if (!_stricmp(pChar->Name, Name)) {
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


// Handle personal loot
bool HandlePersonalLoot(PCHARINFO pChar, PCHARINFO2 pChar2, PEQADVLOOTWND pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList) {
	//Auto Loot All is on, so this will get handled by EQ. But here's a snippet otherwise.
	if (AutoLootAllIsOn()) {
		return false;
	}

	//if (pAdvLoot->pPLootList) {
	//	for (long k = 0; k < pPersonalList->ItemsArray.Count; k++) {
	//		unsigned long long listindex = pPersonalList->GetItemData(k);
	//		if (listindex != -1) {
	//			unsigned long long multiplier = sizeof(LOOTITEM) * listindex;
	//			PLOOTITEM pPersonalItem = (PLOOTITEM)(((unsigned int)pAdvLoot->pPLootList->pLootItem) + multiplier);
	//			if (!pPersonalItem)
	//				return false;

	//			if (pPersonalItem->AlwaysNeed || pPersonalItem->AlwaysGreed) {
	//				pAdvancedLootWnd->DoSharedAdvLootAction(pPersonalItem, &CXStr(pChar->Name), Give, pPersonalItem->LootDetails.m_array[0].StackCount);//Give the item to myself?
	//				WriteChatf("%s\agGiving \ap%s\ax to myself.", PluginMsg.c_str(), pPersonalItem->Name);
	//				return true;
	//			}

	//			if (pPersonalItem->Never || pPersonalItem->No) {//Leave this on the corpse.
	//				pAdvancedLootWnd->DoSharedAdvLootAction(pPersonalItem, &CXStr(pChar->Name), Leave, pPersonalItem->LootDetails.m_array[0].StackCount);
	//				WriteChatf("%s\arLeaving \ap%s\ax on the corpse.", PluginMsg.c_str(), pPersonalItem->Name);
	//				return true;
	//			}
	//		}
	//	}
	//}

	return false;//Didn't handle any loot in here.
}

bool HandleSharedLoot(PCHARINFO pChar, PCHARINFO2 pChar2, PEQADVLOOTWND pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList) // Handle items in your shared loot window
{
	if (pSharedList)
	{
		bool bMasterLooter = false;
		if (!IAmMasterLooter()) // if it returns false, the plugin should stop doing loot stuff this pulse
		{
			return true;
		}

		//Plure and MQ2Commands.cpp do it this way. It's an odd way to handle it, but not sure what a better option would be provided the current state of things.
		for (int i = 0; i < pSharedList->ItemsArray.Count; i++) {
			unsigned long long listindex = pSharedList->GetItemData(i);
			if (listindex != -1) {
				unsigned long long multiplier = sizeof(LOOTITEM) * listindex;

				PLOOTITEM pSharedItem = (PLOOTITEM)(((unsigned int)pAdvLoot->pCLootList->pLootItem) + multiplier);
				if (!pSharedItem)
					continue;

				if (pAdvancedLootWnd && pSharedItem && pSharedItem->LootDetails.m_length > 0) {
					if (!pSharedItem->bAutoRoll && pSharedItem->AskTimer > 0 || !pSharedItem->bAutoRoll) {//might not give a shit about this bit.
						//Give the item to myself.
						if (pSharedItem->AlwaysNeed || pSharedItem->AlwaysGreed) {
							pAdvancedLootWnd->DoSharedAdvLootAction(pSharedItem, &CXStr(pChar->Name), Give, pSharedItem->LootDetails.m_array[0].StackCount);
							WriteChatf("%s\agGiving \ap%s\ax to myself.", PluginMsg.c_str(), pSharedItem->Name);
							return true;
						}

						//Leave this on the corpse.
						if (pSharedItem->Never || pSharedItem->No) {
							pAdvancedLootWnd->DoSharedAdvLootAction(pSharedItem, &CXStr(pChar->Name), Leave, pSharedItem->LootDetails.m_array[0].StackCount);
							WriteChatf("%s\arLeaving \ap%s\ax on the corpse.", PluginMsg.c_str(), pSharedItem->Name);
							return true;
						}
					}
				}
			}
		}
		//Only handles the first item in the list.
		//if (!pSharedList->ItemsArray.Count)
		//	return false;

		//LOOTLIST* pSharedList = pAdvLoot->pCLootList;
		//if (!pSharedList)
		//	return false;

		//PLOOTITEM pThisLootItem = pSharedList->pLootItem;
		//if (!pThisLootItem)
		//	return false;

		//if (pThisLootItem->AlwaysNeed || pThisLootItem->AlwaysGreed) {
		//	pAdvancedLootWnd->DoSharedAdvLootAction(pThisLootItem, &CXStr(pChar->Name), Give, pThisLootItem->LootDetails.m_array[0].StackCount);//Give the item to myself?
		//	WriteChatf("%s\agGiving \ap%s\ax to myself.", PluginMsg.c_str(), pThisLootItem->Name);
		//	return true;
		//}

		//if (pThisLootItem->Never || pThisLootItem->No) {//Leave this on the corpse.
		//	pAdvancedLootWnd->DoSharedAdvLootAction(pThisLootItem, &CXStr(pChar->Name), Leave, pThisLootItem->LootDetails.m_array[0].StackCount);
		//	WriteChatf("%s\arLeaving \ap%s\ax on the corpse.", PluginMsg.c_str(), pThisLootItem->Name);
		//	return true;
		//}
	}
	return false;//Didn't do any looting. So return false.
}