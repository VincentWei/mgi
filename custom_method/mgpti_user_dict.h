/*
** The header of user dictionary for predictive text input.
**
** Copyright (C) 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
*/

#ifndef __PTI_USER_DICT_H
	#define __PTI_USER_DICT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef MAX_LEN_USER_WORD
#  define MAX_LEN_USER_WORD         12
#endif

#define NR_BUCKETS_USER_DICT_HASH   78
#define NR_USER_WORDS                (NR_BUCKETS_USER_DICT_HASH * 4)

#define MAX_NR_WORDS_IN_A_BUCKET    8

typedef struct _MGPTI_USER_DICTIONARY {
	char user_words [NR_USER_WORDS] [MAX_LEN_USER_WORD + 1];
    int last_empty_slot;
	char* buckets [NR_BUCKETS_USER_DICT_HASH][MAX_NR_WORDS_IN_A_BUCKET];
} MGPTI_USER_DICTIONARY;

/* Load the user dictionary from the secified file */
int _pti_user_dict_load (MGPTI_USER_DICTIONARY* user_dict, FILE* fp);

/* Save the user dictionary to the secified file */
int _pti_user_dict_save (const MGPTI_USER_DICTIONARY* user_dict, FILE* fp);

/* Add a word to the user dictionary */
int _pti_user_dict_add_word (MGPTI_USER_DICTIONARY* user_dict, 
                const char* user_word);

/*
 * Retrive the word in the user dictionary.
 *
 * This function returns the index of the pointer to the word found
 * in the user dictionary on success, NULL on failure.
 *
 * Note that the length of the word should be larger than 1.
 */
char* const* _pti_user_dict_retrive_word (
                const MGPTI_USER_DICTIONARY* user_dict, const char* word);

/*
 * Retrive the first word which has the specified leading letters
 * in the user dictionary.
 *
 * This function returns the pointer to the first word found in 
 * the user dictionary on success, NULL on failure.
 *
 * Note that the length of the leading letters should be larger than 1.
 */
char* const* _pti_user_dict_retrive_first_word (
                const MGPTI_USER_DICTIONARY* user_dict, const char* leading);

/*
 * Retrive the first word which matches with the key stokes in 
 * the user dictionary.
 *
 * This function returns the pointer to the first word, which matches with
 * the key stokes in the user dictionary, on success, NULL on failure.
 *
 * Note the length of the key strokes should be larger than 1.
 */
char* const* _pti_user_dict_match_key_strokes (
                const MGPTI_USER_DICTIONARY* user_dict, const char* keystokes);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __PTI_USER_DICT_H */

