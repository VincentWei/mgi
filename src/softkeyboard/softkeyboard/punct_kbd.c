
/*
 * wangjian<wangjian@minigui.org>
 * 2008-7-11
*/

#include "common.h"

#ifdef KBD_TOOLTIP
#include "tooltip.h"
#endif

static md_key_t punct_key_pads[] = {
    /***** 1, 2, 3, 4, 5, 6, 7, 8, 9 ,0 *****/ 
    { RECT_NUM_KEY_1 , '[', SCANCODE_1,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_2 , ']', SCANCODE_2,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_3 , '{', SCANCODE_3,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_4 , '}', SCANCODE_4,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_5 , '#', SCANCODE_5,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_6 , '%', SCANCODE_6,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_7 , '^', SCANCODE_7,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_8 , '*', SCANCODE_8,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_9 , '+', SCANCODE_9,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_10, '=', SCANCODE_EQUAL,     KEY_PAD_CHAR, NULL, share_key_update},
                     
    /***** -, /, :, ; ), $, &, @ ," *****/ 
    { RECT_NUM_KEY_11, '_', SCANCODE_MINUS,     KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_12, '\\', SCANCODE_BACKSLASH,KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_13, '|', SCANCODE_SEMICOLON, KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_14, '~', SCANCODE_SEMICOLON, KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_15, '<', SCANCODE_LESS,      KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_16, '>', SCANCODE_0,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_17, '$', SCANCODE_4,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_18, '&', SCANCODE_7,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_19, '@', SCANCODE_2,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_20, '"', SCANCODE_APOSTROPHE,KEY_PAD_CHAR, NULL, share_key_update},
                     
    /***** tosb, ., , !, ', backspace *****/ 
    { RECT_NUM_KEY_21, ' ', SCANCODE_TONUM,      KEY_PAD_FUNC, NULL, share_key_update},
    { RECT_NUM_KEY_22, '.', SCANCODE_PERIOD,    KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_23, ',', SCANCODE_COMMA,     KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_24, '?', SCANCODE_SLASH,     KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_25, '!', SCANCODE_1,         KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_26, '\'', SCANCODE_APOSTROPHE,KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_27, ' ', SCANCODE_BACKSPACE, KEY_PAD_FUNC, NULL, share_key_update},
                     
    /***** toen , toppace, enter *****/ 
    { RECT_NUM_KEY_28, ' ', SCANCODE_TOEN,      KEY_PAD_FUNC, NULL, share_key_update},
    { RECT_NUM_KEY_29, ' ', SCANCODE_TOPY,      KEY_PAD_FUNC, NULL, share_key_update},
    { RECT_NUM_KEY_30, ' ', SCANCODE_SPACE,     KEY_PAD_CHAR, NULL, share_key_update},
    { RECT_NUM_KEY_31, ' ', SCANCODE_ENTER,     KEY_PAD_FUNC, NULL, share_key_update},
};

static md_key_t* get_punct_key(key_window_t *kw, POINT point)
{
    int i;
    md_key_t* keys = kw->key;

    for(i = 0; i < kw->key_num; i++) {
        if (PtInRect(&keys[i].bound, point.x, point.y))
            return &keys[i];
    }
    return NULL;
}
/*
static void kw_update (key_window_t *kw, HWND hWnd)
{
    //TODO
    printf("==== update key window ====\n");
}
*/
/* return 0 on succes , less than 0 on error */
static int init_punct_key_window(HWND hWnd, key_window_t *kw)
{
    int i;
    md_key_t *key;
    
    kw->bound.left   = SKB_KW_L;
    kw->bound.top    = SKB_KW_T;
    kw->bound.right  = SKB_KW_R;
    kw->bound.bottom = SKB_KW_B;

    kw->press.x = 0;
    kw->press.y = 0;

    kw->key = punct_key_pads;
    kw->key_num = TABLESIZE(punct_key_pads);

    kw->get_key = get_punct_key;

    if (NULL == (kw->data = (void *)calloc (1, sizeof(kw_add_data_t)))){
        _MY_PRINTF("no memory for key window add data.\n");
        return -1;
    }

    if (get_kbd_bitmap(HDC_SCREEN, &(KEYWIN_DATA_PTR(kw)->char_key_press), CHAR_KEY_MASK)) {
        _MY_PRINTF ("Fail to get key mask bitmap.\n");
        return -1;
    }

    if (get_kbd_bitmap(HDC_SCREEN, &(KEYWIN_DATA_PTR(kw)->func_key_press), FUNC_KEY_MASK)) {
        _MY_PRINTF ("Fail to get key mask bitmap.\n");
        return -1;
    }

    kw->update = NULL;

    /*use the shared bitmap for press key painting.*/
    key = kw->key;
    for (i = 0; i < kw->key_num; i++) {
        if(key->style == KEY_PAD_CHAR)
            key->data = &(KEYWIN_DATA_PTR(kw)->char_key_press);
        else
            key->data = &(KEYWIN_DATA_PTR(kw)->func_key_press);
        key++;
    }
    return 0;
}

static void destroy_punct_key_window(key_window_t *kw)
{
    if (kw->data) {
        release_kbd_bitmap(&(KEYWIN_DATA_PTR(kw)->char_key_press));
        release_kbd_bitmap(&(KEYWIN_DATA_PTR(kw)->func_key_press));
        free(kw->data);
    }
}

int init_punct_keyboard(HWND hWnd, key_board_t *kb)
{
    kb->close_rc.left   = SKB_CLOSE_L;
    kb->close_rc.top    = SKB_CLOSE_T;
    kb->close_rc.right  = SKB_CLOSE_R;
    kb->close_rc.bottom = SKB_CLOSE_B;

    kb->view_window = NULL;
    kb->stroke_window = NULL;

    /*initalize for key window.*/
    kb->key_window = (key_window_t *)calloc(1, sizeof(key_window_t));
    if (kb->key_window == NULL) {
        _MY_PRINTF("no memory for key window.\n");
        return -1;
    }
    if (0 != init_punct_key_window(hWnd, kb->key_window)) {
        _MY_PRINTF("error for initalizing key window.\n");
        return -1;
    }
   
    /*initalize for bk image.*/
    kb->data = (void*)calloc(1, sizeof(BITMAP));
    if (kb->data == NULL){
        _MY_PRINTF("no memory for bk image.\n");
        return -1;
    }

    if (get_kbd_bitmap(HDC_SCREEN, (PBITMAP)(kb->data), PUNCT_KBD_BKG)) {
        _MY_PRINTF ("Fail to get key mask bitmap.\n");
        return -1;
    }

    kb->update = keyboard_update;

    kb->ime = NULL;
    memset(&(kb->action), 0, sizeof(action_t));
    kb->proceed_msg = symbol_proc_msg;

	kb->clear = clear_keyboard;

    return 0;
}

void destroy_punct_keyboard(key_board_t *kb)
{
    /*ky window*/
    destroy_punct_key_window(kb->key_window);

    /*add data*/
    release_kbd_bitmap((PBITMAP)(kb->data));
    free(kb->data);
}

