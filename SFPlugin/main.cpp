#include <Windows.h>
#include <shellapi.h>
#include "main.h"
#include <vector>
#include <iostream>

using namespace std;
SAMPFUNCS *SF = new SAMPFUNCS();

struct Gang {
	string name;
	uint color;
	bool enabled;
};

static int DialogId = 9182;
uint MaskColor = 2159918525;

vector <Gang> Settings{
	Gang{"Без организации", (uint)368966908, false},
	Gang{"Grove Street", (uint)2566951719, false},
	Gang{"Los Santos Vagos", (uint)2580667164, false},
	Gang{"East Side Ballas", (uint)2580283596, false},
	Gang{"Varrios Los Aztecas", (uint)2566979554, false},
	Gang{"The Rifa", (uint)2573625087, false},
	Gang{"La Cosa Nostra", (uint)2157523814, false},
	Gang{"Warlock MC", (uint)2159694877, false},
	Gang{"Night Wolves", (uint)23486046, false},
	Gang{"Russian Mafia", (uint)2150852249, false},
	Gang{"Yakuza", (uint)2157314562, false},
	Gang{"Правительство", (uint)2160918272, false},
	Gang{"Страховая компания", (uint)2152104628, false},
	Gang{"Центральный Банк", (uint)2150206647, false},
	Gang{"Инструкторы", (uint)2164221491, false},
	Gang{"Больница", (uint)2164227710, false},
	Gang{"Полиция", (uint)2147503871, false},
	Gang{"Армия/ТСР", (uint)2157536819, false},
	Gang{"TV студия", (uint)2164228096, false},
};

string playerColorToHexCode(uint playerColor) {
	char* Full;
	sprintf(Full, "%x", playerColor);
	string result = (string)Full;
	return "{" + result.substr(2, result.length()) + "}";
}

void ShowDialog() {
	string Text = "Организация\tУдаление игроков\n";
	for (int GangIndex = 0; GangIndex < Settings.size(); GangIndex++) {
		string HEX = playerColorToHexCode(Settings[GangIndex].color);
		Text.append((HEX.length() == 8 ? HEX : "{ffffff}") + Settings[GangIndex].name + "\t\t" + (Settings[GangIndex].enabled ? "{4fffaa}Включено (игроки будут удалятся)" : "{ff4f4f}Выключено") + "\n");
	}
	SF->getSAMP()->getDialog()->ShowDialog(DialogId, 5, "{674fff}ARZ Player Remover by chapo (.SF version)", (char*)Text.c_str(), "Изменить", "Закрыть");
	cout << "[ARZ PLAYER REMOVER]: Dialog shown!" << endl;
}

// CALLBACKS
void CALLBACK commandCallback(string arg) { ShowDialog(); }
void CALLBACK dialogCallback(int dialogId, int buttonId, int listItem, const char* input) {
	if (dialogId == DialogId && buttonId == 1) {
		if (listItem >= 0 && listItem <= Settings.size()) {
			Settings[listItem].enabled = !Settings[listItem].enabled;
			cout << "[ARG PLAYER REMOVER]: Gang state changed! Gang: " + Settings[listItem].name + ", State: " + (Settings[listItem].enabled ? "ON" : "OFF") << endl;
			ShowDialog();
		}
	}
}

void __stdcall mainloop()
{
	static bool initialized = false;
	if (!initialized)
	{
		if (GAME && GAME->GetSystemState() == eSystemState::GS_PLAYING_GAME && SF->getSAMP()->IsInitialized())
		{
			initialized = true;
			cout << "[ARZ PLAYER REMOVER]: Loaded! Command: /plrm" << endl;
			SF->getSAMP()->getChat()->AddChatMessage(0xFF674fff, "[ARZ PLAYER REMOVER]:{ffffff} Загружен, меню: {674fff}/plrm{ffffff}, автор:{674fff} chapo");
			SF->getSAMP()->registerChatCommand("plrm", commandCallback);
			SF->getSAMP()->registerDialogCallback(dialogCallback);
		}
	}
	if (SF->getSAMP()->IsInitialized()) {
		for (int ID = 0; ID < SAMP_MAX_PLAYERS; ID++)
		{
			if (ID != SF->getSAMP()->getPlayers()->sLocalPlayerID && SF->getSAMP()->getPlayers()->IsPlayerDefined(ID, true)) {
				for (int GangIndex = 0; GangIndex < Settings.size(); GangIndex++) {
					if (Settings[GangIndex].enabled && SF->getSAMP()->getPlayers()->GetPlayerColor(ID) == Settings[GangIndex].color) {
						//int Handle = SF->getSAMP()->getPlayers()->GetActorHandleFromSAMPPlayerID(ID);
						BitStream BS;
						BS.Write((int16)ID);
						SF->getRakNet()->emulateRecvRPC(RPC_ScrWorldPlayerRemove, &BS);
						cout << "Removed player " << ID << ". Reason: Gang \"" + Settings[GangIndex].name + "\"" << endl;
					}
				}
			}
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	if (dwReasonForCall == DLL_PROCESS_ATTACH) {
		SF->initPlugin(mainloop, hModule);
	}
	return TRUE;
}

//ARZ_PlayerRemoverByChapo