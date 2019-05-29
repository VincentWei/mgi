
/**
 * \file hwinput.c
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: hwinput.c 231 2008-09-24 08:15:16Z licaijun $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 FMSoft.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

/* ========================================================================= 
 * hand write input module.
   ========================================================================= */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/mgi.h"
#include "hw.h"

#define IDC_BTCLEAR    100
#define IDC_BTBACK     200
#define IDC_BTCR       300
#define IDC_BTSPACE    400

/* --------------------------------------------------------------------- */

HWND hScreenWin;
/* --------------------------------------------------------------------- */

int HWIMECtrlProc (HWND hWnd, int Message, WPARAM wParam, LPARAM lParam);
void set_stroke_point (PHWINFO phwi, HDC hdc, int temp_x, int temp_y);

static int is_mouse_move;
/* --------------------------------------------------------------------- */

/* decides the proper thresh hold of draw stroke */
static int threshhold (PHWINFO phwi, int x1, int y1, int x2, int y2)
{
    int x;
    int y;
    int thresh;

    x = ( x1 > x2 )?(x1 - x2):(x2 - x1);
    //y = ( y1 > y2 )?(y1 - y2):(y2 - y1);
    y = y1 - y2;

    thresh = THRESHHOLD_MIN;

    if ( x*x + y*y > thresh)// && !(x < 4 && y < -9) )
        return TRUE;

    return FALSE;
}   

/* sends the char to the parent window */
static void sendchar (PHWINFO phwi, int n)
{
    if (phwi->fitchar[n][1]) {
        WORD wDByte = MAKEWORD (phwi->fitchar[n][0], phwi->fitchar[n][1]);
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
        SendMessage (GetFocusChild(HWND_DESKTOP), MSG_CHAR, wDByte, (LPARAM)FROM_HWINPUT);
#else
#if (MINIGUI_MAJOR_VERSION > 1)
        Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, wDByte, (LPARAM)FROM_HWINPUT);
#else
        Send2ActiveClient (MSG_CHAR, wDByte, (LPARAM)FROM_HWINPUT);
#endif
#endif
    }
    else {
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
        //SendMessage (phwi->phwnd, MSG_CHAR, phwi->fitchar[n][0], (LPARAM)FROM_HWINPUT);
        SendMessage (GetActiveWindow(), MSG_CHAR, phwi->fitchar[n][0], (LPARAM)FROM_HWINPUT);
#else
#if (MINIGUI_MAJOR_VERSION > 1)
        Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, phwi->fitchar[n][0], (LPARAM)FROM_HWINPUT);
#else
        Send2ActiveClient (MSG_CHAR, phwi->fitchar[n][0], (LPARAM)FROM_HWINPUT);
#endif
#endif
    }

    memset (phwi->fitchar[0], 0, 3*phwi->nword); 
    memset (phwi->result, 0, phwi->nword*2); 
    memset (phwi->m_data, 0, phwi->m_size*6 + 2); 
    phwi->m_len = 0;
}

/* sends the key to the parent window */
static void sendkey (PHWINFO phwi, int code)
{
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
    //PostMessage(sg_hTargetWnd, MSG_KEYDOWN, code, 0);
    //PostMessage(sg_hTargetWnd, MSG_KEYUP, code, 0);
    PostMessage(GetActiveWindow(), MSG_KEYDOWN, code, 0);
    PostMessage(GetActiveWindow(), MSG_KEYUP, code, 0);
#else
#if (MINIGUI_MAJOR_VERSION > 1)
    Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, code, (LPARAM)FROM_HWINPUT);
    Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, code, (LPARAM)FROM_HWINPUT);
#else
    Send2ActiveClient (MSG_KEYDOWN, code, (LPARAM)FROM_HWINPUT);
    Send2ActiveClient (MSG_KEYUP, code, (LPARAM)FROM_HWINPUT);
#endif
#endif
}

/* --------------------------------------------------------------------------------- */

#define HD              7
#define VD              3

#define HW_BT_W         40
#define HW_BT_H         26

#define RIGHT_MAGIN     (HW_BT_W + HD)

static int hw_init_recog_engine (PHWINFO phwi)
{
    /* initialize handwrite recognization engine
     * change me!
     */
    //if (!phwi->engine)
        phwi->engine = &hw_null_engine;

    if (phwi->engine->init)
        phwi->engine->init();

    return 0;
}

/* hw control initilization */
static BOOL phwinput_init (HWND hWnd, PHWINFO phwi)
{
    int  i;
    RECT rcClient;
    int  rcLeft, rcRight;
    //char       bmppath[PATH_MAX];

    GetWindowRect (hWnd, &rcClient);

    /* allocate the memory for stroke data */
    if ( (phwi->m_data = (WORD *) malloc ( (NALLOC * 6 + 2)*sizeof(WORD) ) ) == NULL ) {
        free (phwi);
        return FALSE;
    }

    phwi->nword = NWORD;

    phwi->m_size  = NALLOC;
    phwi->m_len   = 0;
    phwi->capture = FALSE;
    phwi->old_x   = 0;
    phwi->old_y   = 0;
    phwi->phwnd   = GetParent(hWnd);

    if (phwi->phwnd == HWND_INVALID) {
        free (phwi->m_data);
        free (phwi);
        return FALSE;
    }

    memset (phwi->result, 0, phwi->nword*2);

    /*
    for (i = 0; i < phwi->nword; i++)
        memset (phwi->fitchar[i], 0, 3);
    */
    memset (phwi->fitchar[0], 0, 3*phwi->nword); 

    GetClientRect(hWnd, &rcClient);

    rcLeft = rcClient.left+HD;
    rcRight = rcClient.right-RIGHT_MAGIN-HD;

    if (phwi->two_wins)
    {
        SetRect(&phwi->rcl, rcLeft,
                rcClient.top+19, (rcRight - rcLeft)/2 + 3, rcClient.bottom-10);
        SetRect(&phwi->rcr, (rcRight - rcLeft)/2 + 10, rcClient.top+19, rcRight, rcClient.bottom-10);
    } else {
        SetRect(&phwi->rcr, rcLeft, rcClient.top+19, rcRight, rcClient.bottom-10);
    }

    phwi->rcDraw = &phwi->rcr;
    phwi->active_rc = 'r';

    phwi->hHWI = hWnd;

    phwi->hDrawWnd = hWnd;

    phwi->rcchar.left   = rcClient.left + 4;

    phwi->rcchar.right  = rcClient.right - 66;

    phwi->rcchar.top    = rcClient.top;
    phwi->rcchar.bottom = rcClient.top + GetSysCharHeight() ;//+ 2 * SPACE + 1;

    for (i = 0; i < phwi->nword; i++) {
        phwi->charinfo[i].rc.left   = (phwi->rcchar.right * i + 
            phwi->rcchar.left * (phwi->nword - i) ) / phwi->nword;
        phwi->charinfo[i].rc.right  = (phwi->rcchar.right * (i + 1) + 
            phwi->rcchar.left * (phwi->nword - i - 1)) / phwi->nword;
        phwi->charinfo[i].rc.top    = phwi->rcchar.top; 
        phwi->charinfo[i].rc.bottom = phwi->rcchar.bottom; 
        phwi->charinfo[i].code[0]   = '\0';
        phwi->charinfo[i].code[1]   = '\0';
    }

    /* Creates the buttons for controling input */
    CreateWindow ("button",
              "清除",
              //WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
              WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
              IDC_BTCLEAR,
              rcClient.right - HW_BT_W - HD, rcClient.top + 22,
              HW_BT_W, HW_BT_H,
              hWnd, 0);
    CreateWindow ("button",
              //bmppath,
              "退格",
              WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
              IDC_BTBACK,
              rcClient.right - HW_BT_W - HD, rcClient.top + 22 + HW_BT_H + VD,
              HW_BT_W, HW_BT_H,
              hWnd, 0);
    CreateWindow ("button",
              "回车",
              WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
              IDC_BTCR,
              rcClient.right - HW_BT_W - HD, rcClient.top + 22 + HW_BT_H*2+VD*2,
              HW_BT_W, HW_BT_H,
              hWnd, 0);
    CreateWindow ("button",
              "空格",
              WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
              IDC_BTSPACE,
              rcClient.right - HW_BT_W - HD, rcClient.top + 22 + HW_BT_H*3+VD*3,
              HW_BT_W, HW_BT_H,
              hWnd, 0);

    hw_init_recog_engine (phwi);

    return TRUE;
}

void myUnloadBitmap ()
{
}

//draw the recognized word text
void draw_text(HDC hdc, PHWINFO phwi)
{
    int i;

    SetBkMode (hdc, BM_TRANSPARENT);
    SetBrushColor (hdc, PIXEL_lightgray);
    FillBox (hdc, phwi->charinfo[0].rc.left, phwi->charinfo[0].rc.top+1,
                phwi->charinfo[phwi->nword-1].rc.right - phwi->charinfo[0].rc.left,
                phwi->charinfo[0].rc.bottom - phwi->charinfo[0].rc.top+1);
    for (i = 0; i < phwi->nword; i++) {
        if (phwi->fitchar[i][0]) {
            TextOut (hdc, phwi->charinfo[i].rc.left + 2, 
                phwi->charinfo[i].rc.top + SPACE, phwi->fitchar[i]);
        }
    }
}

void onpaint (HDC hdc, PHWINFO phwi)
{ 
    Rectangle(hdc, phwi->rcl.left, phwi->rcl.top, phwi->rcl.right, phwi->rcl.bottom);
    Rectangle(hdc, phwi->rcr.left, phwi->rcr.top, phwi->rcr.right, phwi->rcr.bottom);
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, phwi->rcl.left+1, phwi->rcl.top+1, 
            phwi->rcl.right-phwi->rcl.left-1, phwi->rcl.bottom-phwi->rcl.top-1);
    FillBox(hdc, phwi->rcr.left+1, phwi->rcr.top+1, 
            phwi->rcr.right-phwi->rcr.left-1, phwi->rcr.bottom-phwi->rcr.top-1);

    draw_text(hdc, phwi);
}

/* -------------------------------------------------------------------------- */

/* sets hand write area */
void set_write_area (PHWINFO phwi, HDC hdc, int temp_x, int temp_y)
{

    if (PtInRect(&(phwi->rcl), temp_x, temp_y) && phwi->active_rc == 'r') {
        sendchar(phwi, 0);
        InvalidateRect (phwi->hDrawWnd, phwi->rcDraw, TRUE);            
        phwi->active_rc = 'l';
        phwi->rcDraw = &phwi->rcl;
    }
    else if (PtInRect(&(phwi->rcr), temp_x, temp_y) && phwi->active_rc == 'l') {
        sendchar(phwi, 0);
        InvalidateRect (phwi->hDrawWnd, phwi->rcDraw, TRUE);            
        phwi->active_rc = 'r';
        phwi->rcDraw = &phwi->rcr;
    }
}

/* Sets the stroke point when the pen down */
void set_stroke_point (PHWINFO phwi, HDC hdc, int temp_x, int temp_y)
{
    /* write in the current draw area */
    if (PtInRect ( phwi->rcDraw, temp_x, temp_y ) ) {
        phwi->old_x = temp_x;
        phwi->old_y = temp_y;

        MoveTo (hdc, phwi->old_x, phwi->old_y);

        if (phwi->hDrawWnd) {
            SetCapture (phwi->hDrawWnd);
        }

        is_mouse_move = 0;
        phwi->capture = TRUE;
    }
}

static void draw_stroke (PHWINFO phwi, HDC hdc, int x1, int y1)
{
    int x = x1, y = y1;

        if (!phwi->capture)
        return;

    if ( PtInRect (phwi->rcDraw, x, y) ) {

        if ( !threshhold (phwi, x,y,phwi->old_x,phwi->old_y) )
               return;

        if ( phwi->m_len < phwi->m_size ) {
            phwi->m_data[phwi->m_len*6 + 0] = phwi->old_x;
            phwi->m_data[phwi->m_len*6 + 1] = phwi->old_y;
            phwi->m_data[phwi->m_len*6 + 2] = x;
            phwi->m_data[phwi->m_len*6 + 3] = y;
            phwi->m_data[phwi->m_len*6 + 4] = -1;
            phwi->m_data[phwi->m_len*6 + 5] = 0;
           
            MoveTo (hdc, phwi->old_x, phwi->old_y); 
            LineTo (hdc, x, y); 

            phwi->m_len ++;
            phwi->old_x = x;
            phwi->old_y = y;
        }
    }
}

void recognize_draw (PHWINFO phwi)
{
    int rc, i;

    if ((rc = phwi->engine->recognize (phwi->m_data,  
               phwi->result, &(phwi->nword), phwi->m_len, REC_RANGE)) == 0) {
        for (i = 0; i < phwi->nword; i++) {
            phwi->fitchar[i][0] = phwi->result[i*2];
            phwi->fitchar[i][1] = phwi->result[i*2 + 1];
            phwi->fitchar[i][2] = '\0';
        }
    }
}

/* Clears the drawing screen */
void clear_draw (PHWINFO phwi)
{
    if (phwi->m_len != 0) {
        memset (phwi->m_data, 0, phwi->m_size*6 + 2);
        phwi->m_len = 0;
        memset (phwi->fitchar[0], 0, phwi->nword*3);
        memset (phwi->result, 0, phwi->nword*2);
    }
    //FIXME
    {
        InvalidateRect (phwi->hDrawWnd, phwi->rcDraw, TRUE);            
    }
    InvalidateRect (phwi->hHWI, NULL, FALSE);
}

/* -------------------------------------------------------------------------- */

/* mouse down on the hw control window */
void on_hw_mousedown (PHWINFO phwi, LPARAM lParam) 
{
    int i;
    int temp_x = LOWORD (lParam);
    int temp_y = HIWORD (lParam);
    HDC hdc;

    hdc = GetClientDC (phwi->hDrawWnd);
    //clicks on the recognized word areas
    for (i = 0; i < phwi->nword; i++) {
        if (PtInRect (&(phwi->charinfo[i].rc), temp_x, temp_y)) {
            sendchar(phwi, i);
            clear_draw (phwi);
            ReleaseDC (hdc);
            return;
        }
    }

    set_write_area (phwi, hdc, temp_x, temp_y);

    if (PtInRect(phwi->rcDraw, temp_x, temp_y)) {
        set_stroke_point (phwi, hdc, temp_x, temp_y);
    }

    ReleaseDC (hdc);
}

void on_mousemove (PHWINFO phwi, LPARAM lParam)
{
    int x = LOWORD (lParam);
    int y = HIWORD (lParam);
    HDC hdc;

    if (!phwi->capture) {
        return;
    }

    is_mouse_move = 1;
    hdc = GetClientDC (phwi->hDrawWnd);

    //if (phwi->hw_mode == HW_MODE_NORMAL)
    ScreenToClient (phwi->hDrawWnd, &x, &y);

    draw_stroke (phwi, hdc, x, y);

    ReleaseDC (hdc);
}

//FIXME
void on_mouseup (PHWINFO phwi)
{
    HDC hdc = GetClientDC (phwi->hHWI);

    if (phwi->capture)
        ReleaseCapture ();

    phwi->capture = FALSE;
    phwi->m_data[phwi->m_len*6]     = -1;
    phwi->m_data[phwi->m_len*6 + 1] = -1;
    recognize_draw (phwi);
    draw_text(hdc, phwi);

    ReleaseDC (hdc);
}

void on_command (HWND hWnd, int id)
{
    PHWINFO phwi = (PHWINFO) GetWindowAdditionalData (hWnd);

    switch (id) {
    case IDC_BTCLEAR:
        clear_draw (phwi);
        break;
    case IDC_BTBACK:
        sendkey(phwi, SCANCODE_BACKSPACE);
        break;
    case IDC_BTCR:
        sendkey(phwi, SCANCODE_ENTER);
        break;
    case IDC_BTSPACE:
        sendkey(phwi, SCANCODE_SPACE);
        break;
    }
}

/* -------------------------------------------------------------------------- */

int HWIMECtrlProc (HWND hWnd, int Message, WPARAM wParam, LPARAM lParam)
{
    PHWINFO   phwi = NULL;
    HDC hdc = 0;

    phwi = (PHWINFO) GetWindowAdditionalData (hWnd);

    switch (Message) {

    case MSG_CREATE:
        if ( !phwinput_init (hWnd, phwi) )        
            return -1;
        SetWindowAdditionalData (hWnd,(DWORD)phwi);
        break;
    
    case MSG_LBUTTONDOWN:
        on_hw_mousedown (phwi, lParam);
        break;

    case MSG_COMMAND:
    {
        int id = LOWORD(wParam);
        on_command (hWnd, id);
        break;
    }

    case MSG_LBUTTONUP:
        on_mouseup (phwi);
        break;

    case MSG_MOUSEMOVE:
        if ( !(wParam & KS_CAPTURED) )
            break;
        on_mousemove (phwi, lParam);
        break;


    case MSG_PAINT:
        hdc = BeginPaint (hWnd);
        SetBkColor (hdc, PIXEL_lightgray);
        onpaint (hdc, phwi);
        EndPaint (hWnd, hdc);
        break;

    case MSG_DESTROY:
        myUnloadBitmap ();
        if ( phwi && phwi->m_data)
            free (phwi->m_data);
        if (phwi)
            free (phwi); 
        return 0;

    } /* end switch */
   
    return DefaultControlProc (hWnd, Message, wParam, lParam);
}

/* -------------------------------------------------------------------------- */

static void InitHWIMECreateInfo(PMAINWINCREATE pCreateInfo)
{

    pCreateInfo->dwStyle = WS_NONE;
    pCreateInfo->dwExStyle = WS_EX_USEPRIVATECDC | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    pCreateInfo->spCaption = "HandWrite IME";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(IDC_PENCIL);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = HWIMECtrlProc;
    pCreateInfo->iBkColor = PIXEL_lightgray;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = 0;
}

#ifdef _LITE_VERSION
HWND mgiCreateHandWriteIME (MGI_HANDWRITE_INFO* hw_info, PLOGFONT ime_font)
{
    MAINWINCREATE CreateInfo;
    HWND hw_hwnd;
    PHWINFO phwi;

#ifndef _STAND_ALONE
    if (!mgIsServer)
        return HWND_INVALID;
#endif

    if (!ime_font)
        ime_font = GetSystemFont (SYSLOGFONT_DEFAULT);

    phwi = (PHWINFO)malloc (sizeof(HWINFO));
    phwi->engine = hw_info->engine;
    phwi->two_wins = hw_info->two_wins;

    if (hw_info->w <= 0)
        hw_info->w = DEF_HW_WIDTH;
    if (hw_info->h <= 0)
        hw_info->h = DEF_HW_HEIGHT;

    InitHWIMECreateInfo (&CreateInfo);
    CreateInfo.lx = hw_info->x;
    CreateInfo.ty = hw_info->y;
    CreateInfo.rx = hw_info->x + hw_info->w;
    CreateInfo.by = hw_info->y + hw_info->h;
    CreateInfo.dwAddData = (DWORD)phwi;

    hw_hwnd = CreateMainWindow(&CreateInfo);
    if (hw_hwnd == HWND_INVALID)
    {
        free (phwi);
        return HWND_INVALID;
    }
    //SetWindowFont (hw_hwnd, ime_font);
    return hw_hwnd;
}
#else
typedef struct _HW_IME_INFO
{
    sem_t wait;
    MGI_HANDWRITE_INFO* hw_info;
    PLOGFONT ime_font;
    HWND hwnd;
} HW_IME_INFO;

static void* start_hw_ime (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HW_IME_INFO* hw_ime_info = (HW_IME_INFO*)data;
    HWND hw_hwnd;
    PHWINFO phwi;

    if (!hw_ime_info->ime_font)
        hw_ime_info->ime_font = GetSystemFont (SYSLOGFONT_DEFAULT);

    phwi = (PHWINFO)malloc (sizeof(HWINFO));
    phwi->engine = hw_ime_info->hw_info->engine;
    phwi->two_wins = hw_ime_info->hw_info->two_wins;

    if (hw_ime_info->hw_info->w <= 0)
        hw_ime_info->hw_info->w = DEF_HW_WIDTH;
    if (hw_ime_info->hw_info->h <= 0)
        hw_ime_info->hw_info->h = DEF_HW_HEIGHT;

    InitHWIMECreateInfo (&CreateInfo);
    CreateInfo.lx = hw_ime_info->hw_info->x;
    CreateInfo.ty = hw_ime_info->hw_info->y;
    CreateInfo.rx = hw_ime_info->hw_info->x + hw_ime_info->hw_info->w;
    CreateInfo.by = hw_ime_info->hw_info->y + hw_ime_info->hw_info->h;
    CreateInfo.dwAddData = (DWORD)phwi;

    hw_hwnd = hw_ime_info->hwnd = CreateMainWindow(&CreateInfo);

    sem_post (&hw_ime_info->wait);
    if (hw_hwnd == HWND_INVALID)
    {
        free (phwi);
        return NULL;
    }
    //SetWindowFont (hw_hwnd, ime_font);

    while (GetMessage (&Msg, hw_hwnd) ) {
        TranslateMessage (&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hw_hwnd);
    return NULL;
}

static pthread_t hwimethread;

HWND mgiCreateHandWriteIME (MGI_HANDWRITE_INFO* hw_info, PLOGFONT ime_font)
{
    HW_IME_INFO hw_ime_info;
    pthread_attr_t new_attr;

    if (!hw_info)
        return HWND_INVALID;

    sem_init (&hw_ime_info.wait, 0, 0);
    hw_ime_info.hw_info = hw_info;
    hw_ime_info.ime_font = ime_font;

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create (&hwimethread, &new_attr, start_hw_ime, &hw_ime_info);
    pthread_attr_destroy (&new_attr);

    sem_wait (&hw_ime_info.wait);
    sem_destroy (&hw_ime_info.wait);

    return hw_ime_info.hwnd;
}
#endif

BOOL mgiSetHandWriteEngine (HWND hw_ime_hwnd, MGI_HW_ENGINE* engine)
{
    PHWINFO phwi;
    phwi = (PHWINFO) GetWindowAdditionalData (hw_ime_hwnd);
    if (phwi == NULL)
	return FALSE;
    phwi->engine = engine;
    return TRUE;
}

