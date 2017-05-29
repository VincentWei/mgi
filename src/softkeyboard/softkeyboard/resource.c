/*
** $Id$
**
** resource.c: This file include some functions for system resource loading. 
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 2008/07/15
**
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "softkeyboard.h"

#ifdef SOFTKBD_320_240
#include "320-240/en_kbd_bkg.c"
#include "320-240/num_kbd_bkg.c"
#include "320-240/punct_kbd_bkg.c"
#include "320-240/pinyin_kbd_bkg.c"
#include "320-240/char_key_mask.c"
#include "320-240/func_key_mask.c"
#include "320-240/left_arrow_enable.c"
#include "320-240/left_arrow_disable.c"
#include "320-240/right_arrow_enable.c"
#include "320-240/right_arrow_disable.c"
#ifdef KBD_TOOLTIP
#include "320-240/tooltip_bk.c"
#include "320-240/tooltip_mask.c"
#include "320-240/bitmapfont.c"
#endif
#elif defined (SOFTKBD_480_272)
#include "480-272/en_kbd_bkg.c"
#include "480-272/num_kbd_bkg.c"
#include "480-272/punct_kbd_bkg.c"
#include "480-272/pinyin_kbd_bkg.c"
#include "480-272/char_key_mask.c"
#include "480-272/func_key_mask.c"
#include "480-272/left_arrow_enable.c"
#include "480-272/left_arrow_disable.c"
#include "480-272/right_arrow_enable.c"
#include "480-272/right_arrow_disable.c"
#ifdef KBD_TOOLTIP
#include "480-272/tooltip_bk.c"
#include "480-272/tooltip_mask.c"
#include "480-272/bitmapfont.c"
#endif
#elif defined (SOFTKBD_240_320)
#include "240-320/en_kbd_bkg.c"
#include "240-320/num_kbd_bkg.c"
#include "240-320/punct_kbd_bkg.c"
#include "240-320/pinyin_kbd_bkg.c"
#include "240-320/char_key_mask.c"
#include "240-320/func_key_mask.c"
#include "240-320/left_arrow_enable.c"
#include "240-320/left_arrow_disable.c"
#include "240-320/right_arrow_enable.c"
#include "240-320/right_arrow_disable.c"
#ifdef KBD_TOOLTIP
#include "240-320/tooltip_bk.c"
#include "240-320/tooltip_mask.c"
#include "240-320/bitmapfont.c"
#endif
#endif 


typedef struct _RESDATA {
    const char* file;
    unsigned char* data;
    unsigned int data_size;
} RESDATA;

static RESDATA res[] = {
    /* en keyboard data */
    { "en_kdb_bkg.png", (unsigned char*)en_kbd_data, sizeof(en_kbd_data)},
    /* num keyboard data */
    { "num_kdb_bkg.png", (unsigned char*)num_kbd_data, sizeof(num_kbd_data)},
    /* punct keyboard data */
    { "punct_kdb_bkg.png", (unsigned char*)punct_kbd_data, sizeof(punct_kbd_data)},
    /* pinyin keyboard data */
    { "pinyin_kdb_bkg.png", (unsigned char*)pinyin_kbd_data, sizeof(pinyin_kbd_data)},
    /* char key mask data */
    { "char_key_mask.png", (unsigned char*)char_key_mask_data, sizeof(char_key_mask_data)},
    /* func key mask data */
    { "func_key_mask.png", (unsigned char*)func_key_mask_data, sizeof(func_key_mask_data)},
    /* left arrow enable */
    { "left_arrow_enbale.png", (unsigned char*)left_arrow_enable_data, sizeof(left_arrow_enable_data)},
    /* left arrow disable */
    { "left_arrow_disable.png", (unsigned char*)left_arrow_disable_data, sizeof(left_arrow_disable_data)},
    /* right arrow enable */
    { "right_arrow_enable.png", (unsigned char*)right_arrow_enable_data, sizeof(right_arrow_enable_data)},
    /* right arrow disable */
    { "right_arrow_disable.png", (unsigned char*)right_arrow_disable_data, sizeof(right_arrow_disable_data)},
#ifdef KBD_TOOLTIP
    /* back ground picture for tooltip window */
    { "tooltip_bk.png", (unsigned char*) tooltip_bk_data, sizeof(tooltip_bk_data)},
    /* mask picture for tooltip window */
    { "tooltip_mask.bmp", (unsigned char*) tooltip_mask_data, sizeof(tooltip_mask_data)},
    /* bitmap font picture for tooltip window */
    { "bitmapfont.png", (unsigned char*) bitmap_font_data, sizeof(bitmap_font_data)},
#endif
};

static const char* get_file_suffix(const char* file)
{
    const char* ext;

    if (file == NULL)
        return NULL;

    ext = strrchr(file, '.');
    if (ext)
        return ext + 1;
    return NULL;
}


int get_kbd_bitmap(HDC hdc, PBITMAP pbmp, int id)
{
    RESDATA* p = NULL;
    if (id  < 0 || id > TABLESIZE(res)) 
        return -1;

    p = &res[id];
    if (!p || LoadBitmapFromMem(hdc, pbmp, p->data, p->data_size, get_file_suffix(p->file))) {
        _MY_PRINTF ("Load image error.\n");
        return -1;
    }
    return 0;
}

void release_kbd_bitmap(PBITMAP pbmp)
{
    UnloadBitmap (pbmp);

    return;
}

int get_kbd_mybitmap (PMYBITMAP pmybmp, RGB *pal,  int id)
{
    RESDATA* p = NULL;
   
    if (id  < 0 || id > TABLESIZE(res)) 
        return -1;

    p = &res[id];
 
    if(!p || LoadMyBitmapFromMem (pmybmp, pal, p->data, p->data_size, 
                        get_file_suffix (p->file))) {
        _MY_PRINTF ("Load mybitmap error.\n");
        return -1;
    }

    return 0;
}

void release_kbd_mybitmap (PMYBITMAP pmybmp)
{
    UnloadMyBitmap (pmybmp);

    return;
}
