#include "manageevent.h"

bool _mouseLeftDown = false;
bool _mouseRightDown = false;
bool _commandMode = false;
bool _areaMode = false;
bool _ctrlState = false;
bool _shiftState = false;
bool _altState = false;

//
// Manages current command
bool ManageCommand()
{
  for(int i = 0; i < _commandsMax; i++)
  {
    if (_commands[i].ctrlState != _ctrlState || _commands[i].shiftState != _shiftState || _commands[i].altState != _altState) continue;
    if (strcmp(_currentKeyMap, _commands[i].keyMap) != 0) continue;
    
    SetMsg(NULL, _commands[i].progPath);
    if (!StartApplication(_commands[i].progPath)) SetMsg(NULL, "failed");
    return true; //'true' cause the command was managed anyway!
  }
  
  return false;
}

//
// Manages all numpad events!
bool ManageEvent(WPARAM wParam, KBDLLHOOKSTRUCT* lParamKB)
{
  //vkCode ==> char conversion
  char keyChar = ' ';
  BYTE keyboardState[256];
  if (GetKeyboardState(keyboardState))
  {
     unsigned short charBuffer[2];
     if (ToAsciiEx(lParamKB->vkCode, lParamKB->scanCode, keyboardState, charBuffer, 0, GetKeyboardLayout(0)) == 1) keyChar = charBuffer[0];
  }

  //it's better this way:
  //bool ctrlState = (GetKeyState(VK_CONTROL) != 0) ? true : false; //CTRL
  //bool shiftState = (GetKeyState(VK_SHIFT) < 0) ? true : false; //SHIFT
  //bool altState = ((GetKeyState(VK_LMENU) < 0) || (GetKeyState(VK_RMENU) < 0)) ? true : false; //ALT, ALT-GR
  if (lParamKB->vkCode == VK_LCONTROL || lParamKB->vkCode == VK_RCONTROL) _ctrlState = (wParam == WM_KEYDOWN);
  if (lParamKB->vkCode == VK_LSHIFT || lParamKB->vkCode == VK_RSHIFT) _shiftState = (wParam == WM_KEYDOWN);
  if (lParamKB->vkCode == VK_LMENU || lParamKB->vkCode == VK_RMENU) _altState = (wParam == WM_KEYDOWN);
  
  //debug:
  char msg[100];
  sprintf(msg, "WPARAM: %d, vkCode: %d, scanCode: %d, flags: %d, char: %c, ctrl: %d, shift: %d, alt: %d", wParam, lParamKB->vkCode, lParamKB->scanCode, lParamKB->flags, keyChar, _ctrlState, _shiftState, _altState);
  SetMsg(msg, "");

  //----------------------------------------------------------------------------- Num Lock, VK_NUMLOCK, 144
  int numLockKey = GetKeyState(VK_NUMLOCK);
  int numLockState = (numLockKey == -127 || numLockKey == 1) ? 1 : 0; //Num Lock state
  if (wParam == WM_KEYUP && lParamKB->vkCode == VK_NUMLOCK && lParamKB->flags == 129)
  {
    _mouseLeftDown = false;
    _mouseRightDown = false;
    
    if (numLockState)
    {
      SetTNAIcon(0, IDI_MOUSE_OFF);
      SetMsg(NULL, "numpadmouse off");
    }
    else
    {
      SetTNAIcon(0, IDI_MOUSE_ON);
      SetMsg(NULL, "numpadmouse on");
    }
    return false; //pass event to OS
  }

  //all other functions are disabled when num lock is on!
  if (numLockState == 1) return false; //pass event to OS
  
  //============================================================================== command mode
  if (_commandMode)
  {
    if (wParam == WM_KEYDOWN) return true; //skips the KEYDOWN event
    
    if (lParamKB->vkCode == VK_ESCAPE || _currentKeyMapLength >= _longestKeyMap) 
    {
      _commandMode = false;
      //char buffer[100];
      //sprintf(buffer,"command mode off: key map too long: %d >= %d", _currentKeyMapLength, _longestKeyMap);
      //SetMsg(NULL, buffer);
      return true; //the KEYDOWN event is already gone anyway!
    }
    
    _currentKeyMap[_currentKeyMapLength++] = keyChar;
    _currentKeyMap[_currentKeyMapLength] = '\0';
    if (ManageCommand())
    {
      //SetMsg(NULL, "command mode off: command managed");
      _commandMode = false;
    }
    
    return true;
  }

  //----------------------------------------------------------------------------- 0, VK_INSERT, 45
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == VK_INSERT && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse left down");
    if (!_mouseLeftDown)
    {
      MouseClick(1, ACT_MOUSE_DOWN);
      _mouseLeftDown = true;
    }
    return true;
  }
  else
  {
    _mouseLeftDown = false;
  }

  if (wParam == WM_KEYUP && lParamKB->vkCode == VK_INSERT && lParamKB->flags == 128)
  {
    SetMsg(NULL, "mouse left up");
    MouseClick(1, ACT_MOUSE_UP);
    return true;
  }

  //----------------------------------------------------------------------------- ., VK_DELETE, 46
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == VK_DELETE && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse right down");
    if (!_mouseRightDown)
    {
      MouseClick(0, ACT_MOUSE_DOWN);
      _mouseRightDown = true;
    }
    return true;
  }
  else
  {
    _mouseRightDown = false;
  }

  if (wParam == WM_KEYUP && lParamKB->vkCode == VK_DELETE && lParamKB->flags == 128)
  {
    SetMsg(NULL, "mouse right up");
    MouseClick(0, ACT_MOUSE_UP);
    return true;
  }

  //----------------------------------------------------------------------------- 1
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 35 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move south-west");
    SetMousePosition(ACT_MOUSE_MOVE_SW, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- 2
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 40 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move south");
    SetMousePosition(ACT_MOUSE_MOVE_S, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- 3
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 34 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move south-east");
    SetMousePosition(ACT_MOUSE_MOVE_SE, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- 4
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 37 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move west");
    SetMousePosition(ACT_MOUSE_MOVE_W, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- 5
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 12 && lParamKB->flags == 0)
  {
    if (!_areaMode) 
    {
      SetMsg(NULL, "show highlight");
      HighlightMousePointer(true);
    }
    else SetMousePosition(ACT_MOUSE_MOVE_CENTER, _areaMode, _ctrlState);
    return true;
  }
  
  if (wParam == WM_KEYUP && lParamKB->vkCode == 12 && lParamKB->flags == 128)
  {
    if (!_areaMode) 
    {
      SetMsg(NULL, "hide highlight");
      HighlightMousePointer(false);
    }
    return true;
  }

  //----------------------------------------------------------------------------- 6
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 39 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move east");
    SetMousePosition(ACT_MOUSE_MOVE_E, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- 7
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 36 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move north-west");
    SetMousePosition(ACT_MOUSE_MOVE_NW, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- 8
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 38 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move north");
    SetMousePosition(ACT_MOUSE_MOVE_N, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- 9
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 33 && lParamKB->flags == 0)
  {
    SetMsg(NULL, "mouse move north-east");
    SetMousePosition(ACT_MOUSE_MOVE_NE, _areaMode, _ctrlState);
    return true;
  }

  //----------------------------------------------------------------------------- /, VK_DIVIDE, 111
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == VK_DIVIDE && lParamKB->flags == 1) return true;
  if (wParam == WM_KEYUP && lParamKB->vkCode == VK_DIVIDE && lParamKB->flags == 129)
  {
    SetMsg(NULL, "minimize win");
    WindowAction(ACT_WIN_MINIMIZE);
    return true;
  }

  //----------------------------------------------------------------------------- *, VK_MULTIPLY, 106
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == VK_MULTIPLY && lParamKB->flags == 0) return true;
  if (wParam == WM_KEYUP && lParamKB->vkCode == VK_MULTIPLY && lParamKB->flags == 128)
  {
    SetMsg(NULL, "maximize win");
    if (_ctrlState) WindowAction(ACT_WIN_MDIMAXIMIZE);
    else WindowAction(ACT_WIN_MAXIMIZE);
    return true;
  }

  //----------------------------------------------------------------------------- -, VK_SUBTRACT, 109
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == VK_SUBTRACT && lParamKB->flags == 0) return true;
  if (wParam == WM_KEYUP && lParamKB->vkCode == VK_SUBTRACT && lParamKB->flags == 128)
  {
    SetMsg(NULL, "close win");
    if (_ctrlState) DestroyWindow(_hMainWnd); //close numpadmouse
    else WindowAction(ACT_WIN_CLOSE); //close active window
    return true;
  }

  //----------------------------------------------------------------------------- +, VK_ADD, 107
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == VK_ADD && lParamKB->flags == 0)
  {
    SetMsg(NULL, "show grid");
    ShowWindowGrid(_ctrlState, true);
    _areaMode = true;
    return true;
  }
  if (wParam == WM_KEYUP && lParamKB->vkCode == VK_ADD && lParamKB->flags == 128) 
  {
    SetMsg(NULL, "hide grid");
    ShowWindowGrid(_ctrlState, false);
    _areaMode = false;
    return true;
  }
  
  //----------------------------------------------------------------------------- return
  if (wParam == WM_KEYDOWN && lParamKB->vkCode == 13 && lParamKB->flags == 1) return true;
  if (wParam == WM_KEYUP && lParamKB->vkCode == 13 && lParamKB->flags == 129)
  {
    if (_ctrlState)
    {
      //Show/Hide debug window
      if (IsWindowVisible(_hMainWnd)) ShowWindow(_hMainWnd, SW_HIDE);
      else
      {
        ShowWindow(_hMainWnd, SW_RESTORE);
        SetForegroundWindow(_hMainWnd);
      }
      _commandMode = false;
    }
    else if (_commandMode)
    {
      _commandMode = false;
      SetMsg(NULL, "command mode off");
    }
    else 
    {
      ReadCfgFile(_cfgFilePath);
      _commandMode = true;
      SetMsg(NULL, "command mode on");
      _currentKeyMapLength = 0;
    }
    
    return true;
  }

  return false; //pass event to OS
}
