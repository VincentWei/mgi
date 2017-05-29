#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgi/mgi.h>

#define IDC_CHARS1       100
#define IDC_CHARS2       101 
#define IDC_CHAR1         102
#define IDC_CHAR2         103
#define IDC_CHAR3         104

#define IDC_GB2312          111
#define IDC_UTF8            112
#define IDC_BIG5            113
#define IDC_AUTOTRACK       114
#define IDC_NOTRACK         115

#define EDIT_NUM        5
static unsigned int EDITS_ID[EDIT_NUM] ={IDC_CHARS1, IDC_CHARS2,IDC_CHAR1, IDC_CHAR2, IDC_CHAR3}; 

#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__);
static PLOGFONT big5_font;
static PLOGFONT utf8_font;
static PLOGFONT gb2312_font;

static void show_ime_window(BOOL show);
void notify_ime_status(BOOL);
static HWND sk_ime_hwnd;
static HWND g_current_hwnd; 
static BOOL g_ime_opened = FALSE; 
static CTRLDATA CtrlInputChar [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE,
        10, 10, 380, 25, 
        IDC_STATIC,
        "EDITABLE",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE,
        10, 50, 380, 25, 
        IDC_STATIC,
        "PASSWORD",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE,
        10, 90, 380, 25, 
        IDC_STATIC,
        "READ ONLY",
        0
    }, 


    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        120, 10, 180, 25,
        IDC_CHAR1,
        NULL,
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_PASSWORD,
        120, 50, 180, 25,
        IDC_CHAR2,
        NULL,
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_READONLY,
        120, 90, 180, 25,
        IDC_CHAR3,
        NULL,
        0
    },

    {
        CTRL_MLEDIT,
        WS_VISIBLE | WS_VSCROLL 
            | ES_BASELINE | ES_AUTOWRAP | ES_NOHIDESEL | ES_NOHIDESEL,//| WS_DISABLED,
        0, 150, 155, 200, 
        IDC_CHARS1, 
        NULL,
        0 
    },
    {
        CTRL_MLEDIT,
        WS_VISIBLE | WS_VSCROLL 
            | ES_BASELINE | ES_AUTOWRAP | ES_NOHIDESEL | ES_NOHIDESEL,//| WS_DISABLED,
        165, 150, 155, 200, 
        IDC_CHARS2, 
        NULL,
        0 
    },

    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP, 
        20, 360, 80, 34,
        IDC_GB2312,
        "GB2312",
        0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        20, 400, 80, 34,
        IDC_UTF8,
        "UTF8",
        0
    },
    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        20, 440, 80, 34,
        IDC_BIG5,
        "BIG5",
        0
    },

    {
        CTRL_BUTTON,
        WS_VISIBLE | BS_AUTORADIOBUTTON | BS_CHECKED | WS_TABSTOP | WS_GROUP, 
        200, 380, 90, 34,
        IDC_AUTOTRACK,
        "AUTOTRACK",
        0
    },

    {
        CTRL_BUTTON,
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        200, 420, 90, 34,
        IDC_NOTRACK,
        "NOTRACK",
        0
    },
}; 

static DLGTEMPLATE DlgBoxInputChar =
{
    WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 320, 500, 
    "",
    0, 0,
    TABLESIZE(CtrlInputChar), NULL,
    0
}; 


static void btn_notif_proc (HWND hwnd, int id, int nc, DWORD add_data) 
{ 
    int i = 0;
    if (id == IDC_GB2312 && nc == BN_CLICKED) { 
        SetIMEStatus(IME_STATUS_ENCODING, IME_ENCODING_LOCAL); 
        for(i = 0; i < EDIT_NUM; ++i) {
            //printf("EDITS_ID[%d] = %d\n", i,  EDITS_ID[i]);
            //SetWindowText(GetDlgItem(GetParent(hwnd), EDITS_ID[i]), NULL);
            SetWindowFont(GetDlgItem(GetParent(hwnd), EDITS_ID[i]),gb2312_font);
        }
    } else if (id == IDC_BIG5 && nc == BN_CLICKED) { 
        //printf("big5 btn clicked\n");
        SetIMEStatus(IME_STATUS_ENCODING, IME_ENCODING_LOCAL); 
        for(i = 0; i < EDIT_NUM; ++i) {
            //SetWindowText(GetDlgItem(GetParent(hwnd), EDITS_ID[i]),NULL);
            SetWindowFont(GetDlgItem(GetParent(hwnd), EDITS_ID[i]), big5_font);
        }
    } else if (id == IDC_UTF8 && nc == BN_CLICKED) { 
        //printf("UTF8 btn clicked\n");
        SetIMEStatus(IME_STATUS_ENCODING, IME_ENCODING_UTF8); 
        for(i = 0; i < EDIT_NUM; ++i) {
            //SetWindowText(GetDlgItem(GetParent(hwnd), EDITS_ID[i]),NULL);
            SetWindowFont(GetDlgItem(GetParent(hwnd), EDITS_ID[i]), utf8_font);
        }
    } else if (id == IDC_AUTOTRACK && nc == BN_CLICKED) { 
        SetIMEStatus(IME_STATUS_AUTOTRACK, TRUE); 
    } else if (id == IDC_NOTRACK && nc == BN_CLICKED) { 
        SetIMEStatus(IME_STATUS_AUTOTRACK, FALSE); 
    } 
}       

static int InputCharDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HWND hwnd; 
    g_current_hwnd = hDlg;
    switch (message) {
    case MSG_INITDIALOG:
        SetNotificationCallback (GetDlgItem (hDlg, IDC_GB2312), btn_notif_proc);
        SetNotificationCallback (GetDlgItem (hDlg, IDC_UTF8), btn_notif_proc);
        SetNotificationCallback (GetDlgItem (hDlg, IDC_BIG5), btn_notif_proc);
        SetNotificationCallback (GetDlgItem (hDlg, IDC_AUTOTRACK), btn_notif_proc);
        SetNotificationCallback (GetDlgItem (hDlg, IDC_NOTRACK), btn_notif_proc);

        utf8_font = CreateLogFontByName("upf-fmsong-rrncnn-12-12-UTF-8");
        gb2312_font = CreateLogFontByName("upf-fmsong-rrncnn-12-12-GB2312");
        big5_font = CreateLogFontByName("upf-fmsong-rrncnn-12-12-BIG5");
        //fprintf(stderr, "name is %s.\n", utf8_font->mbc_devfont->name);
        if(NULL == utf8_font || NULL == gb2312_font || NULL == big5_font) {
            printf("CreateLogFontByName Failed\n");
        }

        int i = 0;
        for(i = 0; i < EDIT_NUM; ++i) {
            SetWindowFont(GetDlgItem(hDlg, EDITS_ID[i]),gb2312_font);
        }

        sk_ime_hwnd = mgiCreateSoftKeypad(notify_ime_status); 
        mgiEnableSoftKeypad(TRUE);
        SetFocusChild(GetDlgItem(hDlg, IDC_CHAR1));
        SetIMEStatus(IME_STATUS_AUTOTRACK, TRUE); 
        SetIMEStatus(IME_STATUS_ENCODING,IME_ENCODING_LOCAL); 
        return 1; 
    case MSG_CLOSE:
        DestroyLogFont(utf8_font);
        DestroyLogFont(gb2312_font);
        DestroyLogFont(big5_font); 
        PostMessage (GetParent(hDlg), MSG_CLOSE, 0, 0); 
        EndDialog (hDlg, IDCANCEL); 
        break;
    } 
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
void notify_ime_status(BOOL opened)
{
    g_ime_opened = opened;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "mgi_edit" , 0 , 0);
#endif 

#ifndef _MGRM_THREADS
    if (!InitVectorialFonts ()) {
        printf ("InitVectorialFonts: error.\n");
        return 1;
    }
#endif

    DlgBoxInputChar.controls = CtrlInputChar;
    DialogBoxIndirectParam (&DlgBoxInputChar, HWND_DESKTOP, InputCharDialogBoxProc, 0L);

#ifndef _MGRM_THREADS
    TermVectorialFonts ();
#endif
    return 0;
} 
#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
