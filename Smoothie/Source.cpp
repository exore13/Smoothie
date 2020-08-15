// Good Boi
//
// Source: https://www.unknowncheats.me/forum/cs-go-releases/80163-counterstrike-global-offensive-complete-hack-tool-list.html
#include <iostream>
#include <string>
#include <Windows.h>
#include <map>
#include <TlHelp32.h>
#include <filesystem>
#include <fstream>
#include <vector>
#pragma comment (lib, "urlmon.lib") 



const std::string KVersion = "Alpha 0.2";	// Program version
const float KAlpha = 0.8f;					// Transparency for the outline. Default 0.8f (80%)
const int KDelay = 150;						// Time to wait after an input was detected.
const int KIterSleep = 1;					// Time to wait(ms) after each iteration

std::vector<std::string> wantedOffsets;		// Vector where will be the wanted offsets
std::map<std::string, int> offsetsMap;		// Map used to store the offsets and their values
bool glow, radar, endProgram;				// Status variables used across the program

// Set of variables needed for memory reading and writing
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

	return NULL;
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




// Helper module that will work as an alias to read local player for memory easier
uintptr_t getLocalPlayer() {
	return RPM<uintptr_t>(moduleBase + offsetsMap.at("dwLocalPlayer"));
}


// Helper module that will print out a kind of spash start screen
void drawIntro()
{
	// Made using an online tool
	// http://www.patorjk.com/software/taag/#p=display&h=1&v=0&f=Colossal&t=GoodBoi%0ASmoothie
	std::cout << "\n\t .d8888b.                         888 888888b.            d8b\n" <<
		"\td88P  Y88b                        888 888  \"88b           Y8P\n" <<
		"\t888    888                        888 888  .88P\n" <<
		"\t888         .d88b.   .d88b.   .d88888 8888888K.   .d88b.\n" <<
		"\t888  88888 d88\"\"88b d88\"\"88b d88\" 888 888  \"Y88b d88\"\"88b 888\n" <<
		"\t888    888 888  888 888  888 888  888 888    888 888  888 888\n" <<
		"\tY88b  d88P Y88..88P Y88..88P Y88b 888 888   d88P Y88..88P 888\n" <<
		"\t \"Y8888P88  \"Y88P\"   \"Y88P\"   \"Y88888 8888888P\"   \"Y88P\"  888 \n\n";

	std::cout << "\t .d8888b.                                  888    888      d8b\n" <<
		"\td88P  Y88b                                 888    888      Y8P\n" <<
		"\tY88b.                                      888    888\n" <<
		"\t \"Y888b.   88888b.d88b.   .d88b.   .d88b.  888888 88888b.  888  .d88b.  \n" <<
		"\t    \"Y88b. 888 \"888 \"88b d88\"\"88b d88\"\"88b 888    888 \"88b 888 d8P  Y8b\n" <<
		"\t      \"888 888  888  888 888  888 888  888 888    888  888 888 88888888 \n" <<
		"\tY88b  d88P 888  888  888 Y88..88P Y88..88P Y88b.  888  888 888 Y8b.\n" <<
		"\t \"Y8888P\"  888  888  888  \"Y88P\"   \"Y88P\"   \"Y888 888  888 888  \"Y8888\"\n\n";
}


// Helper module that will print out the program state
void drawGlobal()
{
	// This code follows an asynchronous aproach to displaying information
	// Every time there is a status change, the display updates

	system("cls");
	std::cout << "\n****************************************************\n";
	std::cout << "\t\tSmoothie " << KVersion << std::endl << std::endl;

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


// Gets the pointer to the adress of client.dll
void getWindow()
{
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId);
	moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
}


// Modifies the memory to mark entities on the minimap as spotted
void applyRadar(const int& lowerIndex, const int& upperIndex)
{
	// upperIndex should be the max number of entities to draw, tried 64 and 32 and both worked fine
	// Maybe a higher number count could be usefull in community servers with higher max player cap
	// lowerIndex by default should be 1, but you can use 32 mid applyGlow execution to reduce memory reads
	// I have not tested this, but memory access should be slower than accesing a variable
	for (int i = lowerIndex; i < upperIndex; i++)
	{
		uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + offsetsMap.at("dwEntityList") + i * 0x10);
		WPM<bool>(dwEntity + offsetsMap.at("m_bSpotted"), true);
	}
}


// Modifies the memory to draw an outline around the players. Different from teammates to enemies
void applyGlow()
{
	uintptr_t dwGlowManager = RPM<uintptr_t>(moduleBase + offsetsMap.at("dwGlowObjectManager"));
	int LocalTeam = RPM<int>(getLocalPlayer() + offsetsMap.at("m_iTeamNum"));
	int i;

	for (i = 1; i < 32; i++) {
		uintptr_t dwEntity = RPM<uintptr_t>(moduleBase + offsetsMap.at("dwEntityList") + i * 0x10);
		int EnmHealth = RPM<int>(dwEntity + offsetsMap.at("m_iHealth")); if (EnmHealth < 1 || EnmHealth > 150) continue;
		int Dormant = RPM<int>(dwEntity + offsetsMap.at("m_bDormant")); if (Dormant) continue;
		int iGlowIndx = RPM<int>(dwEntity + offsetsMap.at("m_iGlowIndex"));
		int EntityTeam = RPM<int>(dwEntity + offsetsMap.at("m_iTeamNum"));

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
			WPM<bool>(dwEntity + offsetsMap.at("m_bSpotted"), true);
	}

	// If glow is ON also, we can take advantage of the dwEntity reads from the GLOW loop (1 - 32), then just keep going from here (32 - 63)
	if (radar)
	{
		applyRadar(i, 64);
	}
}


// Module in charge of key reads and status changes
void inputManager()
{
	if (GetAsyncKeyState(VK_END) && GetAsyncKeyState(VK_RCONTROL))
	{
		endProgram = true;
		return;
	}

	// STATUS SWITCHES
	if (glow)	// Glow switch
	{
		if (GetAsyncKeyState(VK_INSERT) && GetAsyncKeyState(VK_RCONTROL))
		{
			glow = false;

			drawGlobal();
			Sleep(KDelay);
			return;
		}
	}
	else
	{
		if (GetAsyncKeyState(VK_INSERT) && GetAsyncKeyState(VK_RCONTROL))
		{
			glow = true;

			drawGlobal();
			Sleep(KDelay);
			return;
		}
	}

	if (radar)	// Radar switch
	{
		if (GetAsyncKeyState(VK_HOME) && GetAsyncKeyState(VK_RCONTROL))
		{
			radar = false;

			drawGlobal();
			Sleep(KDelay);
			return;
		}
	}
	else
	{
		if (GetAsyncKeyState(VK_HOME) && GetAsyncKeyState(VK_RCONTROL))
		{
			radar = true;

			drawGlobal();
			Sleep(KDelay);
			return;
		}
	}

	if (GetAsyncKeyState(VK_DELETE) && GetAsyncKeyState(VK_RCONTROL))	// Pause ALL
	{
		glow = false;
		radar = false;

		drawGlobal();
		Sleep(KDelay);
		return;
	}
}


// Adds the entries to the wanted offsets vector. If you are willing to expand the code you will
// have to add them manually here
void prepareWantedOffsetsFilter()
{
	wantedOffsets.push_back(std::string("dwLocalPlayer"));
	wantedOffsets.push_back(std::string("dwEntityList"));
	wantedOffsets.push_back(std::string("dwGlowObjectManager"));
	wantedOffsets.push_back(std::string("m_bSpotted"));
	wantedOffsets.push_back(std::string("m_iTeamNum"));
	wantedOffsets.push_back(std::string("m_iHealth"));
	wantedOffsets.push_back(std::string("m_bDormant"));
	wantedOffsets.push_back(std::string("m_iGlowIndex"));
}

// Checks if the given string is a wanted offset
bool isOffsetWanted(const std::string& offset)
{
	for (const std::string& entry : wantedOffsets)
		if (offset == entry)
			return true;

	return false;
}


// Reads from offsets.cfg and searches for the wanted vars, then inserts them on a map
void readOffsets(const std::string& fileName)
{
	std::ifstream entryFile(fileName.c_str());
	if (!entryFile.is_open())
	{
		std::cout << "Error trying to open file " << fileName << "\n";
		return;
	}

	std::string dump, varName, varValue;
	getline(entryFile, dump);
	while (!entryFile.eof())
	{
		varName = dump.substr(0, dump.find_first_of(' '));
		if (!isOffsetWanted(varName))
		{
			getline(entryFile, dump);
			continue;
		}

		varValue = dump.substr(dump.find_last_of(' ') + 1);

		offsetsMap.insert(std::pair <std::string, int>(varName, stoi(varValue)));
		getline(entryFile, dump);
	}

	entryFile.close();
}


// Checks local directory if there is a file
bool checkLocalOffsets(const char* targetStr)
{
	// INFO
	// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-wsprintfa
	TCHAR path[MAX_PATH], target[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);

	wsprintfA(target, TEXT(targetStr), path);
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.path().string() == target) {
			return true;
		}
	}

	return false;
}


// Looks for an older offsets.cfg file. If there is one, it changes it's name to offsets.cfg.old. If there
// is already an offsets.cfg.old file, it delets it
void checkOldOffsets()
{
	// If there is no old offsets file, return
	if (!checkLocalOffsets("%s\\offsets.cfg"))
	{
		return;
	}


	if (checkLocalOffsets("%s\\offsets.cfg.old"))
	{
		system("del offsets.cfg.old");
	}

	system("rename offsets.cfg offsets.cfg.old");
}


// Tries to download the latest version of csgo offsets from hazedumper's github. If the download fails
// it prints the error and tries to read the offsets from a older local file
bool getOffsets()
{
	// INFO
	// https://stackoverflow.com/questions/5184988/should-i-use-urldownloadtofile
	// https://docs.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/platform-apis/ms775123(v=vs.85)

	TCHAR url[] = TEXT("https://raw.githubusercontent.com/frk1/hazedumper/master/csgo.toml");
	TCHAR path[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, path);
	wsprintf(path, TEXT("%s\\offsets.cfg"), path);	// Adds the offset filename to the absolute path

	checkOldOffsets();		// If there is a old file, saves it as offsets.cfg.old

	HRESULT res = URLDownloadToFile(NULL, url, path, 0, NULL);	// Downloads the file from the url


	if (res == S_OK) {		// Download suscessfull
		readOffsets((std::string)path);
	}

	else {					// Downlaod Failed
		std::cout << "Error trying to download new offsets file\n\n" << res << "\n\nTrying to read old offsets from local file\n";

		if (checkLocalOffsets("%s\\offsets.cfg.old")) {
			TCHAR filename[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, filename);
			wsprintf(filename, TEXT("%s\\offsets.cfg.old"), filename);

			readOffsets((std::string)filename);
		}

		else
		{
			std::cout << "No local offsets were found. Program could not continue\n";
			return false;
		}
	}

	return true;
}


// Main initialization module. Sets some decorations and prepares the code to run
bool init()
{
	// Title decoration set
	std::string titleSet = "title Smoothie " + KVersion;
	system(titleSet.c_str());

	// Draws the intro splash
	drawIntro();


	// Prepares an array with a wanted offset name on each entry
	prepareWantedOffsetsFilter();

	// Tries to get/update the offsets
	if (!getOffsets())
	{
		return false;	// Error downloading and no local offset file. Program not working
	}

	// Links to csgo
	getWindow();

	// Initialize status vars
	endProgram = false;
	glow = true;
	radar = true;

	std::cout << "\tVersion " << KVersion << "\n";
	std::cout << "\tPress RCTRL + LCTRL to initialize\n";
	while (!GetAsyncKeyState(VK_LCONTROL) || !GetAsyncKeyState(VK_RCONTROL))
		Sleep(500);

	std::cout << "\n\tNothing here, just a Smoothie :D\n\n";
	Sleep(500);
	return true;
}



int main() {

	if (init())
	{
		drawGlobal();

		while (!endProgram)
		{
			if (glow)
				applyGlow();

			if (radar && !glow)
				applyRadar(1, 64);

			inputManager();
			Sleep(KIterSleep);
		}
	}

	std::cout << "\n\t@ End @\n";

	exit(0);
}
