# Smoothie

![Smoothie image](https://i.imgur.com/c2KcIwK.png)
Simple wallhack and radarhack for CSGO.

- [Description](#Description)
- [Instructions](#Instructions)
- [Screenshots](#Screenshots)
- [Requirements](#Requirements)
- [Credits](#Credits)


- ⛔ Not meant to use to gain advantage on online matches.
- ⚠️ We are not liable for VAC bans in case of incorrect use.


## Description

_Smoothie_ is just an experiment. It is not meant to be use to gain advantage on online games. 
It was built out of curiosity of how reading and writing memory on windows works and testing Valve's Anticheat measures on this game. 
As context, I've played around 3k hours of CS 1.6 and CSS around cybercafes and pirated versions, and 1.5k hours of CSGO on my main Steam Accout. I know a few tricks around the source engine and decided to experiment a litle bit more with it.

The program just gets the memory address of CSGO's client.dll and using offsets it can read and write directly from memory.
The offsets change with each game update, so to ensure this program still works, I've put a feature that autoupdates the offsets each time the program launches, based on [frk1/hazedumper](https://github.com/frk1/hazedumper)'s repository.

Each time the program launches, it checks to see if there are any old `offsets.cfg` on the launch directory. If there is, it changes that file name to `offsets.cfg.old` and procedes to download a new `offsets.cfg` from [frk1/hazedumper](https://github.com/frk1/hazedumper)'s repository. If the download fails, the program procedes to read the offsets from the `offsets.cfg.old`. If there isn't any file with that name, it closes.

If the game updates, check [frk1/hazedumper](https://github.com/frk1/hazedumper)'s repository to see if the offsets are up to date.

## Instructions

To build Smoothie: 
- Launch the project with Microsoft Visual Studio 2019. 
- Select Release x86 mode.
- Set on Project Properties C++ standard to `std:c++17`.
- Set on Project Properties Character Set to `Use Multi-Byte Character Set`.

To use Smoothie:
- Download the latest compiled version: [MEGA](https://mega.nz/folder/yyAXiIhB#RBZJv9kRIS7KGkS9g3e8og)
- Install Microsoft VC Redist 2015 x86.
- Launch Smoothie and CSGO. Order doesn't matters.
- Once both Smoothie and CSGO are launched. Press LeftControl + RightControl to Start.
- Smoothie is ready to use.
![Smoothie menu image](https://i.imgur.com/lpm645k.png?1)

## Screenshots

![Screenshot 1](https://i.imgur.com/qXs42vE.png)
![Screenshot 2](https://i.imgur.com/XvXPiE9.png)
![Screenshot 3](https://i.imgur.com/P6Z5zRD.png)
![Screenshot 4](https://i.imgur.com/ziyNkCR.png)

## Requirements

- [Microsoft VC Redist 2015 x86](https://www.microsoft.com/en-us/download/details.aspx?id=48145)

## Credits
- [https://github.com/frk1/hazedumper](https://github.com/frk1/hazedumper)
- [https://www.unknowncheats.me/forum/counterstrike-global-offensive/](https://www.unknowncheats.me/forum/counterstrike-global-offensive/)

```
 .d8888b.                         888 888888b.            d8b 
d88P  Y88b                        888 888  "88b           Y8P 
888    888                        888 888  .88P               
888         .d88b.   .d88b.   .d88888 8888888K.   .d88b.  888 
888  88888 d88""88b d88""88b d88" 888 888  "Y88b d88""88b 888 
888    888 888  888 888  888 888  888 888    888 888  888 888 
Y88b  d88P Y88..88P Y88..88P Y88b 888 888   d88P Y88..88P 888 
 "Y8888P88  "Y88P"   "Y88P"   "Y88888 8888888P"   "Y88P"  888 
```

