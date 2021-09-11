#pragma once
#include "..\MQ2Plugin.h"

bool AutoLootAllIsOn();
int GroupTotal();
bool IAmMasterLooter();
bool WinState(CXWnd* pWnd);
bool HandlePersonalLoot(PCHARINFO pChar, PCHARINFO2 pChar2, PEQADVLOOTWND pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList);
bool HandleSharedLoot(PCHARINFO pChar, PCHARINFO2 pChar2, PEQADVLOOTWND pAdvLoot, CListWnd* pPersonalList, CListWnd* pSharedList);