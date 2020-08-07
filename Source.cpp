// Good Boi
//
// Source: https://www.unknowncheats.me/forum/cs-go-releases/80163-counterstrike-global-offensive-complete-hack-tool-list.html


#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"

const std::string KVersion = "Smoothie Alpha 0.1";
const float KAlpha = 0.8f;
const int KDelay = 150;
const int KIterSleep = 5;

bool glow, radar;
uintptr_t moduleBase;
DWORD procId;
HWND hwnd;
HANDLE hProcess;


uintptr_t GetModuleBaseAddress(const char* modName) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, modName)) {
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}

// Read process memory template
template<typename T> T RPM(SIZE_T address) {
	T buffer;
	ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
	return buffer;
}

// Write process memory template
template<typename T> void WPM(SIZE_T address, T buffer) {
	WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
}

struct glowStructEnemy {
	float red = 1.f;
	float green = 0.f;
	float blue = 0.f;
	float alpha = KAlpha;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowEnm;

struct glowStructLocal {
	float red = 0.f;
	float green = 1.f;
	float blue = 0.f;
	float alpha = KAlpha;
	uint8_t padding[8];
	float unknown = 1.f;
	uint8_t padding2[4];
	BYTE renderOccluded = true;
	BYTE renderUnoccluded = false;
	BYTE fullBloom = false;
}glowLocal;

uintptr_t getLocalPlayer() {
	return RPM<uintptr_t>(moduleBase + dwLocalPlayer);
}

void drawIntro()
{
	std::cout <<	"\n\t .d8888b.                         888 888888b.            d8b\n" <<           
					"\td88P  Y88b                        888 888  \"88b           Y8P\n" <<
					"\t888    888                        888 888  .88P\n" <<
					"\t888         .d88b.   .d88b.   .d88888 8888888K.   .d88b.\n" <<
					"\t888  88888 d88\"\"88b d88\"\"88b d88\" 888 888  \"Y88b d88\"\"88b 888\n" <<
					"\t888    888 888  888 888  888 888  888 888    888 888  888 888\n" <<
					"\tY88b  d88P Y88..88P Y88..88P Y88b 888 888   d88P Y88..88P 888\n" <<
					"\t \"Y8888P88  \"Y88P\"   \"Y88P\"   \"Y88888 8888888P\"   \"Y88P\"  888 \n\n";

	std::cout <<	"\t .d8888b.                                  888    888      d8b\n" <<
					"\td88P  Y88b                                 888    888      Y8P\n" <<
					"\tY88b.                                      888    888\n" <<
					"\t \"Y888b.   88888b.d88b.   .d88b.   .d88b.  888888 88888b.  888  .d88b.  \n" <<
					"\t    \"Y88b. 888 \"888 \"88b d88\"\"88b d88\"\"88b 888    888 \"88b 888 d8P  Y8b\n" <<
					"\t      \"888 888  888  888 888  888 888  888 888    888  888 888 88888888 \n" <<
					"\tY88b  d88P 888  888  888 Y88..88P Y88..88P Y88b.  888  888 888 Y8b.\n" <<
					"\t \"Y8888P\"  888  888  888  \"Y88P\"   \"Y88P\"   \"Y888 888  888 888  \"Y8888\"\n\n";
}

void drawGlobal()
{
	system("cls");
	std::cout << "\n****************************************************\n";
	std::cout << "\t\t" << KVersion << std::endl << std::endl;

	std::cout << "\t#Shortcuts\n";
	std::cout << "\t    START\t\tRCTRL + LCTRL\n";
	std::cout << "\t    STOP\t\tRCTRL + END\n";
	std::cout << "\t    PAUSE ALL\t\tRCTRL + DEL\n";
	std::cout << "\t    GLOW\t\tRCTRL + INSERT\n";
	std::cout << "\t    RADAR\t\tRCTRL + HOME\n";

	std::cout << "\n\t#Status\n";
	std::cout << "\t    Glow  = " << glow << "\n";
	std::cout << "\t    Radar = " << radar << "\n";
	std::cout << "\n****************************************************\n";
}


int main() {

	std::string titleSet = "title Smoothie " + KVersion;
	system(titleSet.c_str());

	drawIntro();
	std::cout << "\tVersion " << KVersion << "\n";
	std::cout << "\tPress RCTRL + LCTRL to initialize\n";
	while (!GetAsyncKeyState(VK_LCONTROL) || !GetAsyncKeyState(VK_RCONTROL))
		Sleep(500);


	std::cout << "Nothing here, just a Smoothie :D\n\n";
	Sleep(1500);

	

	glow = true;
	radar = true;


	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId);
	moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	drawGlobal();
	while (!GetAsyncKeyState(VK_END) || !GetAsyncKeyState(VK_RCONTROL))
	{
		// GLOW
		if (glow)
		{
			uintptr_t dwGlowManager = RPM<uintptr_t>(moduleBase + dwGlowObjectManager);
			int LocalTeam = RPM<int>(getLocalPlayer() + m_iTeamNum);
			int i;

			for (i = 1; i < 32; i++) {
				uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
				int EnmHealth = RPM<int>(dwEntity + m_iHealth); if (EnmHealth < 1 || EnmHealth > 150) continue;
				int Dormant = RPM<int>(dwEntity + m_bDormant); if (Dormant) continue;
				int iGlowIndx = RPM<int>(dwEntity + m_iGlowIndex);
				int EntityTeam = RPM<int>(dwEntity + m_iTeamNum);

				if (LocalTeam == EntityTeam)
				{
					WPM<glowStructLocal>(dwGlowManager + (iGlowIndx * 0x38) + 0x4, glowLocal);
				}
				else if (LocalTeam != EntityTeam)
				{
					WPM<glowStructEnemy>(dwGlowManager + (iGlowIndx * 0x38) + 0x4, glowEnm);
				}

				// Radar Hack
				if (radar)
					WPM<bool>(dwEntity + m_bSpotted, true);
			}

			// If glow is ON also, we can take advantage of the dwEntity reads from the GLOW loop (0 - 31), then just keep going from here (31 - 63)
			if (radar)
			{	
				for (; i < 64; i++)
				{
					uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
					WPM<bool>(dwEntity + m_bSpotted, true);
				}
			}
		}

		// RADAR
		if (radar && !glow)
		{
			for (int i = 1; i < 64; i++)
			{
				uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + dwEntityList + i * 0x10);
				WPM<bool>(dwEntity + m_bSpotted, true);
			}
		}


		// STATUS SWITCHES
		if (glow)	// Glow switch
		{
			if (GetAsyncKeyState(VK_INSERT) && GetAsyncKeyState(VK_RCONTROL))
			{
				glow = false;

				drawGlobal();
				Sleep(KDelay);
			}
		}
		else
		{
			if (GetAsyncKeyState(VK_INSERT) && GetAsyncKeyState(VK_RCONTROL))
			{
				glow = true;

				drawGlobal();
				Sleep(KDelay);
			}
		}

		if (radar)	// Radar switch
		{
			if (GetAsyncKeyState(VK_HOME) && GetAsyncKeyState(VK_RCONTROL))
			{
				radar = false;

				drawGlobal();
				Sleep(KDelay);
			}
		}
		else
		{
			if (GetAsyncKeyState(VK_HOME) && GetAsyncKeyState(VK_RCONTROL))
			{
				radar = true;

				drawGlobal();
				Sleep(KDelay);
			}
		}
		
		if (GetAsyncKeyState(VK_DELETE) && GetAsyncKeyState(VK_RCONTROL))	// Pause ALL
		{
			glow = false;
			radar = false;

			drawGlobal();
			Sleep(KDelay);
		}

		Sleep(KIterSleep);
	}

	std::cout << "\n\t@ End @\n";
	getchar();

	exit(0);
}
