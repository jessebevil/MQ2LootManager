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

// Handle personal loot
bool HandlePersonalLoot(PCHARINFO pChar, PCHARINFO2 pChar2, PEQADVLOOTWND pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList) {
	//Auto Loot All is on, so this will get handled by EQ I assume? But here's a snippet otherwise.
	if (!AutoLootAllIsOn()) {
		return false;
	}

	//if (pAdvLoot->pPLootList)
	//{
	//	for (long k = 0; k < pPersonalList->ItemsArray.Count; k++)
	//	{
	//		unsigned long long listindex = pPersonalList->GetItemData(k);
	//		if (listindex != -1)
	//		{
	//			DWORD multiplier = sizeof(LOOTITEM) * listindex;
	//			if (PLOOTITEM pPersonalItem = (PLOOTITEM)(((DWORD)pAdvLoot->pPLootList->pLootItem) + multiplier))
	//			{

	return false;//Didn't handle any loot in here.
}

enum SharedLootType : DWORD {
	NPC_Name,
	Item,
	Status,
	Action,
	Manage,
	AlwaysNeed,
	AlwaysGreed,
	AutoRoll,
	Never,
	Need,
	Greed,
	No,
	rootItem//root?
};

bool HandleSharedLoot(PCHARINFO pChar, PCHARINFO2 pChar2, PEQADVLOOTWND pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList) // Handle items in your shared loot window
{
	if (pSharedList)
	{
		bool bMasterLooter = false;
		if (!IAmMasterLooter()) // if it returns false, the plugin should stop doing loot stuff this pulse
		{
			return true;
		}

		for (int i = 0; i < pSharedList->ItemsArray.Count; i++) {
			unsigned long long listindex = pSharedList->GetItemData(i);
			if (listindex != -1) {
				unsigned long long multiplier = sizeof(LOOTITEM) * listindex;

				PLOOTITEM pSharedItem = (PLOOTITEM((unsigned int)pAdvLoot->pCLootList->pLootItem) + multiplier);
				if (!pSharedItem)
					continue;

				if (pAdvancedLootWnd && pSharedItem && pSharedItem->LootDetails.m_length > 0) {
					if (!pSharedItem->bAutoRoll && pSharedItem->AskTimer > 0 || !pSharedItem->bAutoRoll) {//might not give a shit about this bit.
						bool IWant = false;
						if (pSharedItem->AlwaysNeed || pSharedItem->AlwaysGreed) {
							//What does the 0 mean? Should we enum what these entries are?
							pAdvancedLootWnd->DoSharedAdvLootAction(pSharedItem, &CXStr(pChar->Name), 0, pSharedItem->LootDetails.m_array[0].StackCount);//Give the item to myself?
							return true;//Doing something with the loot, let's return true.
						}
						else if (pSharedItem->Never || pSharedItem->No) {//Leave this on the corpse.
							pAdvancedLootWnd->DoSharedAdvLootAction(pSharedItem, &CXStr(pChar->Name), 1, pSharedItem->LootDetails.m_array[0].StackCount);
						}
					}
				}
			}
		}
	}
	return false;//Didn't do any looting. So return false.
}