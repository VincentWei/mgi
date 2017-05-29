/*
** $Id: switchtest.c 214 2008-08-15 04:27:32Z weiym $
**
** A simple program to test handwriting input method of mGi V1.3.
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#ifdef __MGI_LIB__
#include "mgi.h"
#else
#include <mgi/mgi.h>
#endif

int NoteBookWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    RECT client;

    switch (message) {
    case MSG_CREATE:
        GetClientRect(hWnd, &client);
        CreateWindow("textedit", 
                 "",  WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL,
                100, 0, 0, client.right,client.bottom , hWnd, 0);
        break;

    case MSG_SHOWWINDOW:
        SetFocus (GetDlgItem (hWnd, 100));
        return 0;

    case MSG_CLOSE:
        DestroyAllControls (hWnd);
        DestroyMainWindow (hWnd);
        PostQuitMessage (hWnd);
        return 0;
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

static void InitNoteBookInfo (PMAINWINCREATE pCreateInfo, HWND host)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER |
                        WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE | WS_SYSMENU;     
    pCreateInfo->dwExStyle = WS_EX_IMECOMPOSE;
    pCreateInfo->spCaption = "mGi test program";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = NoteBookWinProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 400;
    pCreateInfo->by = 300;
    pCreateInfo->iBkColor = COLOR_lightgray; 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = host;
}

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
GHANDLE ime_handle;

static void my_ime_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    static int show_sk = 1, show_hw = 1;
    switch (nc) {
        case MGIN_SKIME:
            if (show_sk)
            {
                mgiSetActiveIMEWindow (ime_handle, "softkeyboard");
                show_sk = 0;
                show_hw = 1;
            } else {
                mgiSetActiveIMEWindow (ime_handle, "");
                show_sk = 1;
            }
            break;
        case MGIN_HWIME:
            if (show_hw)
            {
                mgiSetActiveIMEWindow (ime_handle, "handwrite");
                show_hw = 0;
                show_sk = 1;
            } else {
                mgiSetActiveIMEWindow (ime_handle, "");
                show_hw = 1;
            }
            break;
        default:
            break;
    };
}
#endif

void* NoteBook (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
    HWND sk_ime_hwnd, hw_ime_hwnd;
    MGI_SOFTKEYBOARD_INFO sk_info;
    MGI_HANDWRITE_INFO hw_info;
    int ret;

    memset (&sk_info, 0, sizeof(MGI_SOFTKEYBOARD_INFO));
    memset (&hw_info, 0, sizeof(MGI_HANDWRITE_INFO));

    hw_info.x = g_rcScr.right - 300;
    hw_info.y = g_rcScr.bottom - 160;
    hw_info.w = 280;
    hw_info.h = 150;
    hw_info.two_wins = TRUE;

    ime_handle = mgiCreateIMEContainer (2, TRUE);
    sk_ime_hwnd = mgiCreateSoftKeyboardIME (&sk_info, NULL);
    hw_ime_hwnd = mgiCreateHandWriteIME (&hw_info, NULL);

    ret = mgiAddIMEWindow (ime_handle, sk_ime_hwnd, "softkeyboard");
    ret = mgiAddIMEWindow (ime_handle, hw_ime_hwnd, "handwrite");

    mgiAddSKBTranslateOps (sk_ime_hwnd, &quanpin_trans_ops);
    mgiSetNotificationCallback (ime_handle, my_ime_proc);
#endif

    InitNoteBookInfo (&CreateInfo, HWND_DESKTOP);
    hMainWnd = CreateMainWindow(&CreateInfo);

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    if (hMainWnd == HWND_INVALID)
    {
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
        mgiDestroyIMEContainer (ime_handle);
#endif
        return NULL;
    }

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(hMainWnd);

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
    mgiDestroyIMEContainer (ime_handle);
#endif

    return NULL;
}

int MiniGUIMain(int args, const char* arg[])
{

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
#if (MINIGUI_MAJOR_VERSION > 1)
    JoinLayer(NAME_DEF_LAYER , "notebook" , 0 , 0);
#else
    int i;
    const char* layer = NULL;
    RECT max_rect = {0, 0, 0, 0};

    for (i = 1; i < args; i++) {
        if (strcmp (arg[i], "-layer") == 0) {
            layer = arg[i + 1];
            break;
        }
    }
    GetLayerInfo (layer, &max_rect, NULL, NULL, NULL);
    if (JoinLayer (layer, arg[0], 
                max_rect.left, max_rect.top, 
                max_rect.left + 800, 
                max_rect.top + 600) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        exit (1);
    }
#endif
#endif

    NoteBook(NULL);
    return 0;
}

#ifndef _LITE_VERSION 
#include <minigui/dti.c>
#endif

