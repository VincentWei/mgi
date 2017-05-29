#ifdef _JIETONG_RECOG

#include "jt_input.h"

#ifndef BYTE
#define BYTE unsigned char
#endif

BYTE *JTAddress = NULL;

int (* ptrJTHZInitRecog) (BYTE *, int flag) = NULL;
int (* ptrJTCharacterRecognize) (WORD *pTrace, WORD *pResult, int range) = NULL;

static void load_jt_lib (void)
{
    ptrJTHZInitRecog = HZInitCharacterRecognition;
    ptrJTCharacterRecognize = HZCharacterRecognize;
}

static int InitJTHZRecog (void)
{
    long lpos;
    char datafile[NAME_MAX+PATH_MAX+1];
    FILE *fp = NULL;
    int ret = -1;
    
    load_jt_lib();

    sprintf (datafile, "%s/HZRecog.dat", 
            fh_data_path);

    fp = fopen (datafile, "rb");
    if (fp == NULL) {
        printf ("open %s error\n", datafile);
        return -1;
    }

    fseek (fp, 0, SEEK_END);
    lpos = ftell (fp);
    JTAddress = (BYTE *) malloc (lpos);
    if (JTAddress == NULL) {
        printf ("malloc fail for JT!\n");
        return -1;
    }

    fseek (fp, 0, SEEK_SET);
    fread (JTAddress, 1, lpos, fp);
    fclose (fp);

    if (ptrJTHZInitRecog)
        ret = ptrJTHZInitRecog (JTAddress, ADDRESS_POINTER_FLAG);

    printf ("JieTong handwrite initialize. ret:%d\n", ret);

    return 0;
}

staitc int HWJietongRecog (WORD* pTrace, char* pResult,int *nCands, int nPoints, WORD wRange)
{
    int ret = -1;

    if (!ptrJTCharacterRecognize) 
        return -1;

    ret = ptrJTCharacterRecognize (pTrace, (WORD *)pResult, SYMBOL_RECOG_RANGE | SIMPLE_RECOG_RANGE);
    if (ret > 0) {
        *nCands = ret > NWORD ? NWORD:ret;
        pResult[*nCands*2] = '\0';//FIXME
        return 0;
    }

    return -1;
}
MGI_HW_ENGINE hw_jietong_engine = {
    InitJTHZRecog,
    HWJietongRecog
};

#endif /* _JIETONG_RECOG */
