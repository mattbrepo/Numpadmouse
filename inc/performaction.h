#ifndef PERFORMACTION_H
#define PERFORMACTION_H

#include "numpadmouse.h" //for SetMsg

bool StartApplication(char* progPath);
void HighlightMousePointer(bool show);
void ShowWindowGrid(bool desktopBased, bool show);

#define MOUSE_MOVE_SMALL 5
#define MOUSE_MOVE_BIG 35

#define ACT_MOUSE_MOVE_N				0
#define ACT_MOUSE_MOVE_NE				1
#define ACT_MOUSE_MOVE_E				2
#define ACT_MOUSE_MOVE_SE				3
#define ACT_MOUSE_MOVE_S				4
#define ACT_MOUSE_MOVE_SW				5
#define ACT_MOUSE_MOVE_W				6
#define ACT_MOUSE_MOVE_NW				7
#define ACT_MOUSE_MOVE_CENTER		8

void SetMousePosition(int action, bool areaMode, bool smallOrDesktop);

#define ACT_MOUSE_DOWN	0
#define ACT_MOUSE_UP		1
#define ACT_MOUSE_CLICK	2

void MouseClick(int left, int type);

#define ACT_WIN_CLOSE					0
#define ACT_WIN_MINIMIZE			1
#define ACT_WIN_MDIMAXIMIZE   2
#define ACT_WIN_MAXIMIZE			3

void WindowAction(int action);

#endif