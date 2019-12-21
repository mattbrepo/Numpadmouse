#include "numpadmouse.h"

#define ID_TRAY_APP_ICON 1
#define ID_TRAY_EXIT 1
#define ID_TRAY_ABOUT 2
#define WM_SYSICON WM_USER + 100
NOTIFYICONDATA _niData; //TNA

HWND _hTxtKeyboard; //keyboard data textbox
HWND _hTxtMsg; //message textbox
bool _readingCfgFile; //lock

//
// Set messages on main window
void SetMsg(char* keyboardEvent, char* sMsg)
{
  const int MAX_MSG_LEN = 2000;
  if (keyboardEvent != NULL)
    SetWindowTextA(_hTxtKeyboard, keyboardEvent);

  if (sMsg == NULL)
  {
    SetWindowTextA(_hTxtMsg, "");
    return;
  }
  
  if (strlen(sMsg) == 0) return; //if it's empty
  
  char msg[MAX_MSG_LEN];
  GetWindowTextA(_hTxtMsg, msg, MAX_MSG_LEN);
  strcat(msg, sMsg);
  strcat(msg, "\r\n");
  SetWindowTextA(_hTxtMsg, msg);
}

//
// Set Tray Notification Area Icon
void SetTNAIcon(int addFlag, int idiIcon)
{
  if (addFlag)
  {
    //set the icon
    ZeroMemory(&_niData,sizeof(NOTIFYICONDATA));
    _niData.cbSize = sizeof(NOTIFYICONDATA);

    _niData.uID = ID_TRAY_APP_ICON;
    _niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    _niData.hIcon = (HICON)LoadImage(_hAppInstance, MAKEINTRESOURCE(idiIcon), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_DEFAULTCOLOR);

    _niData.hWnd = _hMainWnd;
    _niData.uCallbackMessage = WM_SYSICON;
    lstrcpy(_niData.szTip, TEXT(APP_VERSION));
    
    Shell_NotifyIcon(NIM_ADD, &_niData);
  }
  else
  {
    //just change the icon
    _niData.hIcon = (HICON)LoadImage(_hAppInstance, MAKEINTRESOURCE(idiIcon), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    Shell_NotifyIcon(NIM_MODIFY, &_niData);
  }

  if(_niData.hIcon && DestroyIcon(_niData.hIcon)) _niData.hIcon = NULL;
}

//
// Reads the configuration file
bool ReadCfgFile0(char* filePath)
{
  const int MAX_LINE_LENGTH = MAX_PATH * 2;
  FILE *stream;
  char line[MAX_LINE_LENGTH];
  char *p;
  
  //pro debug
  //SetMsg(NULL, filePath);
  
  if((stream = fopen(filePath, "r" )) == NULL)
    return false;

  _commandsMax = 0;
  _longestKeyMap = -1;
  while ((fgets(line, MAX_LINE_LENGTH, stream) != NULL) && ((_commandsMax + 1) < MAX_COMMANDS))
  {
    //syntax: <key map>=<program path and arguments...>
    if (strlen(line) < 3) continue; 
    if (line[0] == '#') continue; //comment
    
    if (line[strlen(line)-1] == '\n' || line[strlen(line)-1] == '\r') line[strlen(line)-1] = '\0';
    if (line[strlen(line)-2] == '\r') line[strlen(line)-1] = '\0';

    p = strchr(line, '=');
    if (p == NULL) continue;

    if ((p - line) > MAX_KEYMAP_LENGTH) continue;
    strncpy(_commands[_commandsMax].keyMap, line, p - line);
    strcpy(_commands[_commandsMax].progPath, p + 1);

    //CTRL/SHIFT/ALT
    if (_commands[_commandsMax].keyMap[0] == '^') _commands[_commandsMax].ctrlState = true;
    else if (_commands[_commandsMax].keyMap[0] == '+') _commands[_commandsMax].shiftState = true;
    else  if (_commands[_commandsMax].keyMap[0] == '%') _commands[_commandsMax].altState = true;
    if (_commands[_commandsMax].ctrlState || _commands[_commandsMax].shiftState || _commands[_commandsMax].altState)
        strcpy(_commands[_commandsMax].keyMap, _commands[_commandsMax].keyMap + 1);

    //---pro debug:
    //char buffer[MAX_PATH*3];
    // //sprintf(buffer,"%s: %d (%d)",_commands[_commandsMax].keyMap, strlen(_commands[_commandsMax].keyMap), _longestKeyMap);
    // sprintf(buffer,"%s: %s",_commands[_commandsMax].keyMap, _commands[_commandsMax].progPath);
    //sprintf(buffer,"%s: %d", _commands[_commandsMax].progPath, strlen(_commands[_commandsMax].progPath));
    //sprintf(buffer,"%s: %d", line, strlen(line));
    //sprintf(buffer,"%s: %d",_commands[_commandsMax].keyMap, _commands[_commandsMax].ctrlState);
    //SetMsg(NULL, buffer);
    //---
    
    if (((int)strlen(_commands[_commandsMax].keyMap)) > _longestKeyMap)
    {
      //pro debug:SetMsg(NULL, "longest");
      _longestKeyMap = strlen(_commands[_commandsMax].keyMap);
    }
    _commandsMax++;
  }

  fclose(stream);
  return true;
}

//
// Reads the configuration file
bool ReadCfgFile(char* filePath)
{
  if (_readingCfgFile) return false; //it's still reading..
  _readingCfgFile = true;
  bool res = ReadCfgFile0(filePath);
  _readingCfgFile = false;
  return res;
}

//
// Keyboard event manager
DWORD FAR PASCAL KeyboardHookLL(int iCode, WPARAM wParam, LPARAM lParam) 
{
  if (iCode < 0 || iCode != HC_ACTION)
    return CallNextHookEx(_ghKbrdHook, iCode, wParam, lParam);
     
  KBDLLHOOKSTRUCT* lParamKB = (KBDLLHOOKSTRUCT*)(lParam);
  if(lParamKB != 0) 
  {
      if (ManageEvent(wParam, lParamKB)) return 1; //stop event process
  }

  return CallNextHookEx(_ghKbrdHook, iCode, wParam, lParam);
}

//
// Main window callbacks manager
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {  
    case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
        //case ID_FILE_EXIT:
        //{
        //  DestroyWindow(hWnd);
        //  return 0;
        //}
      }
      break;
    }

    case WM_GETMINMAXINFO:
    {
      MINMAXINFO *minMax = (MINMAXINFO*) lParam;
      minMax->ptMinTrackSize.x = 220;
      minMax->ptMinTrackSize.y = 110;

      return 0;
    }

    case WM_SYSICON:
    {
      //if (wParam == ID_TRAY_APP_ICON) SetForegroundWindow(hWnd);
      if (lParam == WM_RBUTTONUP)
      {
          SetMsg(NULL, "WM_RBUTTONUP");
          // Get current mouse position.
          POINT curPoint;
          GetCursorPos(&curPoint);
          HMENU Hmenu = CreatePopupMenu();
          AppendMenu(Hmenu, MF_STRING, ID_TRAY_ABOUT, TEXT("About"));
          AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));
          
          // TrackPopupMenu blocks the app until TrackPopupMenu returns
          UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x,curPoint.y, 0, hWnd, NULL);
           
          if (clicked == ID_TRAY_EXIT) DestroyWindow(hWnd); //quit the application
          else if (clicked == ID_TRAY_ABOUT) MessageBoxA(NULL, APP_VERSION, APP_VERSION, MB_OK);
      }
      break;
    }
    
    case WM_CREATE:
    {
      //keyboard event hook definition
      HOOKPROC lpfnKbrdHook = (LPVOID)MakeProcInstance((FARPROC)KeyboardHookLL, _hAppInstance);
      _ghKbrdHook = SetWindowsHookEx(WH_KEYBOARD_LL, lpfnKbrdHook, _hAppInstance, 0);
      return 0;
    }

    case WM_CLOSE:
    {
      DestroyWindow(hWnd);
      return 0;
    }

    case WM_DESTROY:
    {
      if (_ghKbrdHook > 0) UnhookWindowsHookEx(_ghKbrdHook);
      Shell_NotifyIcon(NIM_DELETE, &_niData);
      
      PostQuitMessage(0);
      return 0;
    }
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

//
// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  const int WIN_WIDTH = 800;
  const int WIN_HEIGHT = 600;
  const int OBJ_WIDTH = WIN_WIDTH - 20;
  const int TXT_KB_HEIGHT = 50;
  const int TXT_DEBUG_HEIGHT = WIN_HEIGHT - TXT_KB_HEIGHT - 50;
  
  _hAppInstance = hInstance;

  //init common controls
  INITCOMMONCONTROLSEX icc;
  icc.dwSize = sizeof(icc);
  icc.dwICC = ICC_WIN95_CLASSES;
  InitCommonControlsEx(&icc);
  
  //init application paths
  GetModuleFileNameA(hInstance, _appFilePath, MAX_PATH);
  int pos = (strrchr(_appFilePath, '\\') - _appFilePath + 1);
  strncpy(_appPath, _appFilePath, pos);
  strcpy(_cfgFilePath, _appPath);
  strcat(_cfgFilePath, APP_CFG_FILE);
  ReadCfgFile(_cfgFilePath);

  //main window class definition
  LPCTSTR MainWndClass = TEXT(APP_VERSION);
  WNDCLASSEX wc;
  wc.cbSize        = sizeof(wc);
  wc.style         = 0;
  wc.lpfnWndProc   = &MainWndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = _hAppInstance;
  wc.hIcon         = (HICON) LoadImage(_hAppInstance, MAKEINTRESOURCE(IDI_MOUSE_ON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
  wc.hCursor       = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
  wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  wc.lpszClassName = MainWndClass;
  wc.hIconSm       = (HICON) LoadImage(_hAppInstance, MAKEINTRESOURCE(IDI_MOUSE_ON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR | LR_SHARED);
  if (!RegisterClassEx(&wc))
  {
    MessageBoxA(NULL, "error: registering window class", APP_VERSION, MB_ICONERROR | MB_OK);
    return 0;
  }

  //create main window
  _hMainWnd = CreateWindowEx(0, MainWndClass, MainWndClass, WS_OVERLAPPEDWINDOW, 200, 200, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, _hAppInstance, NULL);
  if (!_hMainWnd)
  {
    MessageBoxA(NULL, "error: creating main window", APP_VERSION, MB_ICONERROR | MB_OK);
    return 0;
  }

  //add a text box for debug purposes
  _hTxtKeyboard = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 0, 0, OBJ_WIDTH, TXT_KB_HEIGHT, _hMainWnd, NULL, GetModuleHandle(NULL), NULL);
  _hTxtMsg = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 0, TXT_KB_HEIGHT, OBJ_WIDTH, TXT_DEBUG_HEIGHT, _hMainWnd, NULL, GetModuleHandle(NULL), NULL);
  if(!_hTxtKeyboard || !_hTxtMsg)
  {
    MessageBoxA(NULL, "error: creating text box", APP_VERSION, MB_OK | MB_ICONERROR);
    return 0;
  }

  //Hide the main window by default
  //ShowWindow(_hMainWnd, nCmdShow);
  //UpdateWindow(_hMainWnd);
  ShowWindow(_hMainWnd, SW_HIDE);

  //Set Tray Notification Area Icon
  int numLockState = (GetKeyState(VK_NUMLOCK) == 1) ? 1 : 0; //Num Lock
  if (numLockState) SetTNAIcon(1, IDI_MOUSE_OFF);
  else SetTNAIcon(1, IDI_MOUSE_ON);

  //main message loop
  MSG mainMsg;
  while(GetMessage(&mainMsg, NULL, 0, 0) > 0)
  {
    DispatchMessage(&mainMsg);
  }

  return (int) mainMsg.wParam;
}
