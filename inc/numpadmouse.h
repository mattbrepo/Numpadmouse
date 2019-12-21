#ifndef NUMPADMOUSE_H
#define NUMPADMOUSE_H

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "resource.h"
#include "manageevent.h"

#define APP_VERSION "numpadmouse v0.1" //%%recuperare la versione dalla risorsa!
#define APP_CFG_FILE "numpadmouse.cfg"

//------- application
HINSTANCE _hAppInstance; //application instance
HWND _hMainWnd; //main window HWND
HHOOK _ghKbrdHook; //keyboard event hook

//------- path variables
char _appPath[MAX_PATH]; //application dir path
char _appFilePath[MAX_PATH]; //application file path
char _cfgFilePath[MAX_PATH]; //configuration file path

//------- commands
#define MAX_COMMANDS 100
#define MAX_KEYMAP_LENGTH 100

typedef struct 
{
  char keyMap[MAX_PATH];
  char progPath[MAX_PATH];
  bool ctrlState;
  bool shiftState;
  bool altState;
} NP_COMMANDS;

NP_COMMANDS _commands[MAX_COMMANDS];
int _commandsMax; //real max number of commands (< MAX_COMMANDS)
int _longestKeyMap; //longest key map loaded (< MAX_KEYMAP_LENGTH)
char _currentKeyMap[MAX_KEYMAP_LENGTH]; //current key map
int _currentKeyMapLength; //current key map real length (< MAX_KEYMAP_LENGTH)

//------- functions
void SetMsg(char* keyboardEvent, char* sMsg);
void SetTNAIcon(int addFlag, int idi_icon);
bool ReadCfgFile(char* filePath);

#endif