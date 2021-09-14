#pragma once
#include <mq/Plugin.h>
const std::string PluginMsg = "\ar[\a-tMQ2LootManager\ar]\ao:: ";
enum ItemSize : int {
	Tiny,
	Small,
	Medium,
	Large,
	Giant
};

bool AutoLootAllIsOn();
int GroupTotal();
bool IAmMasterLooter();
bool WinState(CXWnd* pWnd);
bool HandlePersonalLoot(PCHARINFO pChar, PcProfile* pChar2, CAdvancedLootWnd* pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList);
bool HandleSharedLoot(PCHARINFO pChar, PcProfile* pChar2, CAdvancedLootWnd* pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList);