#ifndef __ALG_DINSERT__
#define __ALG_DINSERT__
typedef struct Pivot_t
{
    int s32S;
    int s32E;
}Pivot;

typedef struct PivotQueue_t
{
    int s32Len;
    Pivot* pstrHead;
    Pivot* pstrPos;
} PivotQueue;

extern int printPivotQueue(PivotQueue* pstrPivotQ);
extern int initPivotQueue(PivotQueue* pstrPivotQ, int s32MaxLen);
extern int destroyPivotQueue(PivotQueue* pstrPivotQ);
extern int getPivotHeadPos(PivotQueue* pstrPivotQ);
extern int addPivotMem(PivotQueue* pstrPivotQ, int s32Lst);
extern int enquePivotQueue(PivotQueue* pstrPivotQ, int s32PivotPos);
extern int dequePivotQueue(PivotQueue* pstrPivotQ);
extern double maxTurbulence(PivotQueue* pstrPivotQ, int s32Seg, vmCriPara* pstrT);



extern int calModeChangeSeq_algdinsert(int s32VMNum, double d64Threshold, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT);

#endif
