/* 
** edit.c: Sample program mGpti (Client in MiniGUI-Processes).
**
** Copyright (C) 2007 Feynman Software.
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
#include "mgi-phone.h"
#else
#include <mgi/mgi.h>
#include <mgi/mgi-phone.h>
#endif


HWND phone_ime_hwnd;

//#define _TEST_MGPTI_ 1

#ifdef PHONE_SCREEN_WIDTH_128
#define WINDOW_W        128
#define EDIT_W          126
#define BUTTON_W        40
#endif

#ifdef PHONE_SCREEN_WIDTH_176
#define WINDOW_W        176
#define EDIT_W          174
#define BUTTON_W        60
#endif

#ifdef PHONE_SCREEN_WIDTH_240
#define WINDOW_W        240
#define EDIT_W          238 
#define BUTTON_W        80
#endif

static DLGTEMPLATE DlgBoxInputChar =
{
    WS_BORDER, 
    WS_EX_NONE,
    0, 0, WINDOW_W, 320, 
    "Please input letters",
    0, 0,
    10, NULL,
    0
};
#define IDC_ICON         99
#define IDC_MODE        100
#define IDC_SEND        110
#define IDC_CONT        120
#define IDC_OPTION      130
#define IDC_STATUS      140

static CTRLDATA CtrlInputChar [] =
{ 
    {
        CTRL_STATIC,
        WS_VISIBLE |SS_BITMAP,
        1, 1, 12, 13, 
        IDC_ICON, 
        ":Pti ",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE ,
        13, 1, 112, 16, 
        IDC_MODE, 
        ":Pti ",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE ,
        130, 1, 122, 16, 
        IDC_STATUS, 
        "",
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE ,
        1, 15, 122, 15, 
        IDC_STATIC, 
        "Send TO:",
        0
    },
    {
        CTRL_SLEDIT,
        WS_VISIBLE | WS_TABSTOP ,
        1, 30, EDIT_W, 20,
        IDC_SEND,
        NULL,
        0
    },
    {
        CTRL_STATIC,
        WS_VISIBLE ,
        1, 50, 122, 15, 
        IDC_STATIC, 
        "Content:",
        0
    },
    {
        CTRL_MLEDIT,
        WS_VISIBLE | WS_VSCROLL | ES_AUTOWRAP,
        1, 67, EDIT_W, 220, 
        IDC_CONT, 
        NULL,
        0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE , 
        2, 290, 40, 18,
        IDC_OPTION, 
        "Option",
        0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE , 
        2+BUTTON_W, 290, 40, 18,
        IDOK, 
        "OK",
        0
    },
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE, 
        2+2*BUTTON_W, 290, 40, 18,
        IDCANCEL,
        "Cancel",
        0
    },
};

#define MSG_PHONE_METHOD_SWITCH MSG_USER+13
#define MSG_PHONE_METHOD_STATUS MSG_USER+14

#if 0
static char pti_str[15] ="Mode: ";
static void mode2str (int mode, char* str)
{
    strcpy (str, pti_str);
    switch (mode)
    {
        case 1:
            strcat (str, "num"); 
            break;
        case 2:
            strcat (str, "ABC"); 
            break;
        case 3:
            strcat (str, "abc"); 
            break;
        case 100:
            strcat (str, "Pti"); 
            break;
        case 101:
            strcat (str, "Pinyin"); 
            break;
    }
}
#endif

static int InputCharDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:{
        int method_id=0;
        SendMessage(phone_ime_hwnd, MSG_IME_SETTARGET, (WPARAM)hDlg, 0);

        method_id = mgiPhoneKeyPadGetCurrMethod();
        fprintf(stderr, "cur method_id=%d\n", method_id);
        PostMessage(hDlg, MSG_PHONE_METHOD_SWITCH, method_id, 0);
        return 1;
    }
    case MSG_CLOSE:
        EndDialog (hDlg, IDCANCEL);
        break;
        
    case MSG_KEYDOWN:
    {
        break;
    }
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
            //_pti_client_open_pti_func (1);
            return 0;

        case IDCANCEL:
            //EndDialog (hDlg, wParam);
            break;
        }
        break;
    case MSG_PHONE_METHOD_SWITCH:{
        if (wParam > 0){
            char method_name[32];
            char tmp[32];

            memset(method_name, 0, sizeof(method_name));
            mgiPhoneKeyPadGetMethodCode(method_name, 32, wParam);
            sprintf(tmp, ": %s", (char *)method_name);
            //fprintf(stderr,"edit method_name=%s\n", tmp);
            SetWindowText(GetDlgItem(hDlg, IDC_MODE), tmp);

            if (strstr(method_name, "pti"))
                PostMessage(hDlg, MSG_PHONE_METHOD_STATUS, 0, 0);
            else
                SetWindowText(GetDlgItem(hDlg, IDC_STATUS), "");
        }
        break;
    }
    case MSG_PHONE_METHOD_STATUS: {
        if (wParam > 0){
            char tmp[32];
            switch(wParam) {
                case 0x03:
                    sprintf(tmp, "status:%s", "abc");
                    break;
                case 0x02:
                    sprintf(tmp, "status:%s", "Abc");
                    break;
                case 0x01:
                    sprintf(tmp, "status:%s", "ABC");
                    break;
                default:
                    tmp[0]='\0';
            }
            //fprintf(stderr,"edit method_name=%s\n", tmp);
            SetWindowText(GetDlgItem(hDlg, IDC_STATUS), tmp);
        }
        break;
    }
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

#if _TEST_MGPTI_ 
static char* key_stokes [] = {
    "89",
    "245",
    "689",
    "96626",
    "ac*",
    "tw*",
    "ba*",
    "22",
    "23",
    "262",
};

static void mgpti_test_matching (void)
{
    int i;
    char buff [PTI_RESULT_BUFF_LONG];
    int cursor;

    for (i = 0; i < sizeof (key_stokes) / sizeof (char*); i++) {
        printf ("matching: %s...\n", key_stokes [i]);

        cursor = 0;
        while (cursor != -1) {
            /* the candidates will be stored in buff */
            cursor = _pti_client_match_keystokes (key_stokes [i], buff, 
                cursor, PTI_CASE_Abc);

            printf ("    BUFF: \"%s\"\n", buff);
        }

        printf ("done.\n");
    }
}

static char* user_words [] = {
    "ab",
    "abs",
    "abt",
    "abu",
    "aby",
    "abw",
    "asdkfasdf",
    "cas",
    "casdkfaskdfk",
    "caddice",
    "card",
    "careen",
    "caref",
    "zy",
    "zzz",
    "user",
    "ur",
    "chin",
    "win",
    "upd",
    "windo",
    "da",
    "vin",
    "ci",
    "Vincent",
    "Tsinghua"
};

static void mgpti_test_add_word (void)
{
    int i;

    for (i = 0; i < sizeof (user_words) / sizeof (char*); i++) {
        _pti_client_add_user_word (user_words [i]);
    }
}
#endif

extern HWND create_test_phone_ime_wnd();

int MiniGUIMain (int argc, const char* argv[])
{

extern      BITMAP pen_bmp;
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
    HWND ime_handle;
#endif

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "edit" , 0 , 0);
#endif

#if _TEST_MGPTI_ 
    mgpti_test_add_word ();
    mgpti_test_matching ();
#endif

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
    ime_handle = mgiCreateIMEContainer (2, FALSE);
    phone_ime_hwnd = create_test_phone_ime_wnd();

    mgiAddIMEWindow (ime_handle, phone_ime_hwnd, "mgphone");
    mgiSetActiveIMEWindow (ime_handle, "mgphone");
#endif

  //   LoadBitmapFromFile (HDC_SCREEN, &pen, "pen.bmp");
    pen_bmp.bmType=BMP_TYPE_COLORKEY;
    pen_bmp.bmColorKey = GetPixelInBitmap (&pen_bmp, 0, 0);
    CtrlInputChar [0].dwAddData = (int)&pen_bmp ;

    DlgBoxInputChar.controls = CtrlInputChar;
    DialogBoxIndirectParam (&DlgBoxInputChar, HWND_DESKTOP, InputCharDialogBoxProc, 0L);
    
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
    //mgiDestroyIMEContainer (ime_handle);
#endif
    //create_test_phone_ime_wnd();
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

