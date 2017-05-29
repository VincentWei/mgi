#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#ifdef __MGI_LIB__
#include "mgi-phone.h"
#include "mgpti.h"
#include "ime_pinyin.h"
#else
#include <mgi/mgi-phone.h>
#include <mgi/mgpti.h>
#include <mgi/ime_pinyin.h>
#endif

#define MSG_PHONE_METHOD_SWITCH MSG_USER+13
#define MSG_PHONE_METHOD_STATUS MSG_USER+14

HWND g_phone_ime_hwnd;

int pti_case=PTI_CASE_abc;

int mgi_pti_match_keystrokes(void *method_data, const char *strokes, char *buffer, int buffer_len, int index)
{
    //return ptiMatchIcwKeystokes(strokes, buffer, index, PTI_CASE_abc);
    return ptiMatchKeystokes(strokes, buffer, buffer_len, index, pti_case);
}

int mgi_pti_translate_word(void *method_data, const char *strokes, char *buffer, int buffer_len, int index)
{
    char tmp[30+1]={0};
            
    if (strokes[strlen(strokes)-1] != '*') {
        if (strlen(strokes) < buffer_len - 2)
            sprintf(buffer, "%s ", strokes);
        else {
            strncpy(buffer, strokes, buffer_len);
            buffer[buffer_len - 1] = " ";
            buffer[buffer_len] = "\0";
        }
        return 0;
    }

    strncpy(tmp, strokes, sizeof(tmp)-1);
    if (pti_case == PTI_CASE_Abc) {
        tmp[0] = tolower(tmp[0]);
    }
    else if (pti_case == PTI_CASE_ABC) {
        int i;
        for(i=0;i<strlen(strokes);i++)
            tmp[i]=tolower(tmp[i]);
    }
    return ptiMatchKeystokes(tmp, buffer, buffer_len, index, pti_case);
}

int mgi_pti_pridict_word(void *method_data, const char *lookfor_str, char *buffer, int buffer_len, int index)
{
    return 0;
}

int mgi_pti_status_changed(void *method_data, DWORD status)
{
    HWND htargetwnd;
    if (pti_case < PTI_CASE_abc)
        pti_case++;
    else
        pti_case = PTI_CASE_ABC;

#if defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
    Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_STATUS, (DWORD)pti_case, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
    htargetwnd = SendMessage(g_phone_ime_hwnd, MSG_IME_GETTARGET, 0, 0);
    PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, (DWORD)pti_case, 0 );
#endif
    

    return 0;
}

int mgi_pti_active(void *method_data,BOOL is_active)
{
    return 0;
}

int pinyin_get_symbol(void *method_data, char *buff, int buff_len)
{
    //char punctuate[] = "£¬ ¡£ ¡¢ £º £» £¡ ¡¤ £¿ ££ £¤ £¥ ¡æ ¡« ¡Á £¨ £© ¡² ¡³ ¡¶ ¡· ¡´ ¡µ ¡¾ ¡¿ ¡¼ ¡½ ¡º";
    char punctuate[] = "£¬ ¡£ ¡¢ £º £» £¡ ¡¤ £¿ ¡® ¡¯ ¡° ¡± ££ £¤ £¥ ¡æ ¡« ¡Á £¨ £© ¡² ¡³ ¡­¡­ ©¤©¤ ¡¶ ¡· ¡´ ¡µ ¡¾ ¡¿ ¡¼ ¡º ¡» ¡¸ ¡¹ ";

    if (buff == NULL || buff_len <= 0)
        return -1;
    if (strlen(punctuate) < buff_len)
        strcpy(buff, punctuate);
    else {
        strncpy(buff, punctuate, buff_len);
        buff[buff_len+1] = '\0';
    }
    return strlen(buff);
}

MGI_PHONE_KEY_MAP default_map[MAX_PHONE_KEY_NUM] = 
{
    {SCANCODE_CURSORBLOCKLEFT, ""},
    {SCANCODE_CURSORBLOCKRIGHT, ""},
    {SCANCODE_CURSORBLOCKUP, ""},
    {SCANCODE_CURSORBLOCKDOWN, ""},
    {SCANCODE_BACKSPACE, ""},
    {SCANCODE_ENTER, ""},
    {SCANCODE_F8, ""},
    {SCANCODE_F9, ""},
    {SCANCODE_0, " 0"},
    {SCANCODE_1, ",.;:!?\"'-()@/_+&%*=<>$[]{}\\~^`1"},
    {SCANCODE_2, "abc2"},
    {SCANCODE_3, "def3"},
    {SCANCODE_4, "ghi4"},
    {SCANCODE_5, "jkl5"},
    {SCANCODE_6, "mno6"},
    {SCANCODE_7, "pqrs7"},
    {SCANCODE_8, "tuv8"},
    {SCANCODE_9, "wxyz9"},
};

MGI_PHONE_IME_METHOD pti_method = 
{
    100,
    "mgpti",
    NULL,
    mgi_pti_match_keystrokes,
    mgi_pti_translate_word,
    NULL,
    mgi_pti_status_changed,
    mgi_pti_active,
    NULL,
    NULL,
    NULL
};


MGI_PHONE_IME_METHOD pinyin_method = 
{
    101,
    "pinyin",
    NULL,
    pinyin_match_keystokes,
    pinyin_translate_word,
    pinyin_predict_pord,
    NULL,
    NULL,
    pinyin_get_symbol,
    NULL,
    NULL
};

BITMAP bmp;
PLOGFONT plogfont = INV_LOGFONT;


int mgphone_method_switch_notify(void *method_data, int new_method_id)
{
#if defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
    Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_SWITCH, (DWORD)new_method_id, 0);
    if (new_method_id == pti_method.method_id)
        Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_STATUS, (DWORD)pti_case, 0);
    else
        Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_STATUS, 0, 0);

#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
    HWND htargetwnd = SendMessage(g_phone_ime_hwnd, MSG_IME_GETTARGET, 0, 0);
    PostMessage(htargetwnd, MSG_PHONE_METHOD_SWITCH, (DWORD)new_method_id, 0 );
    if (new_method_id == pti_method.method_id)
        PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, (DWORD)pti_case, 0);
    else
        PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, 0, 0);
#endif
    return 0;
}

HWND create_test_phone_ime_wnd()
{
    RECT rect;
    HWND hphonewnd;
    //int i;

#ifdef  PHONE_SCREEN_WIDTH_128
    SetRect(&rect, 0, 270, 128, 320);
#endif
#ifdef  PHONE_SCREEN_WIDTH_176
    SetRect(&rect, 0, 270, 176, 320);
#endif
#ifdef  PHONE_SCREEN_WIDTH_240
    SetRect(&rect, 0, 270, 240, 320);
#endif

#if defined(_MGRM_PROCESSES)
    mgiPhoneKeyPadServerInstallRequest();
#endif
    //LoadBitmapFromFile(HDC_SCREEN, &bmp, "bkg.bmp");
    //plogfont = CreateLogFontIndirect
    ime_pinyin_init(); 
    hphonewnd = mgiCreatePhoneKeyPadIME(&rect, NULL, NULL, NULL, default_map, mgphone_method_switch_notify);
    
    g_phone_ime_hwnd = hphonewnd;

    mgiPhoneKeyPadAddMethod(hphonewnd, &pti_method);

    if ( mgiPhoneKeyPadAddMethod(hphonewnd, &pinyin_method) ) 
        mgiPhoneKeyPadSetCurrMethod(pinyin_method.method_id);
    else
        fprintf(stderr, "mgiPhoneKeyPadAddMethod failed\n");

    return hphonewnd;
}
