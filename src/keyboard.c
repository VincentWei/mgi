/**
 * \file keyboard.c
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: keyboard.c 280 2009-09-22 09:56:27Z hejia $
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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "../include/mgi.h"

#include "mgime_tab.h"
#include "mgi_helper.h"

#include "default_kb_bmp.c"
#include "skb_bmp.c"

static int sk_IsOpened;         /* 1 for opened, 0 for closed */
static int sk_IsFullChar;       /* 0 for half char, 1 for full char */
static int sk_IsFullPunc;       /* 0 for half punctuate marks, 1 for full marks */

static int sk_nIMENr;           /* effetive IME method number */

static HWND sk_sg_hTargetWnd = 0;       // target window of ime.

static MGI_SOFTKEY_INFO default_keyboard[] = {
    {SFT_SCANCODE, NULL, SCANCODE_GRAVE, {0, 0, 20, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_1, {20, 0, 37, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_2, {37, 0, 55, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_3, {55, 0, 73, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_4, {73, 0, 91, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_5, {91, 0, 109, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_6, {109, 0, 127, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_7, {127, 0, 144, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_8, {144, 0, 162, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_9, {162, 0, 180, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_0, {180, 0, 198, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_MINUS, {198, 0, 216, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_EQUAL, {216, 0, 233, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BACKSPACE, {233, 0, 257, 18}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_TAB, {0, 18, 24, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_Q, {24, 18, 42, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_W, {42, 18, 60, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_E, {60, 18, 78, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_R, {78, 18, 96, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_T, {96, 18, 114, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_Y, {114, 18, 132, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_U, {132, 18, 150, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_I, {150, 18, 167, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_O, {167, 18, 185, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_P, {185, 18, 203, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BRACKET_LEFT, {203, 18, 221, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BRACKET_RIGHT, {221, 18, 239, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_BACKSLASH, {239, 18, 257, 36}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_CAPSLOCK, {0, 36, 28, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_A, {28, 36, 46, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_S, {46, 36, 64, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_D, {64, 36, 81, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_F, {81, 36, 100, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_G, {100, 36, 117, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_H, {117, 36, 135, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_J, {135, 36, 153, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_K, {153, 36, 171, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_L, {171, 36, 189, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_SEMICOLON, {189, 36, 207, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_APOSTROPHE, {207, 36, 226, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_KEYPADENTER, {226, 36, 257, 54}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_LEFTSHIFT, {0, 54, 39, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_Z, {39, 54, 57, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_X, {57, 54, 75, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_C, {75, 54, 93, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_V, {93, 54, 111, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_B, {111, 54, 129, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_N, {129, 54, 147, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_M, {147, 54, 165, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_COMMA, {165, 54, 182, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_PERIOD, {182, 54, 200, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_SLASH, {200, 54, 219, 71}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_ESCAPE, {0, 71, 24, 90}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_SPACE, {24, 71, 211, 90}, 0},
    {SFT_SCANCODE, NULL, SCANCODE_INSERT, {211, 71, 234, 90}, 0},
    {SFT_STRING, "北京飞漫软件技术有限公司\nBeijing Feynman Software Technology Co., Ltd.",
        0, {234, 71, 257, 90}, 0}
};

static RECT src_keyrec[TABLESIZE(default_keyboard)];// = { 0 };
static char keymask[5] = { 14, 14, 13, 11, 4 };

static KEYEVENT KE;
static int capslock_index, ctrl_index, shift_index;
static int prev_sel = -1, cur_sel = 0;
static int downkey = -1, drawkey;

#define DEFAULT_NR_KEYS     56

#define INPUT_OPEN      (1 << 0)
#define KEYBOARD_OPEN    (1 << 1)
#define is_input_open   (win_status & INPUT_OPEN)
#if 0
#define is_keyboard_open (win_status & KEYBOARD_OPEN)
#endif

static Uint16 win_status = INPUT_OPEN | KEYBOARD_OPEN;

static int text_height = 0;

static unsigned long sk_InpKey[MAX_INPUT_LENGTH],
    sk_save_InpKey[MAX_INPUT_LENGTH];

static mgime_input_table *quanpin_input_table;
/* 6 bit a key mask */
static const unsigned long sk_mask[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x3F000000, 0x3FFC0000, 0x3FFFF000, 0x3FFFFFC0, 0x3FFFFFFF, 0x3FFFFFFF,
    0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF
};

static const unsigned char sk_fullchar[] =
    "　！”＃＄％＆’（）＊＋，－．／０１２３４５６７８９：；＜＝＞？"
    "＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ〔＼〕＾□"
    "‘ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～  ";

#define CPUNCTYPE_1TO1      0
#define CPUNCTYPE_1TO2      1
#define CPUNCTYPE_CYCLE     2

#define CTRLBTN_WIDTH       16
#define CTRLBTN_HEIGHT      16


#define GET_SOFTKEYBOARD_INFO(hWnd) \
    MGI_SOFTKEYBOARD_INFO* sk_info; \
    sk_info = (MGI_SOFTKEYBOARD_INFO*)GetWindowAdditionalData (hWnd);

#define GET_CUR_OPS(sk_info) \
    (sk_info)->trans_ops[(sk_info)->cur_trans_ops]

typedef struct PuncMark
{
    unsigned char epunc;
    int     type;
    int     currone;            // only for cycle
    unsigned char *cpunc;
} PUNCMARK;

static PUNCMARK sk_puncmark[] = {
    {'`', 0, 0, "·"},
    {'~', 0, 0, "～"},
    {'!', 0, 0, "！"},
    {'@', 0, 0, "℃"},
    {'#', 0, 0, "＃"},
    {'$', 0, 0, "￥"},
    {'%', 0, 0, "％"},
    {'^', CPUNCTYPE_1TO2, 0, "……"},
    {'&', 0, 0, "§"},
    {'*', 0, 0, "×"},
    {'_', CPUNCTYPE_1TO2, 0, "——"},
    {'(', CPUNCTYPE_CYCLE, 0, "（）"},
    {')', CPUNCTYPE_CYCLE, 0, "〔〕"},
    {'{', CPUNCTYPE_CYCLE, 0, "《》"},
    {'}', CPUNCTYPE_CYCLE, 0, "〈〉"},
    {'[', CPUNCTYPE_CYCLE, 0, "〖〗"},
    {']', CPUNCTYPE_CYCLE, 0, "【】"},
    {'\\', 0, 0, "÷"},
    {'|', 0, 0, "※"},
    {';', 0, 0, "；"},
    {':', 0, 0, "："},
    {'\'', CPUNCTYPE_CYCLE, 0, "‘’"},
    {'\"', CPUNCTYPE_CYCLE, 0, "“”"},
    {',', 0, 0, "，"},
    {'.', 0, 0, "。"},
    {'<', CPUNCTYPE_CYCLE, 0, "「」"},
    {'>', CPUNCTYPE_CYCLE, 0, "『』"},
    {'?', 0, 0, "？"},
    {'/', 0, 0, "、"}
};

static BITMAP shurufa, *sk_sg_bmpIME = NULL;    // the bmp of the IME control button

static int sk_CurSelNum = 0;    /* Current Total Selection Number */
static int is_keyboard_open = 1;

static char sk_seltab[16][MAX_PHRASE_LENGTH];
static int sk_NextPageIndex, sk_CurrentPageIndex, sk_MultiPageMode;
   /* Input key buffer */
static int sk_InputCount, sk_InputMatch, sk_StartKey, sk_EndKey;
static int sk_save_StartKey, sk_save_EndKey, sk_save_MultiPageMode,
    sk_save_NextPageIndex, sk_save_CurrentPageIndex;

static int sk_CharIndex[15];    // starting index of matched char/phrases
static unsigned long sk_val1, sk_val2, sk_key1, sk_key2;
static BOOL sk_bTwoLines = TRUE;

static BOOL sk_ime_filter(MGI_SOFTKEYBOARD_INFO* sk_info,
                          const unsigned char key, LPARAM lParam);
static void sk_refresh_input_method_area(HWND hwnd, HDC hdc);
static void sk_DispSelection(HDC hDC);
static int sk_toggle_input_method(MGI_SOFTKEYBOARD_INFO* sk_info);
static void sk_toggle_half_full(void);
static BOOL sk_ime_input_init(MGI_SOFTKEYBOARD_INFO* sk_info);
static void sk_mg_ime_writemsg(BYTE * buffer, int len, LPARAM lParam,
                               BOOL bDByte);
static void sk_mg_ime_outchar(unsigned char c, LPARAM lParam);
static void sk_ime_input_done(MGI_SOFTKEYBOARD_INFO* sk_info);
static void sk_FindMatchKey(void);
static void sk_FillMatchChars(int j, char* ret_value);
static void ClrIn(void);
static void sk_putstr(unsigned char *p, LPARAM lParam);
static mgime_input_table* load_quanpin_input_method(void);
static void sk_free_input_method(mgime_input_table * table);

static BOOL mgi_is_punc (const char c)
{
    int i;
    for (i = 0; i < TABLESIZE(sk_puncmark); i++)
    {
        if (sk_puncmark[i].epunc == c)
            return TRUE;
    }
    return FALSE;
}

void en_trans_init (void)
{
}

void en_trans_destroy (void)
{
}

void en_trans_refresh (HWND hwnd, HDC hdc, void* context)
{
    MGI_TRANSLATE_OPS* trans_ops = (MGI_TRANSLATE_OPS*)context;
    DisabledTextOutEx (hdc, hwnd, 2, 2, trans_ops->ops_name);
}

int en_trans_alpha (const char* origin_str, char* ret_value,
                    LPARAM lParam, BOOL* write_directly)
{
    strncpy (ret_value, origin_str, 1);
    if (write_directly)
        *write_directly = TRUE;
    return 1;
}

int en_trans_num (const char* origin_str, char* ret_value,
                  BOOL *write_directly)
{
    strncpy (ret_value, origin_str, 1);
    if (write_directly)
        *write_directly = TRUE;
    return 1;
}

int en_trans_punc (const char origin_punc, char* ret_value)
{
    strncpy (ret_value, &origin_punc, 1);
    return 1;
}

int en_associate (const char* origin_str, char* ret_value)
{
    strncpy (ret_value, origin_str, 1);
    return 1;
}

MGI_TRANSLATE_OPS en_trans_ops = {
    "【 En 】",
    TRUE,
    en_trans_init,
    en_trans_destroy,
    en_trans_refresh,
    en_trans_alpha,
    en_trans_num,
    en_trans_punc,
    en_associate,
};

void quanpin_trans_init (void)
{
    quanpin_input_table = load_quanpin_input_method();
    quanpin_input_table->MaxDupSel = MAXDUPSEL;
}

void quanpin_trans_destroy (void)
{
    sk_free_input_method(quanpin_input_table);
}

void quanpin_trans_refresh (HWND hwnd, HDC hdc, void* context)
{
    RECT rc;
    char    str[100];
    int     i;
    char    minibuf[2];

    MGI_TRANSLATE_OPS* trans_ops = (MGI_TRANSLATE_OPS*)context;

    GetClientRect(hwnd, &rc);
    FillBoxWithBitmapPart(hdc, rc.right - 4 - 32, 2, CTRLBTN_WIDTH,
                          CTRLBTN_HEIGHT, 0, 0, sk_sg_bmpIME, 0,
                          (sk_IsFullChar * CTRLBTN_HEIGHT));

    FillBoxWithBitmapPart(hdc, rc.right - 4 - 16, 2, CTRLBTN_WIDTH,
                          CTRLBTN_HEIGHT, 0, 0, sk_sg_bmpIME, CTRLBTN_WIDTH,
                          (sk_IsFullPunc * CTRLBTN_HEIGHT));

    minibuf[1] = 0;
    strcpy(str, trans_ops->ops_name);

    for (i = 0; i <= MAX_INPUT_LENGTH; i++)
    {
        if (i < sk_InputCount)
            minibuf[0] = quanpin_input_table->KeyName[sk_InpKey[i]];
        else
            minibuf[0] = ' ';

        if (i == sk_InputMatch && sk_InputCount > sk_InputMatch && i != 0)
            strcat(str, "-");

        strcat(str, minibuf);
    }

    TextOut(hdc, 2, 2, str);
    sk_DispSelection(hdc);
}

int quanpin_trans_alpha (const char* origin_str, char* ret_value,
                         LPARAM lParam, BOOL* write_directly)
{
    int     inkey = 0;
    unsigned char key = origin_str[0];
    if (sk_IsFullChar && key >= ' ' && key < 127)
    {
        key = (key - ' ') << 1;
        ret_value[0] = (char)(sk_fullchar[key]);
        ret_value[1] = (char)(sk_fullchar[key + 1]);
        if (write_directly)
            *write_directly = TRUE;
        return 2;
    }

    switch (key)
    {
    case '\010':               /* BackSpace Ctrl+H */
    case '\177':               /* BackSpace */
        if (sk_InputCount > 0)
        {
            sk_InpKey[--sk_InputCount] = 0;
            if (sk_InputCount == 0)
            {
                ClrIn();
            }
            else if (sk_InputCount <= sk_InputMatch)
            {
                sk_FindMatchKey();
                sk_MultiPageMode = 0;
                sk_CurrentPageIndex = sk_StartKey;
                sk_FillMatchChars(sk_StartKey, ret_value);
            }
        }
        else
            sk_mg_ime_outchar(key, lParam);
        break;

    case '\033':               /* ESCAPE */
        if (sk_InputCount > 0)
            ClrIn();
        else
            sk_mg_ime_outchar(key, lParam);
        break;

    case '-':
        if (sk_MultiPageMode)
        {
            if (sk_CurrentPageIndex > sk_StartKey)
                sk_CurrentPageIndex =
                    sk_CurrentPageIndex - quanpin_input_table->MaxDupSel;
            else
                sk_CurrentPageIndex = sk_StartKey;
            sk_FillMatchChars(sk_CurrentPageIndex, ret_value);

        }
        else
            sk_mg_ime_outchar(key, lParam);
        break;

    case '=':
        if (sk_MultiPageMode)
        {
            sk_CurrentPageIndex = sk_NextPageIndex;
            sk_FillMatchChars(sk_CurrentPageIndex, ret_value);
        }
        else
            sk_mg_ime_outchar(key, lParam);
        break;

    case ' ':
        if (sk_CurSelNum == 0)
            sk_mg_ime_outchar(key, lParam);
        if (sk_seltab[0][0])
            sk_putstr(sk_seltab[0], lParam);
        break;

    default:
        inkey = quanpin_input_table->KeyMap[key];
        if (!inkey)
        {
            ClrIn();
            sk_mg_ime_outchar(key, lParam);
            break;
        }

        if (inkey >= 1 && sk_InputCount < MAX_INPUT_LENGTH)
            sk_InpKey[sk_InputCount++] = inkey;

        if (sk_InputCount <= sk_InputMatch + 1)
        {
            sk_FindMatchKey();
            sk_CurrentPageIndex = sk_StartKey;
            sk_MultiPageMode = 0;
            sk_FillMatchChars(sk_StartKey, ret_value);
            if (sk_InputCount >= quanpin_input_table->MaxPress && sk_CurSelNum == 1
                && quanpin_input_table->last_full)
            {
                // left only one selection 
                sk_putstr(sk_seltab[0], lParam);
            }
        }

        break;

    }                           /* switch */
    return 0;
}

int quanpin_trans_num (const char* origin_str, char* ret_value,
                       BOOL* write_directly)
{
    char   *is_sel_key = (char *)0;
    int vv, len;
    is_sel_key = strchr(quanpin_input_table->selkey, origin_str[0]);
    vv = is_sel_key - quanpin_input_table->selkey;
    if (sk_CurSelNum == 0 || sk_seltab[vv][0] == 0)
    {
        ClrIn();
        memcpy (ret_value, origin_str, 1);
        if (write_directly)
            *write_directly = TRUE;
        return 1;
    }
    if (sk_CurSelNum > 0 && sk_seltab[vv][0])
    {
        len = strlen (sk_seltab[vv]);
        len = len > MAX_SEL_LENGTH ? MAX_SEL_LENGTH : len;
        strncpy (ret_value, sk_seltab[vv], len);
        ret_value[len]='\0';
        if (write_directly)
            *write_directly = TRUE;
        return len;
    }
    return 0;
}

int quanpin_trans_punc (const char origin_punc, char* ret_value)
{
    int     i;

    if (!sk_IsFullPunc)
    {
        memcpy (ret_value, &origin_punc, 1);
        ClrIn();
        return 1;
    }

    for (i = 0; i < TABLESIZE(sk_puncmark); i++)
    {
        if (sk_puncmark[i].epunc == origin_punc)
        {
            switch (sk_puncmark[i].type)
            {
            case CPUNCTYPE_1TO1:
                memcpy(ret_value, sk_puncmark[i].cpunc, 2);
                ClrIn();
                return 2;
            case CPUNCTYPE_1TO2:
                memcpy(ret_value, sk_puncmark[i].cpunc, 4);
                ClrIn();
                return 4;
            case CPUNCTYPE_CYCLE:
                memcpy(ret_value, sk_puncmark[i].cpunc + sk_puncmark[i].currone, 2);
                if (sk_puncmark[i].currone == 0)
                    sk_puncmark[i].currone = 2;
                else
                    sk_puncmark[i].currone = 0;
                ClrIn();
                return 2;
            default:
                return 0;
            }
        }
    }
    return 0;
}

int quanpin_associate (const char* origin_str, char* ret_value)
{
    return 0;
}

MGI_TRANSLATE_OPS quanpin_trans_ops = {
    "【全拼】",
    FALSE,
    quanpin_trans_init,
    quanpin_trans_destroy,
    quanpin_trans_refresh,
    quanpin_trans_alpha,
    quanpin_trans_num,
    quanpin_trans_punc,
    quanpin_associate,
};


static mgime_input_table *
load_quanpin_input_method(void)
{
    mgime_input_table *table;
	int i;

	table = (mgime_input_table *) mgime_tab;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
	table->TotalKey = ArchSwap32(table->TotalKey);
	table->MaxPress = ArchSwap32(table->MaxPress);
	table->MaxDupSel= ArchSwap32(table->MaxDupSel);
	table->TotalChar= ArchSwap32(table->TotalChar);
	table->PhraseNum= ArchSwap32(table->PhraseNum);
	for (i = 0; i < 64; i++) {
		table->KeyIndex [i] = ArchSwap16(table->KeyIndex[i]);
	}
#endif

    if (strcmp(MAGIC_NUMBER, table->magic_number))
        return NULL;

    table->item = (ITEM *) (table + 1);
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN

	for (i = 0; i < table->TotalChar; i++) {
		table->item[i].key1 = ArchSwap32(table->item[i].key1);
		table->item[i].key2 = ArchSwap32(table->item[i].key2);
		table->item[i].frequency = ArchSwap16(table->item[i].frequency);
	}
#endif
    table->PhraseFile = table->AssocFile = NULL;

    return table;
}

static void ClrIn(void)
{
    memset(sk_InpKey, 0, sizeof(sk_InpKey));
    memset(sk_seltab, 0, sizeof(sk_seltab));

    sk_MultiPageMode = sk_NextPageIndex = sk_CurrentPageIndex = 0;
    sk_CurSelNum = sk_InputCount = sk_InputMatch = 0;
}

/*  Find the matched chars/phrases and fill it into SelTab
    The starting checked index is j 
 
    The Selection Line 1xxx 2xxx,  80-20=60 60/2=30 chinese chars only
    0-9 Selection can contain only 30 chinese chars
*/
static void
sk_FillMatchChars(int j, char* ret_value)
{
    int     SelNum = 0, CurLen = 0;

    //bzero( sk_seltab, sizeof( sk_seltab ) );
    while ((quanpin_input_table->item[j].key1 & sk_mask[sk_InputCount + 5]) == sk_val1
           && (quanpin_input_table->item[j].key2 & sk_mask[sk_InputCount]) == sk_val2
           && SelNum < quanpin_input_table->MaxDupSel && j < sk_EndKey
           && CurLen < MAX_SEL_LENGTH)
    {
        unsigned short ch = quanpin_input_table->item[j].ch;

        if (ch >= 0xA1A1)
        {
            memcpy(&sk_seltab[SelNum], &(quanpin_input_table->item[j].ch), 2);
            sk_seltab[SelNum][2] = '\0';
        }

        CurLen += strlen(sk_seltab[SelNum++]);
        j++;
    }

    if (SelNum == 0)            /* some match found */
    {
        sk_StartKey = sk_save_StartKey;
        sk_EndKey = sk_save_EndKey;
        sk_MultiPageMode = sk_save_MultiPageMode;
        sk_NextPageIndex = sk_save_NextPageIndex;
        sk_CurrentPageIndex = sk_save_CurrentPageIndex;
        return;                 /* keep the original selection */
    }

    sk_CurSelNum = SelNum;

    for (SelNum = sk_CurSelNum; SelNum < 16; SelNum++)
        sk_seltab[SelNum][0] = '\0';    /* zero out the unused area */
    sk_InputMatch = sk_InputCount;      /* until now we have some matches */

    /* check if more than one page */
    if (j < sk_EndKey
        && (quanpin_input_table->item[j].key1 & sk_mask[sk_InputCount + 5]) == sk_val1
        && (quanpin_input_table->item[j].key2 & sk_mask[sk_InputCount]) == sk_val2
        && sk_CurSelNum == quanpin_input_table->MaxDupSel)
    {
        /* has another matched key, so enter sk_MultiPageMode, has more pages */
        sk_NextPageIndex = j;
        sk_MultiPageMode = 1;
    }
    else if (sk_MultiPageMode)
    {
        sk_NextPageIndex = sk_StartKey; /* rotate selection */
    }
    else
        sk_MultiPageMode = 0;
}

/* After add/delete a char, search the matched char/phrase, update the
   sk_StartKey/sk_EndKey key,  save the related keys at first, if no match
   is found, we may restore its original value
*/
static void sk_FindMatchKey(void)
{
    sk_save_StartKey = sk_StartKey;
    sk_save_EndKey = sk_EndKey;
    sk_save_MultiPageMode = sk_MultiPageMode;
    sk_save_NextPageIndex = sk_NextPageIndex;
    sk_save_CurrentPageIndex = sk_CurrentPageIndex;

    sk_val1 =
        sk_InpKey[4] | (sk_InpKey[3] << 6) | (sk_InpKey[2] << 12) |
        (sk_InpKey[1] << 18) | (sk_InpKey[0] << 24);
    sk_val2 =
        sk_InpKey[9] | (sk_InpKey[8] << 6) | (sk_InpKey[7] << 12) |
        (sk_InpKey[6] << 18) | (sk_InpKey[5] << 24);

    if (sk_InputCount == 1)
        sk_StartKey = quanpin_input_table->KeyIndex[sk_InpKey[0]];
    else
        sk_StartKey = sk_CharIndex[sk_InputCount - 1];

    sk_EndKey = quanpin_input_table->KeyIndex[sk_InpKey[0] + 1];
    for (; sk_StartKey < sk_EndKey; sk_StartKey++)
    {
        sk_key1 =
            (quanpin_input_table->item[sk_StartKey].key1 & sk_mask[sk_InputCount + 5]);
        sk_key2 =
            (quanpin_input_table->item[sk_StartKey].key2 & sk_mask[sk_InputCount]);

        if (sk_key1 > sk_val1)
            break;
        if (sk_key1 < sk_val1)
            continue;
        if (sk_key2 < sk_val2)
            continue;
        break;
    }
    sk_CharIndex[sk_InputCount] = sk_StartKey;
}

static inline BOOL
sk_imeIsSpecialChar(int chr)
{
    if (chr == '\t' || chr == '\n' || chr == '\r')
        return TRUE;

    return FALSE;
}

static void
sk_free_input_method(mgime_input_table * table)
{
}

static void
sk_ime_input_done(MGI_SOFTKEYBOARD_INFO* sk_info)
{
    int     i;

    for (i = 0; i < NR_INPUTMETHOD
            && sk_info->trans_ops[i] != NULL; i++)
    {
        sk_info->trans_ops[i]->cb_destroy();
    }
}

static void
sk_mg_ime_outchar(unsigned char c, LPARAM lParam)
{
    int     scancode = 0;

    switch (c)
    {
    case '\010':
    case '\177':               /* BackSpace */
        scancode = SCANCODE_BACKSPACE;
        break;
    case '\033':
        scancode = SCANCODE_ESCAPE;
        break;
    case '\t':
        scancode = SCANCODE_TAB;
        break;
    case '\n':
        scancode = SCANCODE_ENTER;
        break;
    }

    if (scancode)
    {
#if defined(_LITE_VERSION) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
        if (!sk_mg_ime_outchar) {
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, scancode, (LPARAM)KS_IMEPOST);
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, scancode, (LPARAM)KS_IMEPOST);
            return;
        }
#elif defined(_LITE_VERSION) && !defined(_STAND_ALONE)
        if (!sk_mg_ime_outchar) {
            Send2ActiveClient (MSG_KEYDOWN, scancode, (LPARAM)KS_IMEPOST);
            Send2ActiveClient (MSG_KEYUP, scancode, (LPARAM)KS_IMEPOST);
            return;
        }
#endif
        PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, scancode,
                    (LPARAM) KS_IMEPOST);
        PostMessage(sk_sg_hTargetWnd, MSG_KEYUP, scancode, (LPARAM) KS_IMEPOST);
    }
    else
        sk_mg_ime_writemsg(&c, 1, lParam, FALSE);
}

static void
sk_mg_ime_writemsg(BYTE * buffer, int len, LPARAM lParam, BOOL bDByte)
{
    WORD    wDByte;
    int     i;

    if (bDByte)
    {
        for (i = 0; i < len; i += 2)
        {
            wDByte = MAKEWORD(buffer[i], buffer[i + 1]);
            if (sk_sg_hTargetWnd)
                PostMessage(sk_sg_hTargetWnd, MSG_CHAR, wDByte, 0);
#if defined(_LITE_VERSION) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
            else
                Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, wDByte, 0);
#elif defined(_LITE_VERSION) && !defined(_STAND_ALONE)
            else
                Send2ActiveClient (MSG_CHAR, wDByte, 0);
#endif
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            if (sk_sg_hTargetWnd)
                SendMessage (sk_sg_hTargetWnd, MSG_CHAR, buffer[i], 0);
#if defined(_LITE_VERSION) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
            else
                Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, buffer[i], 0);
#elif defined(_LITE_VERSION) && !defined(_STAND_ALONE)
            else
                Send2ActiveClient (MSG_CHAR, buffer[i], 0);
#endif
        }
    }
}

static void
sk_putstr(unsigned char *p, LPARAM lParam)
{
    int index, len = strlen(p);

    sk_mg_ime_writemsg(p, len, lParam, TRUE);
    if (sk_InputCount <= sk_InputMatch) /* All Match */
    {
        index = (int)p[len - 2] * 256 + p[len - 1];
        ClrIn();
    }
    else
    {
        int nCount = sk_InputCount - sk_InputMatch, nMatch =
            sk_InputMatch, i;
        sk_MultiPageMode = sk_NextPageIndex = sk_CurrentPageIndex = 0;
        sk_InputCount = sk_InputMatch = 0;

        for (i = 0; i < nCount; i++)
            sk_save_InpKey[i] = sk_InpKey[nMatch + i];

        memset(sk_InpKey, 0, sizeof(sk_InpKey));
        for (i = 1; i <= nCount; i++)   /* feed the additional keys */
        {
            sk_InpKey[sk_InputCount] = sk_save_InpKey[sk_InputCount];
            sk_InputCount++;
            if (sk_InputCount <= sk_InputMatch + 1)
            {
                sk_FindMatchKey();
                sk_MultiPageMode = 0;
                sk_CurrentPageIndex = sk_StartKey;
                sk_FillMatchChars(sk_StartKey, NULL);
            }
        }
        if (sk_InputMatch == 0) /* left key has no match, delete */
        {
            ClrIn();
            return;
        }
    }
}

static inline BOOL
sk_imeIsEffectiveIME()
{
    return sk_IsFullChar || sk_IsFullPunc || sk_IsOpened;
}

static int
is_enter_key(WPARAM wParam)
{
    if (wParam == SCANCODE_KEYPADENTER || wParam == SCANCODE_ENTER)
        return TRUE;
    return FALSE;
}

static  inline BOOL
is_direc_key(int scancode)
{
    if (scancode == SCANCODE_CURSORBLOCKUP
        || scancode == SCANCODE_CURSORBLOCKDOWN
        || scancode == SCANCODE_CURSORBLOCKLEFT
        || scancode == SCANCODE_CURSORBLOCKRIGHT)
        return TRUE;
    return FALSE;
}

static inline BOOL
sk_imeIsSpecialKey(int scancode)
{
    if (scancode == SCANCODE_TAB
            || scancode == SCANCODE_ENTER
            || scancode > SCANCODE_F1)
        return TRUE;

    return FALSE;
}

static  BOOL
sk_ime_input_init(MGI_SOFTKEYBOARD_INFO* sk_info)
{
    int     i;

    sk_IsOpened = 1;
    sk_IsFullChar = 0;
    sk_IsFullPunc = 0;
    sk_nIMENr = 0;

    sk_sg_bmpIME = &shurufa;

    sk_info->cur_trans_ops = 0;

    for (i = 0; i < NR_INPUTMETHOD; i++)
        sk_info->trans_ops [i] = NULL;

    if (LoadBitmapFromMem
        (HDC_SCREEN, sk_sg_bmpIME, shurufa_bmp, sizeof(shurufa_bmp), "bmp") < 0)
    {
        fprintf(stderr, "IME: Load the control button failure!\n");
        return FALSE;
    }

    return TRUE;
}

static void
sk_toggle_punc(void)
{
    sk_IsFullPunc ^= 1;
    ClrIn();
}

static void
sk_toggle_half_full(void)
{
    sk_IsFullChar ^= 1;
    ClrIn();
}

static int
sk_toggle_input_method(MGI_SOFTKEYBOARD_INFO* sk_info)
{
    if (sk_info->trans_ops [sk_info->cur_trans_ops + 1] != NULL)
        sk_info->cur_trans_ops++;
    else
        sk_info->cur_trans_ops = 0;

    ClrIn();

    return sk_info->cur_trans_ops;
}

static  BOOL
sk_ime_filter(MGI_SOFTKEYBOARD_INFO* sk_info, const unsigned char key, LPARAM lParam)
{
    char ret_value[MAX_SEL_LENGTH];
    int     len;
    BOOL write_directly = FALSE;

    if (mgi_is_punc (key))
    {
        len = GET_CUR_OPS(sk_info)->cb_trans_punc(key, ret_value);

        if (len > 0)
        {
            if (len == 1)
            {
                sk_mg_ime_outchar(ret_value[0], lParam);
                return FALSE;
            }
            else
            {
                sk_mg_ime_writemsg((BYTE *)ret_value, len, lParam, TRUE);
                return TRUE;
            }
        }
        return FALSE;
    }

#if 0
    if (key == 127) {
        sk_mg_ime_outchar(key, lParam);
        return TRUE;
    }
#endif

    if (key > 47 && key < 59)       //isdigit
    {
        len = GET_CUR_OPS(sk_info)->cb_trans_num (&key, ret_value,
                                            &write_directly);
        if (write_directly)
        {
            if (len > 0)
            {
                if (len == 1)
                {
                    sk_mg_ime_outchar(ret_value[0], lParam);
                    return FALSE;
                }
                else
                    sk_putstr(ret_value, 0);
            }
        }
        return TRUE;
    }

    len = GET_CUR_OPS(sk_info)->cb_trans_alpha(&key, ret_value,
                                        lParam, &write_directly);

    if (write_directly)
    {
        if (len > 0)
        {
            if (len == 1)
            {
                sk_mg_ime_outchar(ret_value[0], lParam);
                return FALSE;
            }
            else
                sk_mg_ime_writemsg((BYTE *)ret_value, len, lParam, TRUE);
        }
    }

    return TRUE;
}

static void
sk_refresh_input_method_area(HWND hwnd, HDC hdc)
{
    GET_SOFTKEYBOARD_INFO (hwnd);
    if (!sk_IsOpened)
        return;

    GET_CUR_OPS(sk_info)->cb_draw
                (hwnd, hdc, GET_CUR_OPS(sk_info));
}

static void
sk_DispSelection(HDC hDC)
{
    int     i, pos = 1;

    char    str[100];
    char    minibuf[2];

    minibuf[1] = 0;
    str[0] = 0;
    if (sk_MultiPageMode && sk_CurrentPageIndex != sk_StartKey)
    {
        strcat(str, "< ");
        pos += 2;
    }
    for (i = 0; i < sk_CurSelNum; i++)
    {
        if (!sk_seltab[i][0])
        {
            if (i == 0)
                continue;
            else
                break;
        }
        minibuf[0] = quanpin_input_table->selkey[i];
        strcat(str, minibuf);
        strcat(str, sk_seltab[i]);
        strcat(str, " ");
    }

    if (sk_MultiPageMode && sk_NextPageIndex != sk_StartKey)
        strcat(str, "> ");

    if (sk_bTwoLines)
        TextOut(hDC, 2, 18, str);
    else
        TextOut(hDC, 150, 2, str);
}


static int
shift_valid_key(MGI_SOFTKEYBOARD_INFO* sk_info, int key)
{
    switch (sk_info->keys[key].scancode)
    {
    case SCANCODE_LEFTSHIFT:
    case SCANCODE_CAPSLOCK:
    case SCANCODE_LEFTCONTROL:
    case SCANCODE_SPACE:
    case SCANCODE_ESCAPE:
    case SCANCODE_INSERT:
    case SCANCODE_REMOVE:
    case SCANCODE_TAB:
    case SCANCODE_BACKSPACE:
        return 0;

    default:
        return 1;
    }
}

static BOOL
set_key_state(MGI_SOFTKEYBOARD_INFO* sk_info, int key, KEYEVENT * ke)
{
    if (sk_info->keys[key].type == SFT_STRING
                && sk_info->keys[key].str != NULL)
    {
        sk_mg_ime_writemsg((BYTE *)sk_info->keys[key].str,
                strlen(sk_info->keys[key].str), 0, FALSE);
        return FALSE;
    }

    switch (sk_info->keys[key].scancode)
    {
    case SCANCODE_LEFTSHIFT:
    case SCANCODE_CAPSLOCK:
        sk_info->keys[key].status = 1 - sk_info->keys[key].status;
        KE.status = sk_info->keys[key].status << 18;
        break;

    case SCANCODE_LEFTCONTROL:
        sk_info->keys[key].status = 1;
    }

    KE.status &= ~(0x00000FFF);
    KE.status |=
        (DWORD) (sk_info->keys[capslock_index].status << 8 |
                 sk_info->keys[ctrl_index].status << 5 |
                 sk_info->keys[shift_index].status << 1);
    KE.scancode = sk_info->keys[key].scancode;
    *ke = KE;

    return TRUE;
}

int
soft_keyboard_mouse_down(HWND hWnd, int x, int y, KEYEVENT * ke)
{
    int     i;

    GET_SOFTKEYBOARD_INFO (hWnd);

    for (i = 0; i < sk_info->nr_keys; i++)
    {
        if (PtInRect(&sk_info->keys[i].rect, x, y))
        {
            drawkey = downkey = i;
            prev_sel = cur_sel;
            cur_sel = i;
            set_key_state(sk_info, i, ke);
#if 1
            InvalidateRect (hWnd, NULL, TRUE);
#else
            if (prev_sel >= 0)
                InvalidateRect(hWnd, &sk_info->keys[prev_sel].rect, FALSE);
            if (cur_sel >= 0)
                InvalidateRect(hWnd, &sk_info->keys[cur_sel].rect, FALSE);
#endif
            return 1;
        }
    }

    return 0;
}

int cursor_direct(HWND hWnd, int scancode)
{
    int i, line = 0, curline = 0, lpos;

    GET_SOFTKEYBOARD_INFO (hWnd);

    if (scancode == SCANCODE_CURSORBLOCKUP
        || scancode == SCANCODE_CURSORBLOCKDOWN
        || scancode == SCANCODE_CURSORBLOCKLEFT
        || scancode == SCANCODE_CURSORBLOCKRIGHT)
    {

        prev_sel = lpos = cur_sel;
        if (cur_sel >= 0)
        {
            for (i = 0; i < 5; i++)
            {
                line += keymask[i];
                if (line - 1 >= cur_sel)
                {
                    curline = i;
                    break;
                }
                lpos -= keymask[i];
            }
        }

        switch (scancode)
        {
        case SCANCODE_CURSORBLOCKUP:
            if (curline > 0)
                curline--;
            if (lpos > keymask[curline] - 1)
                lpos = keymask[curline] - 1;

            cur_sel = lpos;
            for (i = 0; i < curline; i++)
                cur_sel += keymask[i];
            break;

        case SCANCODE_CURSORBLOCKDOWN:
            if (curline < 4)
                curline++;
            if (lpos > keymask[curline] - 1)
                lpos = keymask[curline] - 1;

            cur_sel = lpos;
            for (i = 0; i < curline; i++)
                cur_sel += keymask[i];
            break;

        case SCANCODE_CURSORBLOCKLEFT:
            if (cur_sel > 0)
                cur_sel--;
            break;

        case SCANCODE_CURSORBLOCKRIGHT:
            if (cur_sel < 55)
                cur_sel++;
            break;
        }

#if 1
        if (cur_sel != prev_sel)
            InvalidateRect(hWnd, NULL, TRUE);
#else
        //if (cur_sel != prev_sel)
        //{
        if (prev_sel >= 0)
            InvalidateRect(hWnd, &sk_info->keys[prev_sel].rect, FALSE);
        if (cur_sel >= 0)
            InvalidateRect(hWnd, &sk_info->keys[cur_sel].rect, FALSE);
        //}
#endif
        return 1;
    }

    return 0;
}

int
soft_keyboard_down(HWND hWnd, int scancode, KEYEVENT * ke)
{
    GET_SOFTKEYBOARD_INFO (hWnd);

    if (cur_sel != -1 && scancode == SCANCODE_ENTER)
    {
        drawkey = downkey = cur_sel;
        if (!set_key_state(sk_info, cur_sel, ke))
            return 0;
        InvalidateRect(hWnd, &sk_info->keys[cur_sel].rect, FALSE);
        //InvalidateRect(hWnd, NULL, FALSE);
        return 1;
    }
    return 0;
}

int soft_keyboard_up(HWND hWnd, KEYEVENT * ke)
{
    int old = downkey;

    GET_SOFTKEYBOARD_INFO (hWnd);

    if (old >= 0)
    {
#if 0
        if (sk_info->keys[old].type != SFT_SCANCODE)
            return -1;
#endif

        downkey = -1;
        if (sk_info->keys[old].scancode != SCANCODE_CAPSLOCK
            && sk_info->keys[old].scancode != SCANCODE_LEFTSHIFT)
            sk_info->keys[old].status = 0;
        if (shift_valid_key(sk_info, old) && sk_info->keys[shift_index].status)
        {
            sk_info->keys[shift_index].status = 0;
            KE.status = sk_info->keys[shift_index].status << 18;
            InvalidateRect(hWnd, &sk_info->keys[shift_index].rect, TRUE);
        }
        KE.status &= ~(0x00000FFF);
        KE.status |=
            (DWORD) (sk_info->keys[capslock_index].status << 8 |
                     sk_info->keys[ctrl_index].status << 5 |
                     sk_info->keys[shift_index].status << 1);
        KE.scancode = sk_info->keys[old].scancode;
        *ke = KE;
        return 1;
    }

    return 0;
}

int receive_sk_info (MGI_SOFTKEYBOARD_INFO* sk_info, int* capslock_index,
                    int* ctrl_index, int* shift_index)
{
    int     i;
    for (i = 0; i < sk_info->nr_keys; i++)
    {
        switch (sk_info->keys[i].scancode) {
            case SCANCODE_CAPSLOCK:
                if (capslock_index)
                    *capslock_index = i;
                break;
            case SCANCODE_LEFTCONTROL:
                if (ctrl_index)
                    *ctrl_index = i;
                break;
            case SCANCODE_LEFTSHIFT:
                if (shift_index)
                    *shift_index = i;
                break;
        };
        sk_info->keys[i].status = 0;
        src_keyrec[i] = sk_info->keys[i].rect;
    }

    return 0;
}

void destroy_soft_keyboard(void)
{
    return;
}

static void
set_hit_status(HDC hdc, PRECT prec)
{
#define PEN_WIDTH   3
    POINT   pts[5];
    int half_pen_width = 0;
    if (!prec)
        return;

    half_pen_width = PEN_WIDTH/2;
    SetPenWidth (hdc, PEN_WIDTH);
    SetPenColor (hdc, RGB2Pixel(hdc, 0xff, 0x00, 0x00));

    pts[0].x = prec->left + half_pen_width;
    pts[0].y = prec->top + text_height + half_pen_width;
    pts[1].x = prec->right - 1 - half_pen_width;
    pts[1].y = pts[0].y;
    pts[2].x = pts[1].x;
    pts[2].y = prec->bottom + text_height - 1 - half_pen_width;
    pts[3].x = pts[0].x;
    pts[3].y = pts[2].y;
    pts[4] = pts[0];
    PolyLineEx (hdc, pts, 5);
#undef PEN_WIDTH
}

void refresh_soft_keyboard_bk(HWND hWnd, HDC hdc)
{
    int     top = 0;

    GET_SOFTKEYBOARD_INFO (hWnd);

    if (!win_status || !sk_info)
        return;

    if (is_input_open)
    {
        SetBrushColor(hdc, GetWindowElementColor(WE_THREED_BODY));
        FillBox(hdc, 0, 0, sk_info->skb_bmp.bmWidth, TOP);
        top = TOP;
    }

    if (is_keyboard_open)
    {

        if (sk_info->keys[shift_index].status)
            set_hit_status(hdc, &sk_info->keys[shift_index].rect);

        if (sk_info->keys[capslock_index].status)
            set_hit_status(hdc, &sk_info->keys[capslock_index].rect);

        if (cur_sel >= 0)
            set_hit_status(hdc, &sk_info->keys[cur_sel].rect);
    }

    return;
}

#define PRESSDOWN (prev_sel == capslock_index || prev_sel == shift_index)
void refresh_soft_keyboard_key(HWND hwnd, HDC hdc)
{
    PRECT   cur_rec;

    GET_SOFTKEYBOARD_INFO (hwnd);

    if (!is_keyboard_open || !sk_info)
        return;

    if (cur_sel >= 0)
    {
        cur_rec = &sk_info->keys[cur_sel].rect;
        set_hit_status(hdc, cur_rec);
    }
}

void
move_ime_window(HWND hWnd, HWND target)
{
    POINT   pos;
    HWND    medit;
    RECT    rc;
    int     maxy = GetGDCapability(HDC_SCREEN, GDCAP_MAXY);
    int     maxx = GetGDCapability(HDC_SCREEN, GDCAP_MAXX);
    int     height = 0;

    GET_SOFTKEYBOARD_INFO (hWnd);

    if (!win_status)
        return;
    medit = GetFocusChild(target);
    if (!GetCaretPos(medit, &pos))
        return;

    if (is_keyboard_open && is_input_open)
        height = sk_info->skb_bmp.bmHeight + TOP;
    else if (!is_keyboard_open && is_input_open)
        height = TOP;
    else
        height = sk_info->skb_bmp.bmHeight;

    ClientToScreen(medit, &pos.x, &pos.y);
    GetWindowRect(hWnd, &rc);
    pos.y += 10;
    if (pos.y >= rc.top && pos.y <= rc.bottom)
    {
        if (pos.y > height)
        {
            rc.top = 0;
            rc.bottom = height;
        }
        else if (pos.y <= maxy - height)
        {
            rc.top = maxy - height;
            rc.bottom = maxy;
        }
        else if (pos.x >= rc.left && pos.x <= rc.right)
        {
            if (pos.x > sk_info->skb_bmp.bmWidth)
            {
                rc.left = 0;
                rc.right = sk_info->skb_bmp.bmWidth;
            }
            else if (pos.x < maxx - sk_info->skb_bmp.bmWidth)
            {
                rc.left = maxx - sk_info->skb_bmp.bmWidth;
                rc.right = maxx;
            }
            else
                return;
        }
        else
            return;

        MoveWindow(hWnd, rc.left, rc.top, RECTW(rc), height, TRUE);
    }

    return;
}

void update_input_window(HWND hWnd)
{
    int     i = 0;
    RECT    rc = { 0 };

    GET_SOFTKEYBOARD_INFO(hWnd);

    if (!win_status)
    {
        if (IsWindowVisible(hWnd))
            ShowWindow(hWnd, SW_HIDE);
        return;
    }

    if (!IsWindowVisible(hWnd))
        ShowWindow(hWnd, SW_SHOW);

    GetWindowRect(hWnd, &rc);
    if (is_input_open && !is_keyboard_open)
    {
        MoveWindow(hWnd, rc.left, rc.top, sk_info->skb_bmp.bmWidth, TOP, FALSE);
    }
    else if (is_input_open && is_keyboard_open)
    {
        for (; i < sk_info->nr_keys; i++)
        {
            sk_info->keys[i].rect.left = src_keyrec[i].left + LEFT;
            sk_info->keys[i].rect.right = src_keyrec[i].right + LEFT;
            sk_info->keys[i].rect.top = src_keyrec[i].top + TOP;
            sk_info->keys[i].rect.bottom = src_keyrec[i].bottom + TOP;
        }

        MoveWindow(hWnd, rc.left, rc.top, sk_info->skb_bmp.bmWidth,
                   TOP + sk_info->skb_bmp.bmHeight, FALSE);
    }
    else
    {
        for (; i < sk_info->nr_keys; i++)
        {
            sk_info->keys[i].rect.left = src_keyrec[i].left;
            sk_info->keys[i].rect.right = src_keyrec[i].right;
            sk_info->keys[i].rect.top = src_keyrec[i].top;
            sk_info->keys[i].rect.bottom = src_keyrec[i].bottom;
        }

        MoveWindow(hWnd, rc.left, rc.top, sk_info->skb_bmp.bmWidth, sk_info->skb_bmp.bmHeight,
                   FALSE);
    }

    UpdateWindow(hWnd, TRUE);
}

int
active_soft_key(void *context, HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    int     update = 0;

    switch (message)
    {
    case MSG_KEYDOWN:
        if (wParam == SCANCODE_F2)
        {
            if (win_status & INPUT_OPEN)
            {
                win_status &= ~INPUT_OPEN;
                SendMessage(hWnd, MSG_MYIME_CLOSE, 0, 0);
            }
            else
            {
                win_status |= INPUT_OPEN;
                SendMessage(hWnd, MSG_MYIME_OPEN, 0, 0);
            }
            update = 1;
        }
        else if (wParam == SCANCODE_F3)
        {
            if (win_status & KEYBOARD_OPEN)
            {
                win_status &= ~KEYBOARD_OPEN;
                SendMessage(hWnd, MSG_KEYBOARD_CLOSE, 0, 0);
            }
            else
            {
                win_status |= KEYBOARD_OPEN;
                SendMessage(hWnd, MSG_KEYBOARD_OPEN, 0, 0);
            }
            update = 1;
        }
        else if (wParam == SCANCODE_F4)
        {
            if (!win_status)
            {
                win_status = KEYBOARD_OPEN | INPUT_OPEN;
                SendMessage(hWnd, MSG_MYIME_OPEN, 0, 0);
                SendMessage(hWnd, MSG_KEYBOARD_OPEN, 0, 0);
            }
            else
            {
                win_status = 0;
                SendMessage(hWnd, MSG_KEYBOARD_CLOSE, 0, 0);
                SendMessage(hWnd, MSG_MYIME_CLOSE, 0, 0);
            }
            update = 1;
        }
    }

    if (update)
        update_input_window(hWnd);

    return HOOK_GOON;
}

#if defined(_LITE_VERSION) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
extern MG_Layer* mgTopmostLayer;
#endif

static int
DefaultIMEWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

#if 1
    switch (message)
    {
    case MSG_IME_SETSTATUS:
        return 0;
    case MSG_IME_GETSTATUS:
        return 0;
    case MSG_IME_GETPOS:
        {
            POINT* pt = (POINT*)lParam;
            RECT rc;
            GetWindowRect(hWnd, &rc);
            pt->x = rc.left;
            pt->y = rc.top;
            return 0;
        }
    case MSG_IME_SETPOS:
        {
            POINT* pt = (POINT*)lParam;
            RECT rc;
            GetWindowRect(hWnd, &rc);
            if (pt->x + RECTW(rc) > RECTW(g_rcScr)) {
                pt->x = RECTW(g_rcScr) - RECTW(rc);
            }
            if ((pt->y + RECTH(rc)) > RECTH(g_rcScr)) {
                pt->y = pt->y -  RECTH(rc) - 25;
                printf("y is %d\n", pt->y);
            }
            else 
                printf("y is %d\n", pt->y);
            MoveWindow(hWnd, pt->x, pt->y, RECTW(rc), RECTH(rc), TRUE);
            return 0;
        }
    }

#endif
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static int
sk_IMEWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC     hdc;
    static BOOL fCaptured = FALSE;
    static int oldx, oldy;
    int     x, y;
    int     maxx, maxy;
    RECT    rcWindow;
    RECT   *prcExpect, *prcResult;
    KEYEVENT ke;

    GET_SOFTKEYBOARD_INFO (hWnd);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (message == MSG_KEYDOWN || message == MSG_KEYUP) {
        if (wParam < SCANCODE_KEYPAD7 || wParam > SCANCODE_KEYPADPERIOD)
        {
            if (sk_IsOpened && !GET_CUR_OPS(sk_info)->is_ascii) {
                MSG Msg = {hWnd, message, wParam, lParam};
                TranslateMessage (&Msg);
            }
        }
    }
    else if (message == MSG_SYSKEYDOWN) {
        if (sk_sg_hTargetWnd)
            PostMessage (sk_sg_hTargetWnd, message, wParam, lParam);
        else
#if (MINIGUI_MAJOR_VERSION > 1)
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, wParam, lParam);
#else
            Send2ActiveClient (MSG_KEYDOWN, wParam, lParam);
#endif
    }
    else if (message == MSG_SYSKEYUP) {
        if (sk_sg_hTargetWnd)
            PostMessage (sk_sg_hTargetWnd, message, wParam, lParam);
        else
#if (MINIGUI_MAJOR_VERSION > 1)
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, wParam, lParam);
#else
            Send2ActiveClient (MSG_KEYUP, wParam, lParam);
#endif
    }
#endif

    switch (message)
    {
    case MSG_NCCREATE:
        sk_ime_input_init (sk_info);
        break;
            
    case MSG_MYIME_OPEN:
            printf( "sk_OPEN\n" );
        if (!sk_IsOpened)
        {
            printf( "sk_OPEN\n" );
            ShowWindow (hWnd, SW_SHOW);
            sk_IsOpened = 1;
        }
        break;

    case MSG_MYIME_CLOSE:
        sk_IsOpened = 0;
        break;

    case MSG_KEYBOARD_CLOSE:
        is_keyboard_open = 0;
        break;

    case MSG_KEYBOARD_OPEN:
        is_keyboard_open = 1;
        break;

    case MSG_IME_SETTARGET:
        if (sk_sg_hTargetWnd != hWnd)
        {
            sk_sg_hTargetWnd = (HWND) wParam;
#if 0
#ifndef _LITE_VERSION
            RegisterKeyMsgHook((void *)sk_sg_hTargetWnd, active_soft_key);
#endif
#endif
        }
        break;

    case MSG_IME_GETTARGET:
        return (int)sk_sg_hTargetWnd;

    case MSG_SETFOCUS:
    case MSG_KILLFOCUS:
        return 0;

    case MSG_CHAR:
        if (sk_IsOpened && !sk_imeIsSpecialChar(wParam))
        {
            if (sk_ime_filter(sk_info, (BYTE) wParam, lParam))
                InvalidateRect(hWnd, NULL, TRUE);
        }
        return 0;

    case MSG_KEYDOWN:
        if (sk_IsOpened)
        {
            if (is_keyboard_open)
            {
                if (cursor_direct(hWnd, wParam))
                    return 0;

                if (soft_keyboard_down(hWnd, wParam, &ke))
                {
                    PostMessage(hWnd, MSG_KEYDOWN, ke.scancode, ke.status);
                    return 0;
                }
            }

            if (wParam == SCANCODE_SPACE && (lParam & KS_CTRL))
            {
                sk_toggle_half_full();
                InvalidateRect(hWnd, NULL, TRUE);
                return 0;
            }
            else if (wParam == SCANCODE_PERIOD && (lParam & KS_CTRL))
            {
                sk_toggle_punc();
                InvalidateRect(hWnd, NULL, TRUE);
                return 0;
            }
            else if ((wParam == SCANCODE_LEFTSHIFT) && (lParam & KS_CTRL))
            {
                sk_toggle_input_method(sk_info);
                InvalidateRect(hWnd, NULL, TRUE);
                return 0;
            }

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
            if (sk_sg_hTargetWnd && sk_imeIsEffectiveIME()
                && sk_imeIsSpecialKey(wParam))
                PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
#else
            if (GET_CUR_OPS(sk_info)->is_ascii ||
                    (sk_imeIsEffectiveIME () &&
                     sk_imeIsSpecialKey (wParam))){
                if (sk_sg_hTargetWnd)
                    PostMessage (sk_sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
                else
#if (MINIGUI_MAJOR_VERSION > 1)
                    Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, wParam, lParam);
#else
                    Send2ActiveClient (MSG_KEYDOWN, wParam, lParam);
#endif
            }
#endif
        }
        else if (is_keyboard_open)
        {
            if (cursor_direct(hWnd, wParam))
                return 0;

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
            if (sk_sg_hTargetWnd && !is_enter_key(wParam))
                PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
            else if (soft_keyboard_down(hWnd, wParam, &ke))
                PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, ke.scancode,
                            ke.status);
#else
            if (!is_enter_key(wParam))
            {
                if (sk_sg_hTargetWnd)
                    PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
                else
#if (MINIGUI_MAJOR_VERSION > 1)
                    Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, wParam, lParam);
#else
                    Send2ActiveClient (MSG_KEYDOWN, wParam, lParam);
#endif
            }
            else if (soft_keyboard_down(hWnd, wParam, &ke))
            {
                if (sk_sg_hTargetWnd)
                    PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, ke.scancode,
                            ke.status);
                else
#if (MINIGUI_MAJOR_VERSION > 1)
                    Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, ke.scancode, ke.status);
#else
                    Send2ActiveClient (MSG_KEYDOWN, ke.scancode, ke.status);
#endif
            }
#endif
        }
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
        else
        {
            if (sk_sg_hTargetWnd)
                PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, wParam, lParam);
            else
#if (MINIGUI_MAJOR_VERSION > 1)
                Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, wParam, lParam);
#else
                Send2ActiveClient (MSG_KEYDOWN, wParam, lParam);
#endif
        }
#endif

        return 0;

    case MSG_KEYUP:
        if (sk_IsOpened)
        {
            if (is_keyboard_open && is_direc_key(ke.scancode))
                return 0;

            if (soft_keyboard_up(hWnd, &ke) && is_keyboard_open)
                PostMessage(hWnd, MSG_KEYUP, ke.scancode, ke.status);

            if (wParam == SCANCODE_LEFTCONTROL)
            {
                sk_toggle_input_method(sk_info);
                InvalidateRect(hWnd, NULL, TRUE);
            }

#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
            if (sk_sg_hTargetWnd && sk_imeIsEffectiveIME()
                && sk_imeIsSpecialKey(wParam))
                PostMessage(sk_sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
#else
            if (GET_CUR_OPS(sk_info)->is_ascii ||
                    (sk_imeIsEffectiveIME () &&
                     sk_imeIsSpecialKey (wParam))) {
                if (sk_sg_hTargetWnd)
                    PostMessage (sk_sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
                else
#if (MINIGUI_MAJOR_VERSION > 1)
                    Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, wParam, lParam);
#else
                    Send2ActiveClient (MSG_KEYUP, wParam, lParam);
#endif
            }
#endif

            //move_ime_window(hWnd, sk_sg_hTargetWnd);
        }
        else if (is_keyboard_open)
        {
            if (is_direc_key(ke.scancode))
                return 0;

            if (sk_sg_hTargetWnd && !is_enter_key(wParam))
                PostMessage(sk_sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
            else if (soft_keyboard_up(hWnd, &ke))
                PostMessage(hWnd, MSG_KEYUP, ke.scancode, ke.status);

            //move_ime_window(hWnd, sk_sg_hTargetWnd);
        }
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
        else
        {
            if (sk_sg_hTargetWnd)
                PostMessage(sk_sg_hTargetWnd, MSG_KEYUP, wParam, lParam);
            else
#if (MINIGUI_MAJOR_VERSION > 1)
                Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, wParam, lParam);
#else
                Send2ActiveClient (MSG_KEYUP, wParam, lParam);
#endif
        }
#endif

        return 0;

    case MSG_ERASEBKGND:
        {
            HDC     hdc = (HDC) wParam;
            BOOL    fGetDC = FALSE;
            if (!hdc)
            {
                hdc = GetClientDC(hWnd);
                fGetDC = TRUE;
            }
            FillBoxWithBitmap (hdc, 0, text_height,
                               sk_info->skb_bmp.bmWidth,
                               sk_info->skb_bmp.bmHeight,
                               &sk_info->skb_bmp);
            if (fGetDC)
                ReleaseDC(hdc);
        }
        return 0;
#if 0
            HDC     hdc = (HDC) wParam;
            const RECT *clip = (const RECT *)lParam;
            BOOL    fGetDC = FALSE;
            RECT    rcTemp;

            if (!hdc)
            {
                hdc = GetClientDC(hWnd);
                fGetDC = TRUE;
            }

            if (clip)
            {
                rcTemp = *clip;
                if (IsMainWindow(hWnd))
                {
                    ScreenToClient(hWnd, &rcTemp.left, &rcTemp.top);
                    ScreenToClient(hWnd, &rcTemp.right, &rcTemp.bottom);
                }
                IncludeClipRect(hdc, &rcTemp);
            }

            refresh_soft_keyboard_bk(hWnd, hdc);

            if (fGetDC)
                ReleaseDC(hdc);
        }
#endif

    case MSG_PAINT:
        {
            if (!sk_info)
                return 0;
            hdc = BeginPaint(hWnd);
            SetBrushColor(hdc, GetWindowElementColor(WE_THREED_BODY));
            FillBox(hdc, 0, 0, sk_info->skb_bmp.bmWidth, TOP);
            SetBkColor(hdc, GetWindowElementColor(WE_THREED_BODY));
            sk_refresh_input_method_area(hWnd, hdc);
            refresh_soft_keyboard_key(hWnd, hdc);
            EndPaint(hWnd, hdc);
        }
        return 0;

    case MSG_LBUTTONDOWN:
        {
            RECT    rc;
            int     zwidth = GetSysCCharWidth();
            int     zheight = GetSysCharHeight();

            GetClientRect(hWnd, &rc);

            oldx = LOWORD(lParam);
            oldy = HIWORD(lParam);

            if (sk_IsOpened)
            {
                if (oldx < 4 * zwidth && oldx > 0 && oldy > 2
                    && oldy < zheight + 2)
                {
                    sk_toggle_input_method(sk_info);
                    InvalidateRect(hWnd, NULL, TRUE);
                    return 0;
                }

                if (oldx > rc.right - 36 && oldx < rc.right - 20 && oldy > 2
                    && oldy < 18)
                {
                    sk_toggle_half_full();
                    InvalidateRect(hWnd, NULL, TRUE);
                    return 0;
                }

                if (oldx > rc.right - 20 && oldx < rc.right - 4 && oldy > 2
                    && oldy < 18)
                {
                    sk_toggle_punc();
                    InvalidateRect(hWnd, NULL, TRUE);
                    return 0;
                }
            }

            //oldy -= text_height;

            if (is_keyboard_open && soft_keyboard_mouse_down(hWnd, oldx, oldy - text_height, &ke))
            {
                if (sk_IsOpened)
                    PostMessage(hWnd, MSG_KEYDOWN, ke.scancode, ke.status);
                else if (sk_sg_hTargetWnd)
                    PostMessage(sk_sg_hTargetWnd, MSG_KEYDOWN, ke.scancode,
                                ke.status);
                return 0;
            }

            SetCapture(hWnd);
            ClientToScreen(hWnd, &oldx, &oldy);
            fCaptured = TRUE;
        }
        break;

    case MSG_LBUTTONUP:
        if (fCaptured == TRUE)
        {
            ReleaseCapture();
            fCaptured = FALSE;
        }
        else if (is_keyboard_open && soft_keyboard_up(hWnd, &ke))
        {
            if (sk_IsOpened)
                PostMessage(hWnd, MSG_KEYUP, ke.scancode, ke.status);
            else if (sk_sg_hTargetWnd)
                PostMessage(sk_sg_hTargetWnd, MSG_KEYUP, ke.scancode,
                            ke.status);
        }
        break;

    case MSG_MOUSEMOVE:
        if (fCaptured)
        {
            GetWindowRect(hWnd, &rcWindow);
            x = LOSWORD(lParam);
            y = HISWORD(lParam);
            OffsetRect(&rcWindow, x - oldx, y - oldy);
            MoveWindow(hWnd, rcWindow.left, rcWindow.top, RECTW(rcWindow),
                       RECTH(rcWindow), TRUE);
            oldx = x;
            oldy = y;
        }
        break;

    case MSG_SIZECHANGING:
        prcExpect = (PRECT) wParam;
        prcResult = (PRECT) lParam;

        *prcResult = *prcExpect;
        if (prcExpect->left < 0)
        {
            prcResult->left = 0;
            prcResult->right = RECTWP(prcExpect);
        }
        if (prcExpect->top < 0)
        {
            prcResult->top = 0;
            prcResult->bottom = RECTHP(prcExpect);
        }

        maxx = GetGDCapability(HDC_SCREEN, GDCAP_HPIXEL);
        maxy = GetGDCapability(HDC_SCREEN, GDCAP_VPIXEL);

        if (prcExpect->right > maxx)
        {
            prcResult->right = maxx;
            prcResult->left = maxx - RECTWP(prcExpect);
        }
        if (prcExpect->bottom > maxy)
        {
            prcResult->bottom = maxy;
            prcResult->top = maxy - RECTHP(prcExpect);
        }

        return 0;

    case MSG_CLOSE:
        if (sk_sg_bmpIME)
            UnloadBitmap(sk_sg_bmpIME);
        SendMessage(HWND_DESKTOP, MSG_IME_UNREGISTER, (WPARAM) hWnd, 0);
        sk_ime_input_done(sk_info);
        DestroyMainWindow(hWnd);
#if !defined(_LITE_VERSION) || defined(_STAND_ALONE)
        PostQuitMessage(hWnd);
#endif
        return 0;
    }

    return DefaultIMEWinProc(hWnd, message, wParam, lParam);
}

static void
sk_init_createinfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_ABSSCRPOS | WS_BORDER;
    pCreateInfo->dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    pCreateInfo->spCaption = "SoftKeyboard IME";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = sk_IMEWinProc;
    pCreateInfo->lx = pCreateInfo->rx = GetGDCapability(HDC_SCREEN, GDCAP_MAXX);
    pCreateInfo->ty = pCreateInfo->by = GetGDCapability(HDC_SCREEN, GDCAP_MAXY);
    pCreateInfo->iBkColor = GetWindowElementColor(WE_THREED_BODY);
    pCreateInfo->hHosting = HWND_DESKTOP;
}

void init_sk_window (MGI_SOFTKEYBOARD_INFO* sk_info, PLOGFONT ime_font, int* is_default)
{
    FONTMETRICS metrics;

    if (sk_info->kbd_layout)
        SetKeyboardLayout (sk_info->kbd_layout);

    if (!ime_font)
        ime_font = GetSystemFont (SYSLOGFONT_DEFAULT);

    GetFontMetrics (ime_font, &metrics);
    text_height = TOP;

    if (sk_info->nr_keys == 0)
    {
        LoadBitmapFromMem(HDC_SCREEN, &sk_info->skb_bmp,
                default_kb_bmp, sizeof(default_kb_bmp), "bmp");
        sk_info->nr_keys = DEFAULT_NR_KEYS;
        sk_info->keys = default_keyboard;
        if (is_default)
            *is_default = 1;
    }

    receive_sk_info (sk_info, &capslock_index, &ctrl_index, &shift_index);
}

#ifdef _LITE_VERSION
HWND mgiCreateSoftKeyboardIME (MGI_SOFTKEYBOARD_INFO* sk_info, PLOGFONT ime_font)
{
    MAINWINCREATE CreateInfo;
    HWND sk_hwnd;
    int is_default = 0;
    //int kb_width, kb_height;

    if (!sk_info)
        return HWND_INVALID;

#ifndef _STAND_ALONE
    if (!mgIsServer)
        return HWND_INVALID;
#endif

    init_sk_window (sk_info, ime_font, &is_default);

    sk_init_createinfo (&CreateInfo);

    CreateInfo.lx -= sk_info->skb_bmp.bmWidth;
    CreateInfo.ty -= sk_info->skb_bmp.bmHeight + text_height;
    CreateInfo.dwAddData = (DWORD)sk_info;
    sk_hwnd = CreateMainWindow (&CreateInfo);
    if (sk_hwnd == HWND_INVALID)
    {
        if (is_default)
            UnloadBitmap (&sk_info->skb_bmp);
        return HWND_INVALID;
    }

    mgiAddSKBTranslateOps (sk_hwnd, &en_trans_ops);
    //SetWindowFont (sk_hwnd, ime_font);
    return sk_hwnd;
}
#else
typedef struct _IME_INFO
{
    sem_t wait;
    MGI_SOFTKEYBOARD_INFO* sk_info;
    PLOGFONT ime_font;
    HWND hwnd;
} SK_IME_INFO;

static void* start_sk_ime (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    SK_IME_INFO* sk_ime_info = (SK_IME_INFO*)data;
    HWND sk_hwnd;
    int is_default = 0;

    init_sk_window (sk_ime_info->sk_info, sk_ime_info->ime_font, &is_default);
    sk_init_createinfo (&CreateInfo);

    CreateInfo.lx -= sk_ime_info->sk_info->skb_bmp.bmWidth;
    CreateInfo.ty -= sk_ime_info->sk_info->skb_bmp.bmHeight + text_height;
    CreateInfo.dwAddData = (DWORD)sk_ime_info->sk_info;
    sk_hwnd = sk_ime_info->hwnd = CreateMainWindow (&CreateInfo);
    sem_post (&sk_ime_info->wait);

    if (sk_hwnd == HWND_INVALID)
    {
        if (is_default)
            UnloadBitmap (&sk_ime_info->sk_info->skb_bmp);
        return NULL;
    }

    mgiAddSKBTranslateOps (sk_hwnd, &en_trans_ops);

    while (GetMessage (&Msg, sk_hwnd) ) {
        TranslateMessage (&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (sk_hwnd);
    return NULL;
}

static pthread_t skimethread;

HWND mgiCreateSoftKeyboardIME (MGI_SOFTKEYBOARD_INFO* sk_info, PLOGFONT ime_font)
{
    SK_IME_INFO sk_ime_info;
    pthread_attr_t new_attr;

    if (!sk_info)
        return HWND_INVALID;

    sem_init (&sk_ime_info.wait, 0, 0);
    sk_ime_info.sk_info = sk_info;
    sk_ime_info.ime_font = ime_font;

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create (&skimethread, &new_attr, start_sk_ime, &sk_ime_info);
    pthread_attr_destroy (&new_attr);

    sem_wait (&sk_ime_info.wait);
    sem_destroy (&sk_ime_info.wait);

    return sk_ime_info.hwnd;
}
#endif

BOOL mgiAddSKBTranslateOps (HWND ime_hwnd,
                            MGI_TRANSLATE_OPS* trans_ops)
{
    int i;
    GET_SOFTKEYBOARD_INFO (ime_hwnd);

    for (i = 0; i < NR_INPUTMETHOD; i++)
    {
        if (NULL == sk_info->trans_ops[i])
        {
            sk_info->trans_ops[i] = trans_ops;
            sk_info->trans_ops[i]->cb_init ();
            return TRUE;
        }
    }
    return FALSE;
}

