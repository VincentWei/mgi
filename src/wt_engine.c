#ifdef _WENTONG_RECOG
#define BYTE    unsigned char
static char *RamAddress = NULL, *LibStartAddress = NULL;
/* wen tong engine */

short (*ptrWentongRecog) (unsigned char * PointBuf, short PointsNumber, 
                          unsigned short *CandidateResult) = NULL;
short (*ptrWentongInit) (char *RamAddress, int RamSize,char *LibStartAddress) = NULL;
short (*ptrWentongSetRange) (short Range) = NULL;

#include "wt_input.h"

static int InitWTRecog ()
{
    FILE *WTPENLibFile = NULL;
    long TotaleFileLength = 0;
    
    if ((WTPENLibFile = fopen ("WTPENPDAL1T.lib", "rb")) == NULL) {
        fprintf (stderr, "Cannot open the wt lib file...!\n");
        return -1;
    }

    fseek (WTPENLibFile, 0, SEEK_END);
    TotaleFileLength = ftell (WTPENLibFile);
    fseek (WTPENLibFile, 0, SEEK_SET);

    if (!(RamAddress = (char *) calloc (2048, sizeof (char)))) {
        fprintf (stderr, "Out of memory !\n");
        return -1;
    }

    LibStartAddress = (char *) calloc (TotaleFileLength, sizeof (char));
    fread (LibStartAddress, sizeof (char), TotaleFileLength, WTPENLibFile);
    fclose (WTPENLibFile);

    if (!ptrWentongInit || ptrWentongInit (RamAddress, 2048, LibStartAddress)) {
        fprintf (stderr, "RecongizeInit error!\n");
        return -1;
    }

    if (ptrWentongSetRange)
        ptrWentongSetRange (0x0001);

    return 0;
}

static int HWWentongRecog (WORD* pTrace, char* pResult,int *nCands, int nPoints, WORD wRange)
{
    static unsigned char buf [NALLOC + 2] = {0};
    int i = 0;
    int end = 0;
    int ret = 0;
    int pos = 0;
    int matched = 0;

    if (nPoints <= 0)
        return 0;
    
    for (; i <= nPoints; i ++) {
       if ((pTrace[i*6] == 0xffff) && (pTrace[i*6+1] == 0xffff)) {
           if ((matched == 1) && (buf[pos] == 0xff && buf[pos+1] == 0xff)) {
               pos += 2;
               continue;
           }
           else {
               buf[pos] = 0xff;
               buf[pos+1] = 0;
           }
           break;
       }
       else {
           if (buf[pos] == pTrace[i * 6 + 2] && 
               buf[pos+1] == pTrace[i * 6 + 3]) {
               matched = 1;
           }
           else {
               matched = 0;
               buf[pos] = pTrace[i * 6 + 2];
               buf[pos+1] = pTrace[i * 6 + 3]; 
           }
       }
       pos += 2;
       if (pos >= NALLOC)
           break;
    }    
    
    buf [pos] = 0xff;
    buf [pos+1] = 0xff;
    
    if (ptrWentongRecog) 
        ret = ptrWentongRecog (buf, *nCands, (unsigned short *)pResult);

    pResult[20] = '\0';
   
    if (ret != 0) {
        *nCands = 0;
        
        return -1;
    }
    else {
        int count = 0;

        i = 0;
        while (1) {
            if (pResult[i] == 0x0 && pResult[i+1] == 0x0)
                break;

            i += 2;
            count ++;
        }

        *nCands = count > NWORD ? NWORD:count;
    }
    printf ("wtResult:%s --- *nCands:%d\n", pResult, *nCands);
    
    return 0; 
}

MGI_HW_ENGINE hw_wentong_engine = {
    InitWTRecog,
    HWWentongRecog
};
#endif
