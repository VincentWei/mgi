/*
** The external header for Input method.
**
** Copyright (C) 2007 Feynman Software
**
** All right reserved by Feynman Software.
*/

#ifndef __IME_H
	#define __IME_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define NR_INPUT_METHOD_MAX    2

#define PTI_CASE_ABC                    0x01
#define PTI_CASE_Abc                    0x02
#define PTI_CASE_abc                    0x03

int cb_pti_init(void* filename);
int cb_pti_match_keystrokes(const char *strokes, char *buffer, int buffer_len, int index, int mode);

int cb_pinyin_translate_word(const char *strokes, char *buffer, int buffer_len, int index, int mode);

int cb_pinyin_predict_word(const char *strokes, char *buffer, int buffer_len, int index, int mode);

int cb_pinyin_init(void* filename);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __PTI_H */

