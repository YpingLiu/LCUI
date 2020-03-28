#include "libtest.h"
#include <stdlib.h>
#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/widget/button.h>
#include <LCUI/timer.h>
#include <LCUI/input.h>
#include <LCUI/display.h>

static void OnRefreshScreen(void *arg)
{
	LCUIDisplay_InvalidateArea(NULL);
}

static void OnQuit(void *arg)
{
	LCUI_Quit();
}

static void OnBtnClick(LCUI_Widget w, LCUI_WidgetEvent e, void *arg)
{
	LCUI_MainLoop loop;

	loop = LCUIMainLoop_New();
	LCUI_SetTimeout(10, OnRefreshScreen, NULL);
	LCUI_SetTimeout(50, OnQuit, NULL);
	LCUIMainLoop_Run(loop);
}

static void OnTriggerBtnClick(void *arg)
{
	LCUI_SysEventRec e;

	e.type = LCUI_MOUSEDOWN;
	e.button.button = LCUI_KEY_LEFTBUTTON;
	e.button.x = 5;
	e.button.y = 5;
	LCUI_TriggerEvent(&e, NULL);
}

static void ObserverThread(void *arg)
{
	LCUI_BOOL exited = *((LCUI_BOOL*)arg);

	LCUI_MSleep(100);
	it_b("main loop should exit after 50ms", exited, TRUE);
	if (!exited) {
		exit(-print_test_result());
		return;
	}
	LCUIThread_Exit(NULL);
}

void test_mainloop(void)
{
	static LCUI_BOOL exited;

	LCUI_Thread tid;
	LCUI_Widget root, btn;

	LCUI_Init();
	exited = FALSE;
	btn = LCUIWidget_New("button");
	root = LCUIWidget_GetRoot();
	Button_SetText(btn, "button");
	Widget_BindEvent(btn, "click", OnBtnClick, NULL, NULL);
	Widget_Append(root, btn);
	LCUIThread_Create(&tid, ObserverThread, &exited);
	LCUI_SetTimeout(100, OnTriggerBtnClick, btn);
	LCUI_Main();
	exited = TRUE;
	LCUIThread_Join(tid, NULL);
}
