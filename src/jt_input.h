#ifdef __cplusplus
extern "C"    {
#endif

#define  MAX_CANDIDATE_NUM    10

//#define DllImport	__declspec( dllimport )
//#define DllExport	__declspec( dllexport )
#define DllImport
#define DllExport	

#define ADDRESS_POINTER_FLAG  0x01
#define FILENAME_POINTER_FLAG 0x02
#ifndef UINT
  #define UINT unsigned int
#endif
#ifndef BOOL
  #define BOOL int
#endif
#ifndef WORD
  #define WORD unsigned short
#endif
#ifndef TRUE
  #define TRUE  1
#endif
#ifndef FALSE
  #define FALSE 0
#endif
#ifndef NULL
  #define NULL  0
#endif
// Recognition Kernel Initialization
// call this function before using other functions in handwritting recognition kernel
// the pointer is ROM address for placing HZRecog.dat or is file name string based on uFlag parameter
// Return value :	TRUE	success
//					FALSE	fail
DllExport BOOL HZInitCharacterRecognition(void* pPointer, UINT uFlag);

// Recognition Kernel Exit
// call this function before using other functions handwritting recognition kernel no more
// Return value :	TRUE	success
//					FALSE	fail
DllExport BOOL HZExitCharacterRecognition(void);

#define NUMBER_RECOG_RANGE      0x0001
#define UPPERCASE_RECOG_RANGE   0x0002
#define LOWERCASE_RECOG_RANGE   0x0004
#define INTERPUNCTION_RECOG_RANGE 0x0008
#define SIMPLE1_RECOG_RANGE     0x0020
#define SIMPLE2_RECOG_RANGE     0x0040
#define ADAPTATION_RECOG_RANGE  0x1000

#define SYMBOL_RECOG_RANGE      (UPPERCASE_RECOG_RANGE | LOWERCASE_RECOG_RANGE)
#define SIMPLE_RECOG_RANGE      (SIMPLE1_RECOG_RANGE | SIMPLE2_RECOG_RANGE)
// Main Recognition Function
// Return value is recognition result number, its valid value are [-1, MAX_CANDIDATE_NUM]
// pnStrokeBuffer is handwritting buffer of Chinese character.
// (x, y) make up one point, and x and y are short type, their valid value are from -1 to 32767.
// (-1, 0) is end flag of stroke, (-1, -1) is end flag of character.
// pwResultBuffer is recognition result buffer, we recommend its size are MAX_CANDIDATE_NUM+1
// uMatchRange is one of four kinds of match_range or combination of them. Note: it is not null(zero)
// moreover, uMatchRange may include ADAPTATION_MATCH_RANGE only after calling 'HZInitAdaptation'
DllExport int HZCharacterRecognize(short* pnStrokeBuffer, WORD* pwResultBuffer, UINT uMatchRange);

// Adaptation function initialization
// call this function before using other functions in adaptation function
// Return value :	TRUE	success
//					FALSE	fail
// szFileName1 and szFileName2 are two adaptation function file name
// the format of them should be all path name
DllExport BOOL HZInitAdaptation(const char* szFileName1, const char* szFileName2, const char* szFileName3);

// Adaptation function exit
// call this function before using other functions in adaptatio function no more
// Return value :	TRUE	success
//					FALSE	fail
DllExport BOOL HZExitAdaptation();

// Character Adaptation function
// Return value :	TRUE	success
//					FALSE	fail
// Param same 'HZCharacterRecognize'
DllExport BOOL HZCharacterAdaptation(WORD wCode, short* pnStrokeBuffer);

// Return value	:	Adaptation Character Numbers
DllExport int HZGetAdaptationCharacterNumber();

// Return value	:	Character handwriting length in short
DllExport int HZGetAdaptationCharacterBufferLength(int iIndex);

// Return value	:	Character code
// iIndex is index value in adaptation dictionary
// pnStrokeBuffer is handwritting buffer of character.
DllExport WORD HZGetAdaptationCharacter(int iIndex, short* pnStrokeBuffer);

//
DllExport void HZDeleteAdaptationCharacter(int iIndex);

#ifdef __cplusplus
}
#endif 



