#include "performaction.h"
HWND _hWndHighlight; //Pointer to highlight window
HWND _hWndGrid; //Pointer to grid window

//
// Starts an application
bool StartApplication(char* progPath)
{
  STARTUPINFO si = { sizeof(si) };
  PROCESS_INFORMATION  pi;
  wchar_t wFilePath[MAX_PATH];

  mbstowcs(wFilePath, progPath, strlen(progPath) + 1);

  //--- pro debug
  // char buffer[MAX_PATH*2];
  // sprintf(buffer,"#%s# %d", progPath, strlen(progPath));
  // SetMsg(NULL, buffer);
  //--- 
  
  //startup info
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOWNORMAL; 
  
  //if(CreateProcess(0, (LPWSTR)wFilePath, 0, 0, FALSE, 0, 0, (LPWSTR)wPath, &si, &pi))
  if(CreateProcess(0, (LPWSTR)wFilePath, 0, 0, FALSE, 0, 0, 0, &si, &pi))
  {
     //WaitForSingleObject(pi.hProcess, INFINITE);
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);
     return true;
  }
  
  return false;
}

//
// Restituisce una finestra figlia...
HWND GetRecursiveWindowFromExStyle(HWND hWndParent, int lExStyle)
{
  long lExStyleChild = GetWindowLong(hWndParent, GWL_EXSTYLE);

  if ((lExStyleChild & lExStyle) != 0) return hWndParent; //finestra trovata

  //si cerca la finestra nei figli...
  HWND hWndChild = GetWindow(hWndParent, GW_CHILD);
  while (hWndChild != 0)
  {
      HWND res = GetRecursiveWindowFromExStyle(hWndChild, lExStyle);
      if (res != 0) return res;
     
      //si cerca il fratello
      hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
  }
    
  return 0;
}

//
// Restituisce una finestra figlia...
HWND GetRecursiveWindowFromClass(HWND hWndParent, char* lpClassName)
{
  const int MAXBUFLEN = 100;

  char lpStr[MAXBUFLEN - 1];
  GetClassNameA(hWndParent, lpStr, MAXBUFLEN);
    
  if (strcmp(lpStr, lpClassName) == 0) return hWndParent; //finestra trovata

  //si cerca la finestra nei figli...
  HWND hWndChild = GetWindow(hWndParent, GW_CHILD);
  while (hWndChild != 0)
  {
      HWND res = GetRecursiveWindowFromClass(hWndChild, lpClassName);
      if (res != 0) return res;
     
      //si cerca il fratello
      hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
  }

  return 0;
}

//
// Cause a mouse movement
void MoveMouse(int action, bool smallMove)
{
  POINT lpPoint;
  GetCursorPos(&lpPoint);
  int pixels = smallMove ? MOUSE_MOVE_SMALL : MOUSE_MOVE_BIG;

  switch(action)
  {
    case ACT_MOUSE_MOVE_N: //north
      lpPoint.y = lpPoint.y - pixels;
      break;

    case ACT_MOUSE_MOVE_NE: //north-east
      lpPoint.x = lpPoint.x + pixels;
      lpPoint.y = lpPoint.y - pixels;
      break;

    case ACT_MOUSE_MOVE_E: //east
      lpPoint.x = lpPoint.x + pixels;
      break;

    case ACT_MOUSE_MOVE_SE: //south-east
      lpPoint.x = lpPoint.x + pixels;
      lpPoint.y = lpPoint.y + pixels;
      break;

    case ACT_MOUSE_MOVE_S: //south
      lpPoint.y = lpPoint.y + pixels;
      break;

    case ACT_MOUSE_MOVE_SW: //south-west
      lpPoint.x = lpPoint.x - pixels;
      lpPoint.y = lpPoint.y + pixels;
      break;

    case ACT_MOUSE_MOVE_W: //west
      lpPoint.x = lpPoint.x - pixels;
      break;

    case ACT_MOUSE_MOVE_NW: //north-west
      lpPoint.x = lpPoint.x - pixels;
      lpPoint.y = lpPoint.y - pixels;
      break;
      
    case ACT_MOUSE_MOVE_CENTER: 
      //nothing to do
      break;

  }

  SetCursorPos(lpPoint.x, lpPoint.y);
}

//
// Move current point in a specific area
POINT MovePointToArea(int area, POINT lpPoint, long areaWidth, long areaHeight)
{
  switch(area)
  {
    case ACT_MOUSE_MOVE_NW: //north-west
      lpPoint.x -= areaWidth;
      lpPoint.y -= areaHeight;
      break;
      
    case ACT_MOUSE_MOVE_N: //north
      lpPoint.y -= areaHeight;
      break;

    case ACT_MOUSE_MOVE_NE: //north-east
      lpPoint.x += areaWidth;
      lpPoint.y -= areaHeight;
      break;
      
    case ACT_MOUSE_MOVE_W: //west
      lpPoint.x -= areaWidth;
      break;
      
    case ACT_MOUSE_MOVE_CENTER: //center
      //nothing to do cause the lpPoint is assumed as the center
      break;

    case ACT_MOUSE_MOVE_E: //east
      lpPoint.x += areaWidth;
      break;

    case ACT_MOUSE_MOVE_SW: //south-west
      lpPoint.x -= areaWidth;
      lpPoint.y += areaHeight;
      break;

    case ACT_MOUSE_MOVE_S: //south
      lpPoint.y += areaHeight;
      break;

    case ACT_MOUSE_MOVE_SE: //south-east
      lpPoint.x += areaWidth;
      lpPoint.y += areaHeight;
      break;
  }

  return lpPoint;
}

//////
////// %%
////void GetMouseArea(HWND hWnd, long &areaWidth, long &areaHeight)
////{
////  GetWindowRect(hWnd, &wRect);
////  areaWidth = (wRect.right - wRect.left) / 3;
////  areaHeight = (wRect.bottom - wRect.top) / 3;
////}

//
// Cause the mouse to move in a specific screen area (desktop or window based)
void SetMouseArea(int area, bool desktopBased)
{
  HWND hWnd;
  RECT wRect;
  POINT lpPoint;
  long areaWidth, areaHeight;
  
  hWnd = GetForegroundWindow(); //active window based
  if (desktopBased || hWnd == NULL) hWnd = GetDesktopWindow(); //desktop based

  GetWindowRect(hWnd, &wRect);
  areaWidth = (wRect.right - wRect.left) / 3;
  areaHeight = (wRect.bottom - wRect.top) / 3;
  lpPoint.x = wRect.left + (wRect.right - wRect.left) / 2;
  lpPoint.y = wRect.top + (wRect.bottom - wRect.top) / 2;

  lpPoint = MovePointToArea(area, lpPoint, areaWidth, areaHeight);
  SetCursorPos(lpPoint.x, lpPoint.y);
}

//
// Set the mouse position
void SetMousePosition(int action, bool areaMode, bool smallOrDesktop)
{
  if (areaMode) SetMouseArea(action, smallOrDesktop); //desktop (or active window) based
  else MoveMouse(action, smallOrDesktop); //small (or big) move
}

//
// Main window callbacks manager
LRESULT CALLBACK HighLightWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

//
// Set the transparent level on a window
void SetTrasparentLevel(HWND hWnd, int level)
{
  long msg;
  if (level < 0 || level > 255) return;

  msg = GetWindowLong(hWnd, GWL_EXSTYLE);
  msg = msg | WS_EX_LAYERED;
  SetWindowLong(hWnd, GWL_EXSTYLE, msg);
  SetLayeredWindowAttributes(hWnd, 0, level, LWA_ALPHA);
}

//
// Highlights the mouse pointer
void HighlightMousePointer(bool show)
{
  POINT lpPoint;
  int radius = 25;
  
  GetCursorPos(&lpPoint);

  LPCTSTR highLightWndClass = TEXT("HighlightMouseClass");
  if (!show) 
  {
    if (_hWndHighlight != NULL) 
    {
      DestroyWindow(_hWndHighlight);
      UnregisterClass(highLightWndClass, _hAppInstance);
      _hWndHighlight = NULL;
    }
    return;
  }
  if (_hWndHighlight != NULL) return;

  WNDCLASSEX wc = {0};
  wc.cbSize        = sizeof(wc);
  wc.lpfnWndProc   = &DefWindowProc;
  wc.hInstance     = _hAppInstance;
  wc.hCursor       = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
  wc.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0xFF, 0x80, 0x80)));
  wc.lpszClassName = highLightWndClass;
  if (!RegisterClassEx(&wc))
  {
    //pro-debug: MessageBoxA(NULL, "error: registering window class", APP_VERSION, MB_ICONERROR | MB_OK);
    return;
  }
  
  _hWndHighlight = CreateWindowEx(WS_EX_TOOLWINDOW, highLightWndClass, TEXT(""), WS_BORDER, lpPoint.x - radius, lpPoint.y - radius, radius * 2, radius * 2, NULL, NULL, _hAppInstance, NULL);
  if (!_hWndHighlight)
  {
    //pro-debug: MessageBoxA(NULL, "error: creating main window", APP_VERSION, MB_ICONERROR | MB_OK);
    return;
  }
  
  SetWindowLong(_hWndHighlight, GWL_STYLE, 0);
  SetTrasparentLevel(_hWndHighlight, 100);
  ShowWindow(_hWndHighlight, SW_SHOWNOACTIVATE);
  SetWindowPos(_hWndHighlight, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|0x10);
}

//
// Draws a grid on the screen
void ShowWindowGrid(bool desktopBased, bool show)
{
  HWND hWnd;
  RECT wRect, cRect, dRect;
  wchar_t message[256];
  
  hWnd = GetForegroundWindow(); //active window based
  if (desktopBased || hWnd == NULL) hWnd = GetDesktopWindow(); //desktop based

  LPCTSTR winGridWndClass = TEXT("WinGridClass");

  //-- destroy window
  if (!show)
  {
    if (_hWndGrid != NULL) 
    {
      DestroyWindow(_hWndGrid);
      UnregisterClass(winGridWndClass, _hAppInstance);
      _hWndGrid = NULL;
    }
    return;
  }
  if (_hWndGrid != NULL) return;

  //-- create window
  WNDCLASSEX wc = {0};
  wc.cbSize        = sizeof(wc);
  wc.lpfnWndProc   = &DefWindowProc;
  wc.hInstance     = _hAppInstance;
  wc.hCursor       = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
  wc.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0x80, 0x80, 0x80)));
  wc.lpszClassName = winGridWndClass;
  if (!RegisterClassEx(&wc))
  {
    //pro-debug: MessageBoxA(NULL, "error: registering window class", APP_VERSION, MB_ICONERROR | MB_OK);
    return;
  }

  GetWindowRect(hWnd, &wRect);
  GetClientRect(hWnd, &cRect);
  _hWndGrid = CreateWindowEx(WS_EX_TOOLWINDOW, winGridWndClass, TEXT(""), WS_BORDER, wRect.left, wRect.top, (wRect.right - wRect.left), (wRect.bottom - wRect.top), NULL, NULL, _hAppInstance, NULL);
  if (!_hWndGrid)
  {
    //pro-debug: MessageBoxA(NULL, "error: creating main window", APP_VERSION, MB_ICONERROR | MB_OK);
    return;
  }
  
  HDC hdc = GetDC(_hWndGrid);
  int lineDim = 5;
  long areaWidth, areaHeight;
  areaWidth = (wRect.right - wRect.left) / 3;
  areaHeight = (wRect.bottom - wRect.top) / 3;

  //%%%%%%%%
  SetWindowLong(_hWndGrid, GWL_STYLE, 0); 
  SetWindowLong(_hWndGrid, GWL_STYLE, WS_DLGFRAME); //%%%%%%%%
//  SetTrasparentLevel(_hWndGrid, 100);
  ShowWindow(_hWndGrid, SW_SHOWNOACTIVATE);
  SetWindowPos(_hWndGrid, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|0x10);
  
  SelectObject(hdc, GetStockObject(DC_BRUSH));
  SetDCBrushColor(hdc, RGB(0x80,0,0));

  //-- draw grid lines ("sbavano" un po' per questo motivo penso: http://stackoverflow.com/questions/7561049/what-is-the-difference-between-getclientrect-and-getwindowrect-in-winapi)
  //Rectangle(hdc, cRect.left + areaWidth, cRect.top, cRect.left + areaWidth + lineDim, cRect.bottom); // |
  //Rectangle(hdc, cRect.left + areaWidth * 2, cRect.top, cRect.left + areaWidth * 2 + lineDim, cRect.bottom); // |
  //Rectangle(hdc, cRect.left, cRect.top + areaHeight, cRect.right, cRect.top + areaHeight + lineDim); // -
  //Rectangle(hdc, cRect.left, cRect.top + areaHeight * 2, cRect.right, cRect.top + areaHeight * 2 + lineDim); // -
  Rectangle(hdc, areaWidth, 0, areaWidth + lineDim, wRect.bottom); // |
  Rectangle(hdc, areaWidth * 2, 0, areaWidth * 2 + lineDim, wRect.bottom); // |
  Rectangle(hdc, 0, areaHeight, wRect.right, areaHeight + lineDim); // -
  Rectangle(hdc, 0, areaHeight * 2, wRect.right, areaHeight * 2 + lineDim); // -
  
  //-- draw text
  HFONT font = CreateFont(20, 0, 0, 0, FW_LIGHT, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
  SelectObject(hdc, font);
  SetTextColor(hdc, RGB(0x0, 0x0, 0x0));
  SetBkMode(hdc,TRANSPARENT);
  
  dRect.top = -areaHeight / 2;
  for(int r = 0; r < 3; r++)
  {
    dRect.top += areaHeight;
    dRect.left = -areaWidth / 2;
    for(int c = 0; c < 3; c++)
    {
      dRect.left += areaWidth;
      
      int area = 7 - (3 * r) + c;
      wsprintfW(message, L"%d", area);
      DrawText(hdc, message, -1, &dRect, DT_SINGLELINE | DT_NOCLIP);
    }
  }

  ReleaseDC(_hWndGrid, hdc);
}

//
// Cause a mouse click
//
// type: 0: mouse down, 1: mouse up, 2: click
void MouseClick(int left, int type)
{
  POINT lpPoint;
  DWORD lButtonDown, lButtonUp;

  GetCursorPos(&lpPoint);
  if (left)
  {
    lButtonDown = MOUSEEVENTF_LEFTDOWN;
    lButtonUp = MOUSEEVENTF_LEFTUP;
  }
  else
  {
    lButtonDown = MOUSEEVENTF_RIGHTDOWN;
    lButtonUp = MOUSEEVENTF_RIGHTUP;
  }

  switch(type)
  {
    case ACT_MOUSE_DOWN:
      mouse_event(lButtonDown, lpPoint.x, lpPoint.y, 0, 0);
      break;

    case ACT_MOUSE_UP:
      mouse_event(lButtonUp, lpPoint.x, lpPoint.y, 0, 0);
      break;
    
    case ACT_MOUSE_CLICK:
      mouse_event(lButtonDown, lpPoint.x, lpPoint.y, 0, 0);
      Sleep(10);
      mouse_event(lButtonUp, lpPoint.x, lpPoint.y, 0, 0);
      break;
  }    
}

//
// Performs an action on the active window
void WindowAction(int action)
{
  long lStyle;
  HWND hWndMDI, hWndMDIClient;
  HWND hUserWnd = GetForegroundWindow();
  if (hUserWnd == 0) return;

  switch(action)
  {
    case ACT_WIN_CLOSE:
      SendMessage(hUserWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
      break;

    case ACT_WIN_MINIMIZE:
      ShowWindow(hUserWnd, SW_SHOWMINIMIZED);
      break;
      
    case ACT_WIN_MDIMAXIMIZE:
      hWndMDI = GetRecursiveWindowFromExStyle(hUserWnd, WS_EX_MDICHILD);
      hWndMDIClient = GetRecursiveWindowFromClass(hUserWnd, "MDIClient");
      if (hWndMDI == NULL || hWndMDIClient == NULL) break;
      SendMessage(hWndMDIClient, WM_MDIMAXIMIZE, (WPARAM)hWndMDI, 0);
      break;

    case ACT_WIN_MAXIMIZE:
      lStyle = GetWindowLong(hUserWnd, GWL_STYLE);
      if ((lStyle & WS_MAXIMIZE) != 0) ShowWindow(hUserWnd, SW_SHOWNORMAL);
      else ShowWindow(hUserWnd, SW_SHOWMAXIMIZED);
      break;
  }
}