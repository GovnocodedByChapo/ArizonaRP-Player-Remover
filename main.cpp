#include <Windows.h>
#include <shellapi.h>
#include "main.h"
#include <vector>
#include <iostream>

using namespace std;
SAMPFUNCS *SF = new SAMPFUNCS();

static int DialogId = 9182;

struct Gang {
	string name;
	uint color;
	bool enabled;
};

vector <Gang> Settings{
	Gang{"Grove Street", 0x99009327, false},
	Gang{"Ballas", 0x99CC00CC, false},
	Gang{"Rifa", 0x996666FF, false},
	Gang{"Vagos", 0x99D1DB1C, false},
	Gang{"Aztec", 0x9900FFE2, false},
	Gang{"Night Wolfs", 0x807F6464, false},
	Gang{"TEST HMS Pink", (uint)4293977740, false}
};

void ShowDialog() {
	string Text = "";

	for (int GangIndex = 0; GangIndex < Settings.size(); GangIndex++) {
		Text.append(Settings[GangIndex].name + "\t\t" + (Settings[GangIndex].enabled ? "{4fffaa}Включено (игроки будут удалятся)" : "{ff4f4f}Выключено") + "\n");
	}
	SF->getSAMP()->getDialog()->ShowDialog(DialogId, 4, "{674fff}ARZ Clist Remover by chapo (.SF version)", (char*)Text.c_str(), "Изменить", "Закрыть");
	cout << "[ARG GANG REMOVER]: Dialog shown!" << endl;
}

// CALLBACKS
void CALLBACK commandCallback(string arg) { ShowDialog(); }
void CALLBACK getplayercolor(string arg) { cout << SF->getSAMP()->getPlayers()->GetPlayerColor(stoi(arg)) << endl; }
void CALLBACK dialogCallback(int dialogId, int buttonId, int listItem, const char* input) {
	if (dialogId == DialogId && buttonId == 1) {
		if (listItem >= 0 && listItem <= Settings.size()) {
			Settings[listItem].enabled = !Settings[listItem].enabled;
			cout << "[ARG GANG REMOVER]: Gang state changed! Gang: " + Settings[listItem].name + ", State: " + (Settings[listItem].enabled ? "ON" : "OFF") << endl;
		}
		ShowDialog();
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
			SF->getSAMP()->getChat()->AddChatMessage(0xFF674fff, "[ARZ CLIST REMOVER]:{ffffff} Загружен, меню: {674fff}/gangrem{ffffff}, автор:{674fff} chapo");
			cout << "[ARG GANG REMOVER]: Loaded! Command: /gangrem" << endl;
			SF->getSAMP()->registerChatCommand("gangrem", commandCallback);
			SF->getSAMP()->registerChatCommand("getplayercolor", getplayercolor);
			SF->getSAMP()->registerDialogCallback(dialogCallback);
		}
		
	}
	for (int ID = 0; ID < SAMP_MAX_PLAYERS; ID++)
	{
		if (SF->getSAMP()->getPlayers()->IsPlayerDefined(ID, true)) {
			//cout << ID << endl;
			for (int GangIndex = 0; GangIndex < Settings.size(); GangIndex++) {
				if (SF->getSAMP()->getPlayers()->GetPlayerColor(ID) == Settings[GangIndex].color && Settings[GangIndex].enabled) {
					int Handle = SF->getSAMP()->getPlayers()->GetActorHandleFromSAMPPlayerID(ID);
					
					// Remove player ( RPC_ScrWorldPlayerRemove )
					BitStream BS;
					BS.Write((int16)ID);
					SF->getRakNet()->emulateRecvRPC(RPC_ScrWorldPlayerRemove, &BS);
					cout << "Remove player " << ID << ". Reason: Gang \"" + Settings[GangIndex].name + "\"" << endl;
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