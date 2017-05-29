/* 
** $Id: mginit.c 288 2009-10-14 07:36:33Z hejia $
**
** Listing 31.1
**
** mginit.c: Sample program for MiniGUI Programming Guide
**      A simple mginit program.
**
** Copyright (C) 2003 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
 
#define SFKBD_GETSTATE 3
#define SFKBD_REQID (MAX_SYS_REQID + 9)
#define SENCE_DESKTOP 0

#include "mgi.h"
#include "mgphonetest.h"

//extern HWND GBIMEWindowEx (HWND hosting, int lx, int ty, int rx, int by, BOOL two_lines);
extern HWND SK_GBIMEWindow (HWND hosting);
static BOOL quit = FALSE;
static GHANDLE ime_handle;
static HWND hSoftKeypad;

static void procs_sfk(BOOL skbd_is_shown)
{

}

static void on_new_del_client (int op, int cli)
{       
    static int nr_clients = 0;

    if (op == LCO_NEW_CLIENT) {
        nr_clients ++;
    }   
    else if (op == LCO_DEL_CLIENT) {
        nr_clients --;
        if (nr_clients == 0) {
            printf ("There is no any client, I will quit.\n");
            quit = TRUE;
        }               
        else if (nr_clients < 0) {
            printf ("Serious error: nr_clients less than zero.\n");
        }
    }
    else
        printf ("Serious error: incorrect operations.\n");
}

static int sfkbd_handler (int cli, int clifd, void* buff, size_t len)
{
	int req = *(int*)buff;
	int ret;
	switch (req) {
		case SFKBD_GETSTATE:
			ret = SendMessage(hSoftKeypad, MSG_IME_GETSTATUS, 0, 0);
			break;
		case 0:
			ret = SendMessage(hSoftKeypad, MSG_IME_CLOSE, 1, 1);
			break;
		case 1:
			ret = SendMessage(hSoftKeypad, MSG_IME_OPEN, 1, 1);
			break;
	}

	return ServerSendReply (clifd, &ret, sizeof(int));
}

static pid_t exec_app (const char* file_name, const char* app_name)
{
    pid_t pid = 0;

    if ((pid = vfork ()) > 0) {
        fprintf (stderr, "new child, pid: %d.\n", pid);
    }
    else if (pid == 0) {
        execl (file_name, app_name, NULL);
        perror ("execl");
        _exit (1);
    }
    else {
        perror ("vfork");
    }

    return pid;
}

static void child_wait (int sig)
{
    int pid;
    int status;

    while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED (status))
            printf ("--pid=%d--status=%x--rc=%d---\n", pid, status, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf ("--pid=%d--signal=%d--\n", pid, WTERMSIG (status));
    }
}

static void my_ime_proc (HWND hwnd, int id, int nc, DWORD add_data)
{
    static int show_sk = 1, show_hw = 1;
    switch (nc) {
        case MGIN_SKIME:
            if (show_sk)
            {
                mgiSetActiveIMEWindow (ime_handle, "softkeyboard");
                printf( "sk\n" );
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
                printf( "hw\n" );
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
#define HEIGHT_TASKBAR 30
#define HEIGHT_IMEWIN 30
HWND create_ime (void)
{
    MAINWINCREATE CreateInfo;
    HWND hTaskBar;

    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW;
    CreateInfo.spCaption = "TaskBar" ;
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor (0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DefaultMainWinProc;//TaskBarWinProc;
    CreateInfo.lx = g_rcScr.left; 
    CreateInfo.ty = g_rcScr.bottom - HEIGHT_TASKBAR;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = PIXEL_yellow;//GetWindowElementColor (BKC_CONTROL_DEF); 
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hTaskBar = CreateMainWindow (&CreateInfo);

    GBIMEWindowEx (hTaskBar, 
                    g_rcScr.left, 
                    g_rcScr.bottom - HEIGHT_TASKBAR - HEIGHT_IMEWIN,
                    g_rcScr.right, g_rcScr.bottom - HEIGHT_TASKBAR, FALSE);
    return hTaskBar;
}

int MiniGUIMain (int args, const char* arg[])
{
    MSG msg;
    HWND sk_ime_hwnd=HWND_INVALID, hw_ime_hwnd=HWND_INVALID;
    struct sigaction siga;
    const char* app_name = arg[1];
    MGI_SOFTKEYBOARD_INFO sk_info;
    MGI_HANDWRITE_INFO hw_info;
    HWND phone_ime_hwnd;
    int ret;

    memset (&sk_info, 0, sizeof(MGI_SOFTKEYBOARD_INFO));
    memset (&hw_info, 0, sizeof(MGI_HANDWRITE_INFO));

    sk_info.nr_keys = 0;
    sk_info.kbd_layout = NULL;

    hw_info.w= 280;
    hw_info.h= 150;
    hw_info.two_wins = TRUE;
    
    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));
    sigaction (SIGCHLD, &siga, NULL);

    OnNewDelClient = on_new_del_client;

#if (MINIGUI_MAJOR_VERSION > 1)
    if (!ServerStartup (0, 0, 0)) {
        fprintf (stderr, "Can not start the server of MiniGUI-Lite: mginit.\n");
        return 1;
    }
#endif

#if 0
    if(!RegisterRequestHandler(SFKBD_REQID, sfkbd_handler))
    {
        fprintf(stderr, "Can not create soft key pad. \n");
        return 2;
    }

    if((hSoftKeypad = mgiCreateSoftKeypad(procs_sfk)) == HWND_INVALID)
    {
        fprintf(stderr, "Can not create soft key pad. \n");
        return 3;
    }
#endif

#if (MINIGUI_MAJOR_VERSION < 2)
    if (SetDesktopRect (0, g_rcScr.bottom, g_rcScr.right, g_rcScr.bottom + 10) == 0) {
        fprintf (stderr, "Empty desktop rect.\n");
        return 2;
    }
#endif

    if (NULL == app_name)
        app_name = "sktest";
    if (exec_app (app_name , app_name) == 0)
    {
        fprintf (stderr, "llllEmpty desktop rect.\n");
        mgiDestroyIMEContainer (ime_handle);
        return 3;
    }

    //if (!strcmp(app_name, "switchtest"))
        ime_handle = mgiCreateIMEContainer (3, TRUE );
    //else
    //    ime_handle = mgiCreateIMEContainer (1, FALSE);

    //phone_ime_hwnd = create_test_phone_ime_wnd();
    sk_ime_hwnd = mgiCreateSoftKeyboardIME (&sk_info, NULL);
    hw_ime_hwnd = GBIMEWindowEx( HWND_DESKTOP, 80, 480, 680, 530, FALSE );
   //sk_ime_hwnd = mgiCreateSoftKeypad(procs_sfk);
    //sk_ime_hwnd = create_ime(  );
    //hw_ime_hwnd = mgiCreateHandWriteIME (&hw_info, NULL);

   // ret = mgiAddIMEWindow (ime_handle, phone_ime_hwnd, "mgphone");
    ret = mgiAddIMEWindow (ime_handle, sk_ime_hwnd, "softkeyboard");
    ret = mgiAddIMEWindow (ime_handle, hw_ime_hwnd, "handwrite");

   // mgiAddSKBTranslateOps (sk_ime_hwnd, &quanpin_trans_ops);

    //if (!strcmp(app_name, "switchtest"))
        mgiSetNotificationCallback (ime_handle, my_ime_proc);


    //    mgiSetActiveIMEWindow (ime_handle, "handwrite");
   // if (!strcmp(app_name, "hwtest"))
   //     mgiSetActiveIMEWindow (ime_handle, "handwrite");
   // else if (!strcmp(app_name, "sktest"))
   // {
   //     mgiSetActiveIMEWindow (ime_handle, "softkeyboard");
   // }
   // else if (!strcmp(app_name, "phonetest"))
   //     mgiSetActiveIMEWindow (ime_handle, "mgphone");
    
#if 0 
    char tmp[64] ={0};
    int id = mgiPhoneKeyPadGetCurrMethod();
    mgiPhoneKeyPadGetMethodCode(tmp, 64, id);
    fprintf(stderr, "mginit, method code=%s\n", tmp);
#endif
    while (!quit && GetMessage (&msg, sk_ime_hwnd)) {
        TranslateMessage( &msg );
        DispatchMessage (&msg);
    }

    mgiDestroyIMEContainer (ime_handle);

    return 0;
}
#else
int main ()
{
    return 0;
}
#endif /* _LITE_VERSION */
