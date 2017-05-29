/**
 * \file mgime.c
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: mgime.c 282 2009-09-22 10:16:42Z hejia $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/mgi.h"
#include "mgi_helper.h"

#include "contbmp.c"

#define IDC_CONT_NTB    100

#define IDC_NTB_LOGO    1
#define IDC_NTB_SKIME   2
#define IDC_NTB_HWIME   3

#define MSI_ADDWINDOW   (MSG_USER + 100)
#define MSI_DELWINDOW   (MSG_USER + 101)

#define CONT_WIN_WIDTH      70
#define CONT_WIN_HEIGHT     (20-2)

static IME_CONTAINER* ime_cont = NULL;
static BITMAP cont_ntb_bmp;

static void ntb_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    switch (nc) {
        case IDC_NTB_SKIME:
            NotifyParent (GetParent(hwnd), id, MGIN_SKIME);
            break;
        case IDC_NTB_HWIME:
            NotifyParent (GetParent(hwnd), id, MGIN_HWIME);
            break;
        default:
            break;
    };
}

static int create_cont_toolbar (HWND hwnd)
{
    HWND ntb;
    NTBINFO ntb_info;
    NTBITEMINFO ntbii;
    gal_pixel pixel;
    int i;

    ntb_info.nr_cells = 3;
    ntb_info.w_cell  = 0;
    ntb_info.h_cell  = 0;
    ntb_info.nr_cols = 0;
    if (LoadBitmapFromMem (HDC_SCREEN, &cont_ntb_bmp,
                       cont_bmp, sizeof(cont_bmp), "gif") < 0)
        return -1;
    ntb_info.image = &cont_ntb_bmp;

    ntb = CreateWindow (CTRL_NEWTOOLBAR,
                    "",
                    WS_CHILD | WS_VISIBLE, 
                    IDC_CONT_NTB,
                    1, 0, CONT_WIN_WIDTH - 2, 20,
                    hwnd,
                    (DWORD) &ntb_info);
    pixel = GetPixelInBitmap (ntb_info.image, 0, 0);
    SetWindowBkColor (ntb, pixel);
    InvalidateRect (ntb, NULL, TRUE);

    for (i = 0; i < 3; i++)
    {
        memset (&ntbii, 0, sizeof (ntbii));
        ntbii.flags = NTBIF_PUSHBUTTON;
        ntbii.id = IDC_NTB_LOGO + i;
        ntbii.bmp_cell = i;
        ntbii.text = "";
        SendMessage (ntb, NTBM_ADDITEM, 0, (LPARAM)&ntbii);
    }

    SetNotificationCallback (ntb, ntb_proc);
    return 0;
}

static void draw_border (HWND hwnd, HDC hdc)
{
    SetPenColor (hdc, COLOR_darkgray);

    LineEx (hdc, 1, 0, 1, CONT_WIN_HEIGHT);
    LineEx (hdc, CONT_WIN_WIDTH-1, 0, CONT_WIN_WIDTH-1, CONT_WIN_HEIGHT);
}

int mgi_container_win_proc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_CREATE:
            create_cont_toolbar (hwnd);
            break;
        case MSG_SETFOCUS:
        case MSG_KILLFOCUS:
            return 0;
        case MSG_PAINT:
            hdc = BeginPaint (hwnd);
            draw_border (hwnd, hdc);
            EndPaint (hwnd, hdc);
            break;
    };
    return DefaultMainWinProc (hwnd, message, wParam, lParam);
}

#ifdef _LITE_VERSION
HWND create_container_win ()
{
    MAINWINCREATE CreateInfo;
    HWND cont_hwnd;

    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;

    CreateInfo.MainWindowProc = mgi_container_win_proc;
    CreateInfo.lx = g_rcScr.right-CONT_WIN_WIDTH-20;
    CreateInfo.rx = g_rcScr.right-20;
    CreateInfo.ty = 0;
    CreateInfo.by = CONT_WIN_HEIGHT;
    CreateInfo.iBkColor = GetWindowElementColor(WE_THREED_BODY);
    CreateInfo.hHosting = HWND_DESKTOP;

    cont_hwnd = CreateMainWindow (&CreateInfo);

    return cont_hwnd;
}
#else
typedef struct _CONTAINER_INFO
{
    sem_t wait;
    HWND hwnd;
} CONTAINER_INFO;

static pthread_t cont_thread;

void* start_cont_win (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND cont_hwnd;
    CONTAINER_INFO* cont_info = (CONTAINER_INFO*)data;

    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;

    CreateInfo.MainWindowProc = mgi_container_win_proc;
    CreateInfo.lx = g_rcScr.right-CONT_WIN_WIDTH-20;
    CreateInfo.rx = g_rcScr.right-20;
    CreateInfo.ty = 0;
    CreateInfo.by = CONT_WIN_HEIGHT;
    CreateInfo.iBkColor = GetWindowElementColor(WE_THREED_BODY);
    CreateInfo.hHosting = HWND_DESKTOP;

    cont_hwnd = cont_info->hwnd = CreateMainWindow (&CreateInfo);
    sem_post (&cont_info->wait);

    if (cont_hwnd == HWND_INVALID)
        return NULL;

    while (GetMessage (&Msg, cont_hwnd) ) {
        TranslateMessage (&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (cont_hwnd);
    return NULL;

}

HWND create_container_win ()
{
    CONTAINER_INFO cont_info;
    pthread_attr_t new_attr;

    sem_init (&cont_info.wait, 0, 0);

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create (&cont_thread, &new_attr, start_cont_win, &cont_info);
    pthread_attr_destroy (&new_attr);

    sem_wait (&cont_info.wait);
    sem_destroy (&cont_info.wait);

    return cont_info.hwnd;
}
#endif

GHANDLE mgiCreateIMEContainer (int max_ime_wins, BOOL with_cont_win)
{
    int i;

    if (ime_cont != NULL)
    {
        fprintf (stderr, "The IME container already existed!\n");
        return 0;
    }
    ime_cont = malloc (sizeof(IME_CONTAINER));

    ime_cont->cont_hwnd = HWND_INVALID;
    ime_cont->max_ime_windows = max_ime_wins;
    ime_cont->cur_wins = 0;
    ime_cont->ime_wins = malloc (sizeof(IME_WIN)*max_ime_wins);

    for (i = 0; i < max_ime_wins; i++)
        ime_cont->ime_wins[i].ime_hwnd = HWND_INVALID;

    if (with_cont_win)
        ime_cont->cont_hwnd = create_container_win ();

    return (GHANDLE)ime_cont;
}

int mgiAddIMEWindow (GHANDLE ime, HWND ime_hwnd, const char* ime_name)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    int i;
    if (ime_cont == NULL)
        return IME_WINDOW_INVALID;
    if (ime_cont->cur_wins == ime_cont->max_ime_windows)
        return IME_WINDOW_FULL;

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (!strcmp (ime_cont->ime_wins[i].ime_name, ime_name))
            return IME_WINDOW_EXISTED;
    }

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (ime_cont->ime_wins[i].ime_hwnd == HWND_INVALID)
        {
            ime_cont->ime_wins[i].ime_hwnd = ime_hwnd;
            strncpy (ime_cont->ime_wins[i].ime_name, ime_name, MAX_IME_NAME);
            ime_cont->cur_wins ++;
            return IME_WINDOW_SUCCESS;
        }
    }

    return IME_WINDOW_FULL;
}

BOOL mgiRemoveIMEWindow (GHANDLE ime, const char* ime_name)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    int i;
    if (ime_cont == NULL || ime_cont->cur_wins == 0)
        return FALSE;

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (!strcmp(ime_cont->ime_wins[i].ime_name, ime_name))
        {
            ime_cont->ime_wins[i].ime_hwnd = HWND_INVALID;
            ime_cont->ime_wins[i].ime_name[0] = '\0';
            return TRUE;
        }
    }

    return FALSE;
}

BOOL mgiSetActiveIMEWindow (GHANDLE ime, const char* ime_name)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    HWND ime_hwnd = HWND_INVALID;
    int i, found = 0;

    if (ime_cont == NULL || ime_cont->cur_wins == 0)
        return FALSE;

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (!found && !strcmp(ime_cont->ime_wins[i].ime_name, ime_name))
        {
            ime_hwnd = ime_cont->ime_wins[i].ime_hwnd;
            found = 1;
        } else {
            UnregisterIMEWindow (ime_cont->ime_wins[i].ime_hwnd);
            ShowWindow (ime_cont->ime_wins[i].ime_hwnd, SW_HIDE);
        }
    }

    if (found && ime_hwnd != HWND_INVALID)
    {
        int ret_value;

        ret_value = RegisterIMEWindow (ime_hwnd);

        ShowWindow (ime_hwnd, SW_SHOW);
        return TRUE;
    }
    return FALSE;
}

BOOL mgiDestroyIMEContainer (GHANDLE ime)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    int i;

    if (ime_cont == NULL)
        return FALSE;

    if (ime_cont->cont_hwnd != HWND_INVALID)
	DestroyMainWindow (ime_cont->cont_hwnd);

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
	if (ime_cont->ime_wins[i].ime_hwnd != HWND_INVALID)
	    DestroyMainWindow (ime_cont->ime_wins[i].ime_hwnd);
    }
    
    free (ime_cont->ime_wins);
    free (ime_cont);
    ime_cont = NULL;

    return TRUE;
}

NOTIFPROC mgiSetNotificationCallback (GHANDLE ime, NOTIFPROC notif_proc)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    if (ime_cont == NULL || ime_cont->cont_hwnd == HWND_INVALID)
        return NULL;
    return SetNotificationCallback (ime_cont->cont_hwnd, notif_proc);
}
