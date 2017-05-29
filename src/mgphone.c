#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../include/mgi-phone.h"
#include "mgi_helper.h"

#include "mgphone_nav_bmp.c"

#define MAX_LEN_KEYSTROKES          30

#ifndef MAX_LEN_USER_WORD
#  define MAX_LEN_USER_WORD         12
#endif

#ifndef MAX_LEN_SYMBOL
#  define MAX_LEN_SYMBOL            256
#endif

#define PHONE_DEFAULT_CASE_num      0x01
#define PHONE_DEFAULT_CASE_ABC      0x02
#define PHONE_DEFAULT_CASE_abc      0x03
#define PHONE_DEFAULT_CASE_MAX      (PHONE_DEFAULT_CASE_abc + 1)

#define PHONE_ERROR_INPUT_STATUS    -1 
#define PHONE_NO_INPUT_STATUS       0
#define PHONE_SYMBOL_INPUT_STATUS   1
#define PHONE_CHAR_INPUT_STATUS     2
#define PHONE_PRED_INPUT_STATUS     3   /*predict word input*/

/*predictive ime key map*/
typedef enum {
    MGI_PHONE_KEY_LEFT,
    MGI_PHONE_KEY_RIGHT,
    MGI_PHONE_KEY_UP,
    MGI_PHONE_KEY_DOWN,
    MGI_PHONE_KEY_CLEAR,
    MGI_PHONE_KEY_ENTER,
    MGI_PHONE_KEY_STAR,
    MGI_PHONE_KEY_SHARP,
    MGI_PHONE_KEY_0,
    MGI_PHONE_KEY_1,
    MGI_PHONE_KEY_2,
    MGI_PHONE_KEY_3,
    MGI_PHONE_KEY_4,
    MGI_PHONE_KEY_5,
    MGI_PHONE_KEY_6,
    MGI_PHONE_KEY_7,
    MGI_PHONE_KEY_8,
    MGI_PHONE_KEY_9
}PHONE_KEY;

struct _MGI_PHONE_IME_DATA
{
    /* Save move flag */
    BOOL    is_left;
    BOOL    is_right;
    BOOL    is_top;
    BOOL    is_bottom;
                                            
    /* Save next and prev cursor value */
    int     next_cursor;
    int     prev_cursor;
    int     cur_cursor;

    int     next_cursor_down;
    int     prev_cursor_down;
    int     cur_cursor_down;

    int     str_num;
    int     str_down_num;
    
    /*current selected word index*/
    int     cur_index;
    int     cur_index_down;
    RECT    cur_rect;
                                                                                            
    MGI_PHONE_IME_METHOD *head_method;
    MGI_PHONE_IME_METHOD *cur_method;
                                                                                                            
    char    cur_word[PHONE_RESULT_BUFF_LONG+1];
    char    old_strokes[MAX_LEN_KEYSTROKES+1];
    char    key_strokes[MAX_LEN_KEYSTROKES+1];
    char    str[PHONE_RESULT_BUFF_LONG+1];      /* str display in up line */
    char    str_down[PHONE_RESULT_BUFF_LONG+1]; /* str display in down line */
    char    symbol[MAX_LEN_SYMBOL+1];           /* all symbol */

    MGICB_ON_PHONE_IME_SWITCHED cb_notify;      /* notify callback */
    const MGI_PHONE_KEY_MAP         (*key_map)[MAX_PHONE_KEY_NUM];
    const MGI_PHONE_IME_TEXTCOLOR   *text_color;
    BITMAP                          *bkgnd_bmp;
    
    int                         is_opened; 
    int                         pti_switch_flag;
    int                         ime_switch_flag;
    int                         ptim_case;          /* num ABC abc ...*/

    HWND                        phone_imehwnd;
    HWND                        sg_target_hwnd; 
    BOOL                        is_focus_up;        /* if focus in up line */
    int                         phone_input_status; /*PHONE_SYMBOL_INPUT_STATUS 
                                                      PHONE_NO_INPUT_STATUS 
                                                      PHONE_CHAR_INPUT_STATUS
                                                      PHONE_PRED_INPUT_STATUS
                                                      PHONE_ERROR_INPUT_STATUS*/

};
typedef struct _MGI_PHONE_IME_DATA *MGI_PHONE_IME_DATA;

HWND g_phone_hwnd = HWND_INVALID;

static void phone_adddata_init (void *data)
{
    MGI_PHONE_IME_DATA pdata = data;

    /*todo: initialize addtional data*/
    pdata->is_left = pdata->is_top = pdata->is_right = pdata->is_bottom = FALSE;
    
    pdata->prev_cursor = pdata->cur_cursor = 0;
    pdata->next_cursor = -1;

    pdata->prev_cursor_down = pdata->cur_cursor_down = 0;
    pdata->next_cursor_down = -1;

    pdata->cur_index = 0;
    pdata->cur_index_down = 0;
    SetRectEmpty(&pdata->cur_rect);

    memset(pdata->cur_word, 0, sizeof(pdata->cur_word));
    memset(pdata->old_strokes, 0, sizeof(pdata->old_strokes));
    memset(pdata->key_strokes, 0, sizeof(pdata->key_strokes));
    memset(pdata->str, 0, sizeof(pdata->str));
    memset(pdata->str_down, 0, sizeof(pdata->str_down));
    memset(pdata->symbol, 0, sizeof(pdata->symbol));

    pdata->is_focus_up = TRUE;
    pdata->phone_input_status = PHONE_NO_INPUT_STATUS; 
}

/*======================begin tradition ime======================*/

/*for traditional input method*/
static UINT prev_char = 0;
static int rep_count = 0;

static BOOL filter_trad_en_key (MGI_PHONE_IME_DATA pdata, int key, LPARAM lParam)
{
    BOOL bRep;
    unsigned char c;
    /*for long press*/
#if 0
#ifndef __NOUNIX__
#define TIME_UNIT       1000000
#define TIME_INTERVAL   800000    
    static struct timeval oldtime = {0, 0}, newtime = {0, 0};
#elif defined __VXWORKS__
#define TIME_UNIT       1000000000
#define TIME_INTERVAL   800000000 
    static struct timespec oldtime={0, 0}, newtime = {0, 0};
#endif
#endif

#define TIME_INTERVAL   80   
    static unsigned int oldtick, newtick;
    long interval;

    if (key == 127) {
        return FALSE;
    }

#if 0
    oldtime = newtime;
#ifndef __NOUNIX__
    gettimeofday (&newtime, NULL);
    interval = (newtime.tv_sec - oldtime.tv_sec)*TIME_UNIT + 
                (newtime.tv_usec - oldtime.tv_usec);
#elif defined __VXWORKS__
    clock_gettime(CLOCK_REALTIME, &newtime);
    interval = (newtime.tv_sec - oldtime.tv_sec)*TIME_UNIT + 
                (newtime.tv_nsec - oldtime.tv_nsec);
#endif
#endif
    oldtick = newtick;
    newtick = GetTickCount();
    interval = newtick - oldtick;

    if (prev_char && prev_char == key && interval < TIME_INTERVAL) {
        rep_count ++;
        bRep = TRUE;
    }
    else {
        rep_count = 0;
        bRep = FALSE;
    }

    prev_char = key;

    if (key > 0) {
        const char *pchar;
        pchar =(*pdata->key_map)[key].letters;
        
        if (!pchar)
            return FALSE;

        c = *(pchar+rep_count);
        if ( isdigit(c) && rep_count > 0) {
            rep_count = 0;
            c = *(pchar);
        }
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
        if (bRep) {
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYDOWN, (*pdata->key_map)[MGI_PHONE_KEY_CLEAR].scancode, 0);
            Send2ActiveWindow (mgTopmostLayer, MSG_KEYUP, (*pdata->key_map)[MGI_PHONE_KEY_CLEAR].scancode, 0);
        }

        if (pdata->ptim_case == PHONE_DEFAULT_CASE_ABC)
            Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, toupper(c), 0);
        else
            Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, c, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
        if (bRep) {
            PostMessage(pdata->sg_target_hwnd, MSG_KEYDOWN, (*pdata->key_map)[MGI_PHONE_KEY_CLEAR].scancode, 0);
            PostMessage(pdata->sg_target_hwnd, MSG_KEYUP, (*pdata->key_map)[MGI_PHONE_KEY_CLEAR].scancode, 0);
        }

        if (pdata->ptim_case == PHONE_DEFAULT_CASE_ABC)
            PostMessage(pdata->sg_target_hwnd, MSG_CHAR, toupper(c), 0);
        else
            PostMessage(pdata->sg_target_hwnd, MSG_CHAR, c, 0);
#endif
    }

    return TRUE;
}

static void 
process_trad_en_num_key (MGI_PHONE_IME_DATA pdata, int message, int key)
{
    if (message == MSG_KEYDOWN) 
        filter_trad_en_key(pdata, key, 0);
}
/*======================end tradition ime======================*/

/*
 *fn: static void phone_switch_mode (MGI_PHONE_IME_DATA pdata)
 * */
static void phone_switch_mode (MGI_PHONE_IME_DATA pdata)
{
    int method_num=0;
    MGI_PHONE_IME_METHOD *p=NULL;

    if (!pdata)
        return;

    p = pdata->head_method;
    while(p) {
        p = p->next;
        method_num++;
    }

    if (pdata->ptim_case < (PHONE_DEFAULT_CASE_MAX + method_num - 1))
        pdata->ptim_case++;
    else 
        pdata->ptim_case = PHONE_DEFAULT_CASE_num;

    if (pdata->ptim_case >= PHONE_DEFAULT_CASE_MAX) {
        int i = pdata->ptim_case - PHONE_DEFAULT_CASE_MAX;
        p = pdata->head_method;
        while (i && p) {
            p=p->next;
            i--;
        }
        pdata->cur_method = p;
    }
}

static int get_words_num(const char* buff) {
    int n = 0;
    char *p = NULL;

    p = strchr (buff, ' ');

    while (p != NULL) {
        n++;
        p++;

        p = strchr(p, ' ');
    }
        
    return n;
}

static char* 
get_word(const char* buff, int index, char *word, int len) {
    char *p = NULL;
    int n = 0;

    if (strlen(buff) <= 0)
        return NULL;

    p = strchr (buff, ' ');

    if (index == 0) {
        strncpy(word, buff, p - buff);
        word[p-buff] = '\0';
        return word;
    }

    while (p != NULL) {
        n++;
        if (n == index) {
            char *t;
            p++;
            t = strchr(p, ' ');

            if (t == NULL) {
                strcpy (word, p);
            }
            else {
                strncpy (word, p, t-p);
                word[t-p] = '\0';
            }

            return word;
        }
        p++;
        p = strchr(p, ' ');
    }
    return NULL;
}

static BOOL 
get_substr_pos_ex (HDC hdc, const char* str, const char* substr, RECT *rc)
{

    int fit, startpos;
    int pos[2 *PHONE_RESULT_BUFF_LONG + 1]={0};
    int dx[2 * PHONE_RESULT_BUFF_LONG + 1]={0};
    SIZE txtsize;
    char *p = strstr (str, substr);

    if (!p)
        return FALSE;

    /*memset (pos, 0, sizeof (pos));*/
    /*memset (dx, 0, sizeof (dx));*/

    GetTextExtentPoint (hdc, 
        (const char*)str, 
        strlen(str), 0, 
        &fit,
        pos,
        dx,
        &txtsize);

    startpos = p - str;

    rc->top = 0;
    rc->left = dx [startpos];
    rc->right = dx [startpos + strlen(substr)] + 1;
    rc->bottom = rc->top + GetFontHeight (hdc) + 1; 
    
    return TRUE;
}

static void get_cur_word_rect(HWND hwnd)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);
    char *p=NULL;
    HDC hdc = GetDC(hwnd);
    
    if(pdata->is_focus_up)
        p=pdata->str;
    else
        p=pdata->str_down;

    if (isascii(pdata->cur_word[0])) {
        get_substr_pos_ex (hdc, 
            p, 
            pdata->cur_word, 
            &pdata->cur_rect);
    }
    else {
        SIZE textsize, cur_size;
        char tmp[PHONE_RESULT_BUFF_LONG]={0};
        char *t;
        int n;

        /* maybe string have same word, so need ensure by word length  */
        t = strstr(p, pdata->cur_word);
        while(t) {
            n = get_words_num(t);
            if (pdata->is_focus_up) {
                if (pdata->cur_index == pdata->str_num-n) {
                    strncpy(tmp, p, t-p);
                    tmp[t-p] = '\0';
                    break;
                }
                else {
                    t++;
                    t = strstr(t, pdata->cur_word);
                }
            }
            else {
                if (pdata->cur_index_down == pdata->str_down_num-n) {
                    strncpy(tmp, p, t-p);
                    tmp[t-p] = '\0';
                    break;
                }
                else {
                    t++;
                    t = strstr(t, pdata->cur_word);
                }
            }
        }

        GetTextExtent(hdc, pdata->cur_word, strlen(pdata->cur_word), &cur_size);
        GetTextExtent(hdc, tmp, strlen(tmp), &textsize);
        
        SetRect(&pdata->cur_rect, 
                textsize.cx, 
                0,
                textsize.cx+cur_size.cx+1,
                textsize.cy+1);
    }

    if (!pdata->is_focus_up) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        OffsetRect(&pdata->cur_rect, 0 , RECTH(rc)/2);
    }
    ReleaseDC (hdc);
}

static void send_word (MGI_PHONE_IME_DATA pdata, char *word)
{
    int i = 0; 
    int len = strlen (word);
    WORD    wDByte;
    BOOL    bDByte=FALSE;
    
    if (!isascii(word[0]))
        bDByte=TRUE;

    if (bDByte)
    {
        for (i = 0; i < len; i += 2)
        {
            wDByte = MAKEWORD(word[i], word[i + 1]);
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
            Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, wDByte, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
            PostMessage(pdata->sg_target_hwnd, MSG_CHAR, wDByte, 0);
#endif
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
            Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, word[i], 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
#ifdef __NOUNIX__
            SendMessage(pdata->sg_target_hwnd, MSG_CHAR, word[i], 0);
#else
            PostMessage(pdata->sg_target_hwnd, MSG_CHAR, word[i], 0);
#endif
#endif
        }
    }
}


static void handle_add_data (HWND hwnd, MGI_PHONE_IME_DATA pdata)
{
    char cur_word[64]={0};
    
    /*memset (cur_word, 0, 64);*/
    if (pdata->is_focus_up){
        if (pdata->cur_index > pdata->str_num - 1)
            pdata->cur_index = pdata->str_num - 1;
        get_word(pdata->str, pdata->cur_index, cur_word, 64);
    }
    else {
        if (pdata->cur_index_down > pdata->str_down_num - 1)
            pdata->cur_index_down = pdata->str_down_num - 1;
        get_word(pdata->str_down, pdata->cur_index_down, cur_word, 64);
    }

    if (cur_word) {
        strcpy (pdata->cur_word, cur_word);
        get_cur_word_rect(hwnd);
    }
}

static void update_state(HWND hwnd)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);

    if (pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS){
        if (pdata->is_focus_up) {
            if (pdata->cur_index == 0)
                pdata->is_left = FALSE;
            else
                pdata->is_left = TRUE;

            if (pdata->cur_index == pdata->str_num - 1)
                pdata->is_right = FALSE;
            else
                pdata->is_right = TRUE;
        }
        else {
             if (pdata->cur_index_down == 0)
                pdata->is_left = FALSE;
            else
                pdata->is_left = TRUE;

            if (pdata->cur_index_down == pdata->str_down_num - 1)
                pdata->is_right = FALSE;
            else
                pdata->is_right = TRUE;
        }

        if (pdata->prev_cursor == pdata->cur_cursor)
            pdata->is_top = FALSE;
        else
            pdata->is_top = TRUE;

        if (pdata->next_cursor == -1)
            pdata->is_bottom = FALSE;
        else
            pdata->is_bottom = TRUE;
    }
    else {
        if (pdata->is_focus_up) {
            if (pdata->cur_index == 0)
                pdata->is_left = FALSE;
            else
                pdata->is_left = TRUE;

            if (pdata->cur_index == pdata->str_num - 1)
                pdata->is_right = FALSE;
            else
                pdata->is_right = TRUE;

            if (pdata->prev_cursor == pdata->cur_cursor)
                pdata->is_top = FALSE;
            else
                pdata->is_top = TRUE;

            if (pdata->next_cursor == -1)
                pdata->is_bottom = FALSE;
            else
                pdata->is_bottom = TRUE;
        }
        else {
            if (pdata->cur_index_down == 0)
                pdata->is_left = FALSE;
            else
                pdata->is_left = TRUE;

            if (pdata->cur_index_down == pdata->str_down_num - 1)
                pdata->is_right = FALSE;
            else
                pdata->is_right = TRUE;

            if (pdata->prev_cursor_down == pdata->cur_cursor_down)
                pdata->is_top = FALSE;
            else
                pdata->is_top = TRUE;

            if (pdata->next_cursor_down == -1)
                pdata->is_bottom = FALSE;
            else
                pdata->is_bottom = TRUE;
        }
    }
}

static void get_translate_word(MGI_PHONE_IME_DATA pdata, int index, int cursor)
{
    char buff_translate[PHONE_RESULT_BUFF_LONG+1]={0};
    char buff[PHONE_RESULT_BUFF_LONG+1]={0};
    int tmp;
    
    if (!pdata)
        return;

    get_word(pdata->str, index, buff_translate, PHONE_RESULT_BUFF_LONG); 
    /*memset(buff, 0, sizeof(buff));*/
    tmp = pdata->cur_method->translate_word((void *)pdata->cur_method, buff_translate, 
            buff, PHONE_RESULT_BUFF_LONG, cursor);
    if (tmp == -1 && strlen(buff) < 1){
        pdata->str_down[0]='\0';
        return;
    }
    strncpy(pdata->str_down, buff, sizeof(pdata->str_down));
    pdata->str_down_num = get_words_num(pdata->str_down);
}

static void handle_predict_word(HWND hwnd, int cursor)
{
    char buff[PHONE_RESULT_BUFF_LONG+1]={0};
    int next, prev, tmp;
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);
    
    /* get predict word */
    //strcpy(pdata->str, pdata->cur_word);

    if ( !pdata->cur_method->predict_word || 
        strlen(pdata->str) <= 0 ||
        cursor == -1)
        return;

    //memset(buff, 0, sizeof(buff));
    prev = 0;
    /* Get prev cursor */
    if(pdata->cur_cursor_down != 0) {
        while(prev != -1) {
            tmp = pdata->cur_method->predict_word((void *)pdata->cur_method, 
                        pdata->str, buff, PHONE_RESULT_BUFF_LONG, prev);
            //fprintf(stderr, "predict 0, next=%d\n", tmp);
            if (tmp == cursor)
                break;
            prev = tmp;
        }
    }

    /* Get next cursor and get matched buff */
    next = pdata->cur_method->predict_word((void *)pdata->cur_method, 
                pdata->str, buff, PHONE_RESULT_BUFF_LONG, cursor);
    if (next == -1 && strlen(buff) < 1){
        pdata->phone_input_status = PHONE_NO_INPUT_STATUS;
        phone_adddata_init(pdata);
        return;
    }

    /* Update data */
    if (prev < 0)
        prev = 0;
    pdata->prev_cursor_down = prev;
    pdata->cur_cursor_down = cursor;

    strncpy(pdata->str_down, buff, PHONE_RESULT_BUFF_LONG);
    pdata->str_down_num = get_words_num(pdata->str_down);
#if 0
    if (next != -1) {
        tmp = pdata->cur_method->predict_word((void *)pdata->cur_method, 
                pdata->str, buff, PHONE_RESULT_BUFF_LONG, next);
        if (tmp == -1 && strlen(buff) < 1)
            pdata->next_cursor = tmp;
        else
            pdata->next_cursor = next;
    }
    else
        pdata->next_cursor = next;
#else
    pdata->next_cursor_down = next;
#endif

    if(pdata->str_down_num > 0) {
        pdata->is_focus_up = FALSE;
        pdata->cur_index_down = 0;
    }

    update_state(hwnd);
}

static void handle_translate_word(HWND hwnd, int index, int cursor)
{
    char buff[PHONE_RESULT_BUFF_LONG+1]={0};
    char buff_translate[PHONE_RESULT_BUFF_LONG+1]={0};
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);
    int next, prev, tmp;
    
    if (cursor == -1)
        return;

    /* get translate word */
    if ( !pdata->cur_method->translate_word || strlen(pdata->str) <= 0)
        return;

    /*memset(buff, 0, sizeof(buff));*/
    /*memset(buff_translate, 0, sizeof(buff_translate));*/

    get_word(pdata->str, index, buff_translate, PHONE_RESULT_BUFF_LONG); 

    prev = 0;
    /* Get prev cursor */
    if(pdata->cur_cursor_down != 0) {
        while(prev != -1) {
            if (pdata->cur_method->translate_word) {
                tmp = pdata->cur_method->translate_word((void *)pdata->cur_method, 
                        buff_translate, buff, PHONE_RESULT_BUFF_LONG, prev);
                if (tmp == cursor)
                    break;
                prev = tmp;
            }
        }
    }

    /* Get next cursor and get matched buff */
    next = pdata->cur_method->translate_word((void *)pdata->cur_method, 
                buff_translate, buff, PHONE_RESULT_BUFF_LONG, cursor);
    if (next == -1 && strlen(buff) < 1){
        fprintf(stderr, "handle_translate_data next is NULL, return\n");
        return;
    }

    /* Update data */
    if (prev == -1)
        prev = 0;
    pdata->prev_cursor_down = prev;
    pdata->cur_cursor_down = cursor;
    strncpy(pdata->str_down, buff, PHONE_RESULT_BUFF_LONG);
    pdata->str_down_num = get_words_num(pdata->str_down);

    if (next != -1) {
        tmp = pdata->cur_method->translate_word((void *)pdata->cur_method, 
                buff_translate, buff, PHONE_RESULT_BUFF_LONG, next);
        if (tmp == -1 && strlen(buff) < 1)
            pdata->next_cursor_down = tmp;
        else
            pdata->next_cursor_down = next;
    }
    else
        pdata->next_cursor_down = next;

    if (pdata->cur_index_down > pdata->str_down_num - 1)
        pdata->cur_index_down = pdata->str_down_num - 1;

    update_state(hwnd);
}

static void  handle_data (HWND hwnd, const char *stokes, int cursor)
{
    char buff[PHONE_RESULT_BUFF_LONG+1]={0};
    int next, prev, tmp;
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);

    if (cursor == -1 || stokes == NULL)
        return;
    if (pdata->cur_method == NULL)
        return;

    /* Get prev cursor */
    prev = 0;

    if (cursor != 0) {
        while (prev != -1) {
            if (pdata->cur_method->match_keystrokes) {
                tmp = pdata->cur_method->match_keystrokes((void *)pdata->cur_method, stokes, buff, PHONE_RESULT_BUFF_LONG, prev);
                if (tmp == cursor) 
                    break;
                prev = tmp;
            }
        }
    }

    /* Get next cursor and get matched buff */
    next = pdata->cur_method->match_keystrokes((void *)pdata->cur_method, stokes, buff, PHONE_RESULT_BUFF_LONG, cursor);
    if (next == -1 && strlen(buff) < 1)
        return;

    /* Update data */
    if (prev == -1)
        prev = 0;

    pdata->prev_cursor = prev;
    pdata->cur_cursor = cursor;

    strncpy(pdata->key_strokes ,stokes, sizeof(pdata->key_strokes));
    strncpy(pdata->str, buff, PHONE_RESULT_BUFF_LONG);
    pdata->str_num = get_words_num(pdata->str);
#if 1
    if (next != -1) {
        tmp = pdata->cur_method->match_keystrokes((void *)pdata->cur_method, stokes, buff, PHONE_RESULT_BUFF_LONG, next);
        if (tmp == -1 && strlen(buff) < 1) {
            //fprintf(stderr, "handle_data now have no value\n");
            pdata->next_cursor = tmp;
        }
        else
            pdata->next_cursor = next;
    }
    else
        pdata->next_cursor = next;
#else
    pdata->next_cursor = next;
#endif

    if (pdata->cur_index > pdata->str_num - 1)
        pdata->cur_index = pdata->str_num - 1;

    update_state(hwnd);
#if 0 
    /* get translate word */
    get_translate_word(pdata, pdata->cur_index, 0);
#endif
}

static int get_default_symbol(MGI_PHONE_IME_DATA pdata)
{
    const char *pletters;
    char * tmp;
    int i=0;

    if (!pdata)
        return -1;

    pletters=((*pdata->key_map)[MGI_PHONE_KEY_1]).letters;
    if (!pletters)
        return -1;

    tmp = pdata->symbol;
    while (pletters && !isdigit(*pletters))
    {
       tmp[i++] = *pletters++;
       tmp[i++] = ' ';
       if (i >= MAX_LEN_SYMBOL)
           break;
    }
    return i;
}

static int copy_word_from_source (const char* source, char* buff, int copied)
{
    int len;
    char* space;

    while (source) {
        space = strchr (source, ' ');
        if (space == NULL) {
            len = strlen (source) + 1;
            if (len == 1)
                return copied;

            if (copied + len < PHONE_RESULT_BUFF_LONG) {
                strcpy (buff + copied, source);
                copied += len;
                buff [copied - 1] = ' ';
            }
            return copied;
        }
        else {
            len = space - source + 1;
            if (len == 1)
                return copied;

            if (copied + len < PHONE_RESULT_BUFF_LONG) {
                strncpy (buff + copied, source, len);
                copied += len;
                source += len;
            }
            else
                return copied;
        }
    }
    return copied;
}

/*
 *handle symbol
 * */
void handle_symbol_data(MGI_PHONE_IME_DATA pdata, int cursor)
{
    char *pstr=NULL, *cur, *prev;
    char tmp[2][PHONE_RESULT_BUFF_LONG+1];
    int len, up_len;
    /*int down_len;*/

    cur = (char *)cursor;
    if (cur <= 0)
        cur = pdata->symbol;
    else if ( cur < pdata->symbol || 
              cur > pdata->symbol + sizeof(pdata->symbol)) 
    {
        fprintf(stderr, "handle_symbol_data, cur is too large\n");
        return;
    }

    prev = pdata->symbol;
    if (prev != cur) {
        int len[2]={0};
        while (prev < (pdata->symbol+strlen(pdata->symbol))) {
            len[0] = copy_word_from_source(prev, tmp[0], 0);
            len[1] = copy_word_from_source(prev+len[0], tmp[1], 0);

            if (prev+len[0]+len[1] >= cur)
                break;
            prev += len[0]+len[1];
        }
    }

    pstr = cur;
    up_len = strlen(pdata->str);
    /*memset(pdata->str, 0, sizeof(pdata->str));*/
    pdata->str[0] = '\0';
    len = copy_word_from_source(pstr, pdata->str, 0);

    /*down_len = strlen(pdata->str_down);*/

    /*memset(pdata->str_down, 0, sizeof(pdata->str_down));*/
    pdata->str_down[0] = '\0';
    pstr += len;
    if ( pstr < pdata->symbol + sizeof(pdata->symbol))
        len = copy_word_from_source(pstr, pdata->str_down, 0);

    pdata->str_num = get_words_num(pdata->str);
    pdata->str_down_num = get_words_num(pdata->str_down);
    if (pdata->is_focus_up) {
        if (pdata->cur_index > pdata->str_num -1)
            pdata->cur_index = pdata->str_num -1;
    }
    else {
        if (pdata->cur_index_down > pdata->str_down_num -1)
            pdata->cur_index_down = pdata->str_down_num -1;
    }

    pdata->prev_cursor = (int)prev;
    if (pdata->prev_cursor < (int)pdata->symbol )
        pdata->prev_cursor = (int)pdata->symbol;

    pdata->cur_cursor = (int)cur;
    pdata->next_cursor = (int)(pstr + len);

    if ( pdata->next_cursor >=(int)(pdata->symbol + strlen(pdata->symbol)))
        pdata->next_cursor = -1;
}

static void show_prev_page(HWND hwnd)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);
    int cursor;

    if (pdata->is_focus_up ||
        pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS)
        cursor = pdata->prev_cursor;
    else
        cursor = pdata->prev_cursor_down;

    if(pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS) {
        pdata->is_focus_up = FALSE;
        pdata->cur_index_down = pdata->cur_index;
        handle_symbol_data(pdata, cursor);
    }
    else if (pdata->phone_input_status == PHONE_PRED_INPUT_STATUS)
        handle_predict_word(hwnd, cursor);
    else 
        handle_translate_word(hwnd, pdata->cur_index, cursor);
}

static void show_next_page(HWND hwnd)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);
    int cursor;

    if (pdata->is_focus_up ||
        pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS)
        cursor = pdata->next_cursor;
    else
        cursor = pdata->next_cursor_down;

    if(pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS) {
        if (cursor == -1)
            return;
        pdata->is_focus_up = TRUE;
        pdata->cur_index = pdata->cur_index_down;
        handle_symbol_data(pdata, cursor);
    }
    else if (pdata->phone_input_status == PHONE_PRED_INPUT_STATUS)
        handle_predict_word(hwnd, cursor);
    else 
        handle_translate_word(hwnd, pdata->cur_index, cursor);
}


int get_scancode_index(MGI_PHONE_IME_DATA pdata, int scancode)
{
    int i;
    for (i = 0;i < MAX_PHONE_KEY_NUM; i++) {
        if ( ( *(pdata->key_map) )[i].scancode == scancode )
            return i;
    }
    return -1;
}

static int 
process_en_num_key (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);

    int index = get_scancode_index(pdata, LOWORD(wParam));
    if (message == MSG_KEYDOWN) {
        /* Handle the key 1 for punctuation marks and special characters*/
        if (index == MGI_PHONE_KEY_1){
            if (pdata->phone_input_status == PHONE_NO_INPUT_STATUS ||
                 pdata->phone_input_status == PHONE_PRED_INPUT_STATUS) 
            {
                phone_adddata_init(pdata);
                pdata->phone_input_status = PHONE_SYMBOL_INPUT_STATUS;
                pdata->cur_cursor = 0;

                if (pdata->ptim_case >= PHONE_DEFAULT_CASE_MAX && 
                    pdata->cur_method->get_symbol){
                    pdata->cur_method->get_symbol(pdata, pdata->symbol, MAX_LEN_SYMBOL);
                }
                else
                    get_default_symbol(pdata);

                handle_symbol_data(pdata, pdata->cur_cursor);
                update_state(hwnd);
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        else if (index == MGI_PHONE_KEY_0) {
            /* Handle the key 0*/
            char *cur_word = pdata->cur_word;
            int nr_word = strlen (cur_word);

            /*send word and send space key*/ 
            if (nr_word == 0) {
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
                Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, ' ', 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
                PostMessage(pdata->sg_target_hwnd, MSG_CHAR, ' ', 0);
#endif
            }
            else if ( ! pdata->is_focus_up ||
                    (pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS)) {
                /*send word to target window*/
                send_word (pdata, cur_word);
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
                Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, ' ', 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
                PostMessage(pdata->sg_target_hwnd, MSG_CHAR, ' ', 0);
#endif
                phone_adddata_init (pdata);
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        else if (index >= MGI_PHONE_KEY_2 && index <= MGI_PHONE_KEY_9) {
            char c[2];
            char stokes[MAX_LEN_KEYSTROKES+1];

            if (pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS)
                return 1;
            else if(pdata->phone_input_status == PHONE_PRED_INPUT_STATUS)
                phone_adddata_init(pdata);

            pdata->phone_input_status=PHONE_CHAR_INPUT_STATUS;

            /*c[0] = wParam - 1 + '0';*/
            c[0] = index - MGI_PHONE_KEY_0 + '0';
            c[1] = '\0';
            if (strlen(pdata->key_strokes) < MAX_LEN_KEYSTROKES) {
                strcpy(stokes, pdata->key_strokes);
                strcat (stokes, c);
            }
            else
                return 1;

            pdata->is_focus_up = TRUE;
            pdata->cur_index = 0;
            handle_data(hwnd, stokes, 0);
            //fprintf(stderr, "handle_data over\n");

            /* get translate word */
            get_translate_word(pdata, pdata->cur_index, 0);
            //fprintf(stderr, "get_translate_word over\n");

            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
    return 0;
}

static void process_left_key(HWND hwnd, MGI_PHONE_IME_DATA pdata)
{
    char cur_word[64];

    if (pdata->is_focus_up) {
        if (pdata->cur_index <= 0) {
            if( pdata->prev_cursor != pdata->cur_cursor &&
                pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS) 
            {
                handle_data(hwnd, pdata->key_strokes, pdata->prev_cursor);
                pdata->cur_index = pdata->str_num-1;
                if (pdata->cur_index < 0)
                    pdata->cur_index = 0;

                /* get translate word */
                get_translate_word(pdata, pdata->cur_index, 0);
            }
            return;
        }
    }
    else {
        if (pdata->cur_index_down <= 0)
            return;
    }

    if (pdata->is_focus_up){
        pdata->cur_index--;
        get_word(pdata->str, pdata->cur_index, cur_word, 64);
    }
    else{
        pdata->cur_index_down--;
        get_word(pdata->str_down, pdata->cur_index_down, cur_word, 64);
    }

    if (cur_word) {
        strcpy (pdata->cur_word, cur_word);
        get_cur_word_rect(hwnd);
    }
    /* Update arraw state */
    if (pdata->is_focus_up && pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
        get_translate_word(pdata, pdata->cur_index, 0);
#if 0
    if(pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
        update_state(hwnd);
#endif
}

static void process_right_key(HWND hwnd, MGI_PHONE_IME_DATA pdata)
{
    if (pdata->is_focus_up && pdata->cur_index >= pdata->str_num - 1) 
    {
        if( pdata->next_cursor != -1 && 
            pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
        {/* in up line, right key can page possible*/
            //fprintf(stderr, "right key, page\n");
            handle_data(hwnd, pdata->key_strokes, pdata->next_cursor);
            pdata->cur_index = 0;

            /* get translate word */
            get_translate_word(pdata, pdata->cur_index, 0);
        }
        return;
    }
    else if (!pdata->is_focus_up && pdata->cur_index_down >= pdata->str_down_num -1)
        return;
    else {
        char cur_word[64];
        if (pdata->is_focus_up){
            pdata->cur_index++;
            get_word(pdata->str, pdata->cur_index, cur_word, 64);
        }
        else{
            pdata->cur_index_down++;
            get_word(pdata->str_down, pdata->cur_index_down, cur_word, 64);
        }

        if (cur_word) {
            strcpy (pdata->cur_word, cur_word);
            get_cur_word_rect(hwnd);
        }
    }
    /* Update arraw state */
    if (pdata->is_focus_up && pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
        get_translate_word(pdata, pdata->cur_index, 0);
#if 0
    if(pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
        update_state(hwnd);
#endif
}

static void process_up_key(HWND hwnd, MGI_PHONE_IME_DATA pdata)
{
    if (pdata->is_focus_up) {
        /* symbol input */
        if (pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS &&
            pdata->prev_cursor != pdata->cur_cursor)
            show_prev_page(hwnd);
    }
    else {
        if(pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS) 
        {/*when input symbol*/
            pdata->is_focus_up = TRUE;
            pdata->cur_index = pdata->cur_index_down;
        }
        else {
            if (pdata->prev_cursor_down == pdata->cur_cursor_down)
            {/*the beginning of content in down line, now switch focus to up line*/

                /* when predict input, focus can not move to up*/
                if (pdata->phone_input_status == PHONE_PRED_INPUT_STATUS) {
                    return;
                }
                pdata->is_focus_up = TRUE;
                pdata->cur_index_down = 0;
            }
            else
                show_prev_page(hwnd);
        }
    }
    if(pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
        update_state(hwnd);
}

/* handle down key */
static void process_down_key(HWND hwnd, MGI_PHONE_IME_DATA pdata)
{
    if (pdata->is_focus_up) {
        pdata->is_focus_up = FALSE;

        /* handle symbol input */
        if(pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS) {
           if ( strlen(pdata->str_down) <= 0) {
                pdata->is_focus_up = TRUE;
                return;
           }
           pdata->str_down_num = get_words_num(pdata->str_down);
           if (pdata->cur_index > strlen(pdata->str_down))
               pdata->cur_index_down = pdata->str_down_num -1;
           else
               pdata->cur_index_down = pdata->cur_index;
        }
        else {/* handle char input */
            pdata->cur_index_down = 0;
            handle_translate_word(hwnd, pdata->cur_index, 0);
        }
    }
    else /* focus in down line */
        show_next_page(hwnd);
#if 0
    if(pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
        update_state(hwnd);
#endif
}

static int 
process_navigator_key (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);

    if ( pdata->phone_input_status == PHONE_NO_INPUT_STATUS) {
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
        Send2ActiveWindow (mgTopmostLayer, message, wParam, lParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
        PostMessage(pdata->sg_target_hwnd, message, wParam, lParam);
#endif
        return 0;
    }
    else if (message == MSG_KEYDOWN) {
        int index = get_scancode_index(pdata, LOWORD(wParam));
        switch (index) {
            case MGI_PHONE_KEY_LEFT:
                process_left_key(hwnd, pdata);
                break;
            case MGI_PHONE_KEY_RIGHT:
                process_right_key(hwnd, pdata);
                break;
            case MGI_PHONE_KEY_UP:
                process_up_key(hwnd, pdata);
                break;
            case MGI_PHONE_KEY_DOWN:
                process_down_key(hwnd, pdata);
                break;
        }
#if 0
        if(pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS)
            update_state(hwnd);
#else
        update_state(hwnd);
#endif
        InvalidateRect(hwnd, NULL, TRUE);
    }
    return 0;
}

static int 
process_confirm_key (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);
    char *cur_word = pdata->cur_word;

    if (pdata->phone_input_status == PHONE_NO_INPUT_STATUS) {
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
        Send2ActiveWindow (mgTopmostLayer, message, wParam, lParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
        PostMessage(pdata->sg_target_hwnd, message, wParam, lParam);
#endif
        return 0;
    }
    else if (pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS) {
        send_word(pdata, cur_word);
        phone_adddata_init (pdata);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }
    else {
        if (pdata->is_focus_up){
            /*get next level word list*/
            pdata->is_focus_up = FALSE;
            pdata->cur_index_down = 0;
            handle_translate_word(hwnd, pdata->cur_index, 0);
        }
        else {
            send_word(pdata, cur_word);
            if (pdata->cur_method && 
                pdata->cur_method->predict_word)
            {
                pdata->phone_input_status = PHONE_PRED_INPUT_STATUS;
                strncpy(pdata->str, pdata->cur_word, sizeof(pdata->str));
                handle_predict_word(hwnd, 0);
            }
            else 
                phone_adddata_init (pdata);
        }
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }
    return 0;
}

static int 
process_clear_key (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(hwnd);

    if (pdata->phone_input_status == PHONE_NO_INPUT_STATUS) {
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
        Send2ActiveWindow (mgTopmostLayer, message, wParam, lParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
        PostMessage(pdata->sg_target_hwnd, message, wParam, lParam);
#endif
        return 0;
    }
    else if (message == MSG_KEYDOWN) {
        int len = strlen(pdata->key_strokes);
        if ( pdata->phone_input_status != PHONE_PRED_INPUT_STATUS &&
             len > 1 ) 
        {
            if (pdata->old_strokes[0]) {
                handle_data(hwnd, pdata->old_strokes, 0);
                memset (pdata->old_strokes, 0, MAX_LEN_KEYSTROKES);
            }
            else {
                pdata->key_strokes[len -1] = '\0';
                handle_data(hwnd, pdata->key_strokes, 0);
            }
            pdata->is_focus_up = TRUE;
            pdata->cur_index = 0;

            /* get translate word */
            get_translate_word(pdata, pdata->cur_index, 0);
        }
        else 
        {
            phone_adddata_init (pdata);
        }
        InvalidateRect (hwnd, 0, TRUE);
    }
    return 0;
}

static int 
process_sharp_key (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);
    phone_switch_mode (pdata);

    if(pdata->cb_notify) {
        int method_id;
        if (pdata->ptim_case < PHONE_DEFAULT_CASE_MAX)
            method_id = pdata->ptim_case;                    
        else {
            if (pdata->cur_method)
                method_id = pdata->cur_method->method_id;
            else {
                fprintf(stderr, "process_sharp_key error\n");
                return -1;
            }
        }
        pdata->cb_notify((void *)pdata, method_id);
    }
    return 0;
}

static int 
process_star_key (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);

    if (message == MSG_KEYDOWN) {
        if (pdata->cur_method &&
            pdata->cur_method->status_changed)
        {
            pdata->cur_method->status_changed((void *)pdata, 0);
            phone_adddata_init (pdata);
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
    return 0;
}

static void draw_nav_bmp (HWND hwnd, HDC hdc, void *data)
{
    MGI_PHONE_IME_DATA pdata = data;
    int bw = 0, bh = 0, bmp_nr = 15;
    int y=0;
    RECT cli_rc;
    
    GetClientRect (hwnd, &cli_rc);

    if (!pdata->is_focus_up || 
        pdata->phone_input_status == PHONE_SYMBOL_INPUT_STATUS)
        y = RECTH(cli_rc)/2;
#if 0
    fprintf(stderr, "left=%d, top=%d, right=%d, bottom=%d\n", 
            pdata->is_left, pdata->is_top, pdata->is_right, pdata->is_bottom);
#endif

    bw = navigator_bmp.bmWidth/bmp_nr;
    bh = navigator_bmp.bmHeight;
    /*left navigator:9*/
    if (pdata->is_left) {
        FillBoxWithBitmapPart (hdc, 
                0, y, bw, bh, 
                0, 0, &navigator_bmp, 9 * bw, 0);
    } 

    /*top-right-bottom navigator:2*/
    if (pdata->is_right && pdata->is_top && pdata->is_bottom) {
        //GetClientRect (hwnd, &cli_rc);
        FillBoxWithBitmapPart (hdc, 
                cli_rc.right - bw, y, bw, bh, 
                0, 0, &navigator_bmp, 2 * bw, 0);
        return;
    }
    else if (pdata->is_right && pdata->is_top ) {
        /*top-right navigator:6*/
        //GetClientRect (hwnd, &cli_rc);
        FillBoxWithBitmapPart (hdc, 
                cli_rc.right - bw, y, bw, bh, 
                0, 0, &navigator_bmp, 6 * bw, 0);
        return;
    }
    else if (pdata->is_right && pdata->is_bottom) {
        /*right-bottom navigator:5*/
        //GetClientRect (hwnd, &cli_rc);
        FillBoxWithBitmapPart (hdc, 
                cli_rc.right - bw, y, bw, bh, 
                0, 0, &navigator_bmp, 5 * bw, 0);
        return;
    }
    else if (pdata->is_right) {
        /*right navigator:10*/
        //GetClientRect (hwnd, &cli_rc);
        FillBoxWithBitmapPart (hdc, 
                cli_rc.right - bw, y, bw, bh, 
                0, 0, &navigator_bmp, 10 * bw, 0);
        return;
    }
    else if (pdata->is_top && pdata->is_bottom) {
        /*top-bottom navigator:14*/
        //GetClientRect (hwnd, &cli_rc);
        FillBoxWithBitmapPart (hdc, 
                cli_rc.right - bw, y, bw, bh, 
                0, 0, &navigator_bmp, 14 * bw, 0);
        return;
    }
    else if (pdata->is_top) {
        /*top navigator:11*/
        //GetClientRect (hwnd, &cli_rc);
        FillBoxWithBitmapPart (hdc, 
                cli_rc.right - bw, y, bw, bh, 
                0, 0, &navigator_bmp, 11 * bw, 0);
        return;
    }
    else if (pdata->is_bottom) {
        /*bottom navigator:12*/
        //GetClientRect (hwnd, &cli_rc);
        FillBoxWithBitmapPart (hdc, 
                cli_rc.right - bw, y, bw, bh, 
                0, 0, &navigator_bmp, 12 * bw, 0);
        
        return;
    }
}

/* internal functions */
static void refresh_input_method_area (HWND hwnd, HDC hdc)
{
    static RECT rc;
    RECT clientrc, tmprc;
    Uint32 prev_color=0;
    int off_x = 0;
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA) GetWindowAdditionalData(hwnd);

    SetBkMode (hdc, BM_TRANSPARENT);

    if(pdata->bkgnd_bmp) {
        FillBoxWithBitmap(hdc, 0, 0, 0, 0, pdata->bkgnd_bmp);
    }

    if (pdata->str[0] == 0)
    {
        SetRectEmpty (&rc);
        SetRectEmpty (&pdata->cur_rect);
        ShowWindow (hwnd, SW_HIDE);
        return;
    }
    handle_add_data (hwnd, pdata);

    if (!IsRectEmpty (&pdata->cur_rect) && IsRectEmpty (&rc))
        ShowWindow (hwnd, SW_SHOW);

    /*clear previous drawing rect*/
    if (!IsRectEmpty (&rc)) {
#if 0
        prev_color = SetPenColor (hdc, GetWindowBkColor (hwnd));
        Rectangle (hdc, rc.left, rc.top, rc.right, rc.bottom);
#else
        prev_color = SetBrushColor(hdc, GetWindowBkColor(hwnd));
        FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
#endif
        SetPenColor (hdc, prev_color);
    }

    /*draw navigator key*/
    draw_nav_bmp (hwnd, hdc, pdata);

    if (!IsRectEmpty (&pdata->cur_rect)) {

        CopyRect (&rc, &pdata->cur_rect);
        /*offset current rect*/
        //if (pdata->is_left || pdata->is_top)
        {
            off_x = navigator_bmp.bmWidth/30;
            OffsetRect (&rc, off_x, 0);
        }

        /*draw current rect*/
#if 0
        prev_color = SetPenColor (hdc, PIXEL_black);
        Rectangle (hdc, rc.left, rc.top, rc.right, rc.bottom);
        SetPenColor (hdc, prev_color);
#else
        if (pdata->text_color) {
            if (pdata->is_focus_up)
                prev_color = SetBrushColor(hdc, pdata->text_color->firstline_focus_color);
            else
                prev_color = SetBrushColor(hdc, pdata->text_color->secondline_focus_color);
        }
        else
            prev_color = SetBrushColor(hdc, PIXEL_darkblue);

        FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
        SetBrushColor(hdc, prev_color);
#endif
    }

    /* fillbox focus region*/
    GetClientRect(hwnd, &clientrc);
    OffsetRect(&clientrc, navigator_bmp.bmWidth/30, 0);
    SetRect(&tmprc, clientrc.left, clientrc.top, 
                    clientrc.right-2 * navigator_bmp.bmWidth/30, 
                    clientrc.top+RECTH(clientrc)/2);

    /* draw first line text*/
    if (pdata->text_color)
        prev_color = SetTextColor(hdc, pdata->text_color->firstline_text_color);
    DrawText(hdc, pdata->str, -1, &tmprc, DT_NOCLIP|DT_SINGLELINE|DT_LEFT);
    if (pdata->text_color)
        SetTextColor(hdc, prev_color);

    /* draw second line text*/
    if (strlen(pdata->str_down)) { 
        if (pdata->text_color)
            prev_color = SetTextColor(hdc, pdata->text_color->secondline_text_color);
        OffsetRect(&tmprc, 0, RECTH(clientrc)/2);
        DrawText(hdc, pdata->str_down, -1, &tmprc, DT_NOCLIP|DT_SINGLELINE|DT_LEFT);
        
        if (pdata->text_color)
            SetTextColor(hdc, prev_color);
    }
    
    /* draw focus text*/
    if (pdata->text_color)
    {
        if (pdata->is_focus_up)
            prev_color = SetTextColor(hdc, pdata->text_color->firstline_focus_text_color);
        else
            prev_color = SetTextColor(hdc, pdata->text_color->secondline_focus_text_color);
    }
    else
        prev_color = SetTextColor(hdc, PIXEL_lightwhite);

    DrawText(hdc, pdata->cur_word, -1, &rc, DT_NOCLIP|DT_SINGLELINE|DT_LEFT);
    SetTextColor(hdc, prev_color);
}

int phone_ime_win_proc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    MGI_PHONE_IME_DATA pdata;
    HDC   hdc;

    switch (message) {
        case MSG_NCCREATE:{
            RegisterIMEWindow (hwnd);
            break;
        }
        case MSG_CREATE: {
            if (!(pdata = (MGI_PHONE_IME_DATA) calloc (1, sizeof (struct _MGI_PHONE_IME_DATA)))){
                fprintf(stderr, "phone_ime_win_proc calloc MGI_PHONE_IME_DATA failed\n");
                return -1;
            }

            pdata->ptim_case = PHONE_DEFAULT_CASE_num;
            pdata->next_cursor = -1;
            pdata->next_cursor_down = -1;
            /*phone_adddata_init (pdata);*/
            SetWindowAdditionalData(hwnd, (DWORD) pdata);
            break;
        }
        case MSG_IME_OPEN:{
            pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(hwnd);
            if (! pdata->is_opened)
            {
                ShowWindow (hwnd, SW_SHOW);
                pdata->is_opened = 1;
            }
            break;
        }
        case MSG_IME_CLOSE:{
            pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(hwnd);
            if (pdata->is_opened) {
                ShowWindow (hwnd, SW_HIDE);
                pdata->is_opened = 0;
            }
            break;
        }
        case MSG_IME_SETTARGET:{
            pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(hwnd);
            if (pdata->sg_target_hwnd!= hwnd)
                pdata->sg_target_hwnd = (HWND)wParam;
            break;
        }
        case MSG_IME_GETTARGET:{
            pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(hwnd);
            return (int)pdata->sg_target_hwnd;
            break;
        }
        case MSG_SETFOCUS:
        case MSG_KILLFOCUS:
            return 0;
        case MSG_KEYDOWN: {
            int scancode_index;
            pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(hwnd);

            if (!pdata->is_opened) {
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
                Send2ActiveWindow (mgTopmostLayer, message, wParam, lParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
                PostMessage(pdata->sg_target_hwnd, message, wParam, lParam);
#endif
                return 0;
            }

            scancode_index = get_scancode_index(pdata, LOWORD(wParam));

            /*swith ime mode*/
            if ( scancode_index == MGI_PHONE_KEY_SHARP && message == MSG_KEYDOWN) {
                process_sharp_key (hwnd, message, wParam, lParam);
                phone_adddata_init(pdata);
                InvalidateRect(hwnd, NULL, FALSE);
                break;
            }

            if ( scancode_index == MGI_PHONE_KEY_STAR && message == MSG_KEYDOWN) {
                process_star_key (hwnd, message, wParam, lParam);
                return 0;
            }

            /*process predictive english ime*/
            switch (scancode_index) {
                case MGI_PHONE_KEY_0:
                case MGI_PHONE_KEY_1:
                case MGI_PHONE_KEY_2:
                case MGI_PHONE_KEY_3:
                case MGI_PHONE_KEY_4:
                case MGI_PHONE_KEY_5:
                case MGI_PHONE_KEY_6:
                case MGI_PHONE_KEY_7:
                case MGI_PHONE_KEY_8:
                case MGI_PHONE_KEY_9:
                    /*===================process ime num mode=======================*/
                    if (pdata->ptim_case == PHONE_DEFAULT_CASE_num) {
                        if (pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS) {
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
                            Send2ActiveWindow (mgTopmostLayer, message, wParam, lParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
                            PostMessage(pdata->sg_target_hwnd, message, wParam, lParam);
#endif
                        }
                        break;
                    }

                    /*process traditional english ime*/
                    if (pdata->ptim_case < PHONE_DEFAULT_CASE_MAX) {
                        if (pdata->phone_input_status != PHONE_SYMBOL_INPUT_STATUS) {
                            process_trad_en_num_key (pdata, message, scancode_index);
                        }
                        break;
                    }
                    process_en_num_key (hwnd, message, wParam, lParam);
                    break;

                case MGI_PHONE_KEY_LEFT:
                case MGI_PHONE_KEY_RIGHT:
                case MGI_PHONE_KEY_UP:
                case MGI_PHONE_KEY_DOWN:
                    process_navigator_key (hwnd, message, wParam, lParam);
                    break;

                case MGI_PHONE_KEY_CLEAR:
                    process_clear_key (hwnd, message, wParam, lParam);
                    break;

                case MGI_PHONE_KEY_ENTER:
                    process_confirm_key (hwnd, message, wParam, lParam);
                    break;
                default:
                    break;
            }
            return 0;
        }
        case MSG_PAINT:{
            hdc = BeginPaint (hwnd);
            refresh_input_method_area (hwnd, hdc);
            EndPaint (hwnd, hdc);
            break;
        }
        case MSG_CLOSE:
            SendMessage(HWND_DESKTOP, MSG_IME_UNREGISTER, (WPARAM) hwnd, 0);
            DestroyMainWindow(hwnd);
            PostQuitMessage (hwnd);
            return 0;
    }

    return DefaultMainWinProc(hwnd, message, wParam, lParam);
}

void init_phoneimewin_createinfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_ABSSCRPOS | WS_BORDER | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_TOPMOST;
    pCreateInfo->spCaption = "Phone IME Window";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = phone_ime_win_proc;
    pCreateInfo->lx = GetGDCapability (HDC_SCREEN, GDCAP_MAXX) - 200; 
    pCreateInfo->ty = GetGDCapability (HDC_SCREEN, GDCAP_MAXY) - 32; 
    pCreateInfo->rx = GetGDCapability (HDC_SCREEN, GDCAP_MAXX);
    pCreateInfo->by = GetGDCapability (HDC_SCREEN, GDCAP_MAXY);
    pCreateInfo->iBkColor = GetWindowElementColor(WE_THREED_BODY);
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = 0;
}


HWND mgi_phone_create_win (HWND hosting, int lx, int ty, int rx, int by)
{
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;

    init_phoneimewin_createinfo(&CreateInfo);

    if (lx != 0 || ty != 0 || rx != 0 || by != 0) {
        CreateInfo.lx = lx;
        CreateInfo.ty = ty;
        CreateInfo.rx = rx;
        CreateInfo.by = by;
    }

    hMainWnd = CreateMainWindow (&CreateInfo);
    CreateInfo.hHosting = hosting;

    if (hMainWnd == HWND_INVALID)
        return HWND_INVALID;

    return hMainWnd;
}

HWND mgiPhoneIMEWindowEx (int lx, int ty, int rx, int by)
{
    HWND ptiMainWnd = mgi_phone_create_win(HWND_DESKTOP, lx, ty, rx, by);

    if(HWND_INVALID == ptiMainWnd) {
        fprintf (stderr, "Can not create pti ime window. \n");
        return HWND_INVALID;
    }
    return ptiMainWnd;
}

#ifdef _MGRM_PROCESSES
HWND mgiCreatePhoneKeyPadIME(RECT *rect, BITMAP *bkgnd_bmp, PLOGFONT log_font, const MGI_PHONE_IME_TEXTCOLOR *textcolor, \
       const MGI_PHONE_KEY_MAP (*key_map)[MAX_PHONE_KEY_NUM], MGICB_ON_PHONE_IME_SWITCHED on_ime_switched)
{
    HWND hPhoneWnd;

#ifndef _STAND_ALONE
    if (!mgIsServer)
        return HWND_INVALID;
#endif

    if (rect == NULL) {
        fprintf(stderr, "Can not create mgiphone ime window, rect is NULL\n");
        return HWND_INVALID;
    }

    hPhoneWnd = mgiPhoneIMEWindowEx(rect->left, rect->top, rect->right, rect->bottom);
    if( hPhoneWnd != HWND_INVALID) {
        MGI_PHONE_IME_DATA mgi_phone_ime_data=(MGI_PHONE_IME_DATA)GetWindowAdditionalData(hPhoneWnd);
        if (mgi_phone_ime_data == NULL) {
            mgi_phone_ime_data = (MGI_PHONE_IME_DATA)calloc(1, sizeof(struct _MGI_PHONE_IME_DATA));
            if (mgi_phone_ime_data == NULL) {
                fprintf(stderr, "calloc failed\n");
                return -1;
            }
        }
        mgi_phone_ime_data->cb_notify = on_ime_switched;
        mgi_phone_ime_data->text_color= (MGI_PHONE_IME_TEXTCOLOR *)textcolor;
        mgi_phone_ime_data->key_map = key_map;//here has a warn: assignment from incompatible pointer type
        mgi_phone_ime_data->bkgnd_bmp= bkgnd_bmp;

        if (log_font)
            SetWindowFont(hPhoneWnd, log_font);
    }
    g_phone_hwnd = hPhoneWnd;
    return hPhoneWnd;
}

#else
typedef struct _IME_INFO
{
    sem_t wait;
    RECT *rect;
    BITMAP *bkgnd_bmp;
    PLOGFONT log_font;
    const  MGI_PHONE_IME_TEXTCOLOR *textcolor;
    const MGI_PHONE_KEY_MAP (*key_map)[MAX_PHONE_KEY_NUM];
    MGICB_ON_PHONE_IME_SWITCHED on_ime_switched;
    HWND hwnd;
} PHONE_IME_INFO;

static void* start_phone_ime (void* data)
{
    MSG Msg;
    PHONE_IME_INFO* phone_ime_info = (PHONE_IME_INFO*)data;
    HWND phone_hwnd;
    MGI_PHONE_IME_DATA mgi_phone_ime_data;

    phone_hwnd = mgiPhoneIMEWindowEx(phone_ime_info->rect->left, 
                                     phone_ime_info->rect->top, 
                                     phone_ime_info->rect->right, 
                                     phone_ime_info->rect->bottom);
    if( phone_hwnd == HWND_INVALID)
        return NULL;

    g_phone_hwnd = phone_hwnd;

    mgi_phone_ime_data = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(phone_hwnd);
    if (mgi_phone_ime_data == NULL) {
        mgi_phone_ime_data =(MGI_PHONE_IME_DATA)calloc(1, sizeof(struct _MGI_PHONE_IME_DATA));
        if (mgi_phone_ime_data == NULL) {
            fprintf(stderr, "calloc failed\n");
            return NULL;
        }
    }
    mgi_phone_ime_data->cb_notify = phone_ime_info->on_ime_switched;
    mgi_phone_ime_data->text_color= phone_ime_info->textcolor;
    mgi_phone_ime_data->key_map = phone_ime_info->key_map;
    mgi_phone_ime_data->bkgnd_bmp= phone_ime_info->bkgnd_bmp;

    if (phone_ime_info->log_font)
        SetWindowFont(phone_hwnd, phone_ime_info->log_font);
    
    //mgi_phone_ime_data->is_opened = 1;
    phone_ime_info->hwnd = phone_hwnd;
    sem_post (&phone_ime_info->wait);

    while (GetMessage (&Msg, phone_hwnd) ) {
        if (Msg.message == MSG_KEYDOWN || Msg.message == MSG_KEYUP) {
            if (mgi_phone_ime_data->is_opened) {
                TranslateMessage (&Msg);
            }
            else if (mgi_phone_ime_data->sg_target_hwnd) {
                PostMessage (mgi_phone_ime_data->sg_target_hwnd, 
                            Msg.message, Msg.wParam, Msg.lParam | KS_IMEPOST);
            }
        }
        //TranslateMessage (&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (phone_hwnd);
    return NULL;
}

static pthread_t phone_ime_thread;
HWND mgiCreatePhoneKeyPadIME (RECT *rect, BITMAP *bkgnd_bmp, PLOGFONT log_font, const MGI_PHONE_IME_TEXTCOLOR *textcolor, \
       const MGI_PHONE_KEY_MAP(*key_map)[MAX_PHONE_KEY_NUM], MGICB_ON_PHONE_IME_SWITCHED on_ime_switched)
{
    PHONE_IME_INFO phone_ime_info;
    pthread_attr_t new_attr;

    if (!rect)
        return HWND_INVALID;

    sem_init (&phone_ime_info.wait, 0, 0);
    phone_ime_info.log_font = log_font;
    phone_ime_info.rect = rect;
    phone_ime_info.bkgnd_bmp = bkgnd_bmp;
    phone_ime_info.log_font = log_font;
    phone_ime_info.textcolor = textcolor;
    phone_ime_info.key_map = key_map;
    phone_ime_info.on_ime_switched = on_ime_switched;

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create (&phone_ime_thread, &new_attr, start_phone_ime, &phone_ime_info);
    pthread_attr_destroy (&new_attr);

    sem_wait (&phone_ime_info.wait);
    sem_destroy (&phone_ime_info.wait);

    return phone_ime_info.hwnd;
}
#endif

#define MGPHONE_REQID     (MAX_SYS_REQID + 1)

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
typedef struct _MGPHONE_REQUEST_DATA {
    int op_id;
    MGI_PHONE_IME_METHOD method; 
    int cursor;
    int case_mode;
} MGPHONE_REQUEST_DATA;

typedef struct _MGPHONE_REPLY_DATA {
    int cursor;
    char buff [PHONE_RESULT_BUFF_LONG];
} MGPHONE_REPLY_DATA;

typedef enum {
    OPID_GET_METHOD_CODE,
    OPID_GET_CUR_METHOD,
    OPID_SET_CUR_METHOD
}REQUEST_TYPE;

#endif

BOOL mgi_phone_ime_get_curr_methodcode(char *code, int code_len, int method_id)
{
    MGI_PHONE_IME_DATA pdata;

    if(!code || g_phone_hwnd == HWND_INVALID)
        return FALSE;

    pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(g_phone_hwnd);
    if (!pdata)
        return FALSE;
    if (method_id > 0 && method_id < PHONE_DEFAULT_CASE_MAX) {
        switch(method_id)
        {
            case PHONE_DEFAULT_CASE_num:
                strncpy(code, "123", code_len);
                break;
            case PHONE_DEFAULT_CASE_ABC:
                strncpy(code, "ABC", code_len);
                break;
            case PHONE_DEFAULT_CASE_abc:
                strncpy(code, "abc", code_len);
                break;
            default:
                break;
        }
        return TRUE;
    }
    else {
        if (pdata->cur_method && pdata->cur_method->method_id == method_id){
            strncpy(code, pdata->cur_method->method_name, code_len);
            return TRUE;
        }
        else {
            MGI_PHONE_IME_METHOD *p = pdata->head_method;
            while(p) {
                if (p->method_id == method_id){
                    strncpy(code, p->method_name, code_len);
                    return TRUE;
                }
                p = p->next;
            }
        }
    }
    return FALSE;

}

BOOL mgiPhoneKeyPadGetMethodCode(char *code, int code_len, int method_id)
{
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (!mgIsServer) {
        REQUEST req;
        MGPHONE_REQUEST_DATA data = {0};
        MGPHONE_REPLY_DATA reply;

        data.op_id = OPID_GET_METHOD_CODE;
        data.cursor = method_id;

        req.id = MGPHONE_REQID;
        req.data = &data;
        req.len_data = sizeof(data);

        ClientRequest (&req, &reply, sizeof (reply));
        strncpy(code, reply.buff, code_len);
        return TRUE;
    } else
        return mgi_phone_ime_get_curr_methodcode(code, code_len, method_id);
#else
    return mgi_phone_ime_get_curr_methodcode(code, code_len, method_id);
#endif
}

BOOL mgiPhoneKeyPadRemoveMethod(HWND ime_hwnd, int method_id)
{    
    MGI_PHONE_IME_DATA pdata=NULL;
    MGI_PHONE_IME_METHOD *p=NULL, *prev=NULL;
    int i=0;

    if (g_phone_hwnd == HWND_INVALID || method_id < 1)
        return FALSE;

    pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(g_phone_hwnd);
    if (pdata == NULL)
        return FALSE;

    prev = p = pdata->head_method;
    while (p) {
        if(p->method_id == method_id)
            break;
        prev = p;
        p = p->next;
        i++;
    }

    if (p) 
    {
        if ( p == pdata->head_method) {
            pdata->head_method = p->next;
            if (pdata->cur_method == p) {
                pdata->cur_method = p->next;
                if (! pdata->cur_method )
                    pdata->ptim_case = PHONE_DEFAULT_CASE_num;
            }
        }
        else {
            prev->next = p->next;
            if (pdata->cur_method == p) {
                if (p->next)
                    pdata->cur_method = p->next;
                else {
                    pdata->cur_method = prev;
                    pdata->ptim_case --;
                }
                /*need notice and init*/
            }
        }
        phone_adddata_init(pdata);
        InvalidateRect(g_phone_hwnd, NULL, TRUE);
        return TRUE;
    }
    return FALSE;
}

int mgi_phone_ime_set_currmethod(int method_id)
{
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(g_phone_hwnd);
    MGI_PHONE_IME_METHOD *p=NULL;
    int i=0;

    if (method_id < 0)
        return -1;
    if (!pdata)
        return -1;

    p = pdata->head_method;
    while(p) {
        if (p->method_id == method_id){
            if (pdata->cur_method) {
                if (pdata->cur_method->actived)
                    pdata->cur_method->actived(pdata, FALSE);
            }
            pdata->cur_method = p;
            pdata->ptim_case = PHONE_DEFAULT_CASE_MAX + i;

            /* notify */
            if (pdata->cb_notify)
                pdata->cb_notify((void *)pdata, method_id);
            if (pdata->cur_method->actived)
                pdata->cur_method->actived(pdata, TRUE);
            return 0;
        }
        p = p->next;
        i++;
    }
    return -1;
}

int mgiPhoneKeyPadSetCurrMethod(int method_id)
{
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (!mgIsServer) {
        REQUEST req;
        MGPHONE_REQUEST_DATA data = {0};
        MGPHONE_REPLY_DATA reply;

        data.op_id = OPID_SET_CUR_METHOD;
        data.cursor = method_id;

        req.id = MGPHONE_REQID;
        req.data = &data;
        req.len_data = sizeof(data);

        ClientRequest (&req, &reply, sizeof (reply));
        return 0;
    } else
        return mgi_phone_ime_set_currmethod(method_id);
#else
    return mgi_phone_ime_set_currmethod(method_id);
#endif
}

int mgiPhoneKeyPadGetCurrMethod()
{
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (!mgIsServer) {
        REQUEST req;
        MGPHONE_REQUEST_DATA data = {0};
        MGPHONE_REPLY_DATA reply;

        data.op_id = OPID_GET_CUR_METHOD;

        req.id = MGPHONE_REQID;
        req.data = &data;
        req.len_data = sizeof(data);

        ClientRequest (&req, &reply, sizeof (reply));
        return reply.cursor;
    } 
    else {
        MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(g_phone_hwnd);
        if (!pdata)
            return -1;
        if( pdata->cur_method)
            return pdata->cur_method->method_id;
        else
            return pdata->ptim_case;
    }
#else
    MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(g_phone_hwnd);
    if (!pdata)
        return -1;
    if( pdata->cur_method)
        return pdata->cur_method->method_id;
    else
        return pdata->ptim_case;
#endif
}

BOOL mgiPhoneKeyPadAddMethod(HWND ime_hwnd, MGI_PHONE_IME_METHOD *method_data)
{
    MGI_PHONE_IME_DATA pdata=NULL;
    MGI_PHONE_IME_METHOD *p=NULL;

    if (g_phone_hwnd == HWND_INVALID || method_data == NULL)
        return FALSE;

    pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(g_phone_hwnd);
    if (pdata == NULL)
        return FALSE;

    p = pdata->head_method;
    if (p == NULL)/*first method*/
        pdata->head_method = method_data;
    else {
        while(p) {
            if (p->method_id == method_data->method_id) {
                return FALSE;
            }
            if (p->next == NULL)
                break;
            p = p->next;
        }
        if (p) { 
            p->next = method_data;
            method_data->next = NULL;
        }
    }
    return TRUE;
}

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
int mgphone_handler (int cli, int clifd, void* buff, size_t len)
{
    MGPHONE_REQUEST_DATA* data = (MGPHONE_REQUEST_DATA*) buff;
    MGPHONE_REPLY_DATA reply;
    switch (data->op_id) {
        case OPID_GET_METHOD_CODE:
            mgi_phone_ime_get_curr_methodcode(reply.buff, sizeof(reply.buff), data->cursor);
            break;
        case OPID_GET_CUR_METHOD: {
            MGI_PHONE_IME_DATA pdata = (MGI_PHONE_IME_DATA)GetWindowAdditionalData(g_phone_hwnd);
            if (!pdata)
                reply.cursor = -1;
            if( pdata->cur_method)
                reply.cursor = pdata->cur_method->method_id;
            else
                reply.cursor = pdata->ptim_case;
            break;
        }
        case OPID_SET_CUR_METHOD:
            mgi_phone_ime_set_currmethod(data->cursor);
            break;
        default:
            break;
    }
    return ServerSendReply (clifd, &reply, sizeof(reply));
}

BOOL mgiPhoneKeyPadServerInstallRequest(void)
{
    return RegisterRequestHandler(MGPHONE_REQID, mgphone_handler);
}

#endif
