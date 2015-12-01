#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysmodel.h"

#if defined(__i386__)
static __inline__ unsigned long long rdtsc(void)
{
	unsigned long long int x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;
}
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#endif

int compare_doubles (const void *a, const void *b)
{
    const double *da = (const double *) a;
    const double *db = (const double *) b;
    return (*da > *db) - (*da < *db);
}

int compare_vmCriPara (const void *a, const void *b)
{
    const double *da = &((vmCriPara *)a)->d64Util;
    const double *db = &((vmCriPara *)b)->d64Util;
    return (*da > *db) - (*da < *db);
}

int printHostMode(int s32VMNum, vmCriPara* pstrvmList)
{
    int s32i;
    double d64totalUtil = 0.0;
    printf("   RegID   Crit   Util\n");
    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        printf("%8d,%6d,  %2.2lf%%\n",
               pstrvmList->s32RegID,
               pstrvmList->s32Crit,
               pstrvmList->d64Util * 100.0
        );
        d64totalUtil += (pstrvmList->d64Util);
        pstrvmList++;
    }
    printf("Total Util = %2.2lf %% \n", d64totalUtil * 100.0);
    return 0;
}

double calTotalUtil(int s32VMNum, vmCriPara* pstrThis)
{
    int s32i;
    double d64TotalUtil = 0.0;
    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        d64TotalUtil += pstrThis->d64Util;
        pstrThis++;
    }
    return d64TotalUtil;
}

int genVMNum(int s32MaxVMNum)
{
    int s32Ret;
    srand((unsigned int)rdtsc());
    return (rand() % s32MaxVMNum);
}

double genThreshold(double d64MaxThre)
{
    srand((unsigned int)rdtsc());
    return (d64MaxThre * rand()) / RAND_MAX;
}


int genHostMode(int s32VMNum, double d64Threshold, vmCriPara* pstrvmList)
{
    int s32i, s32j;
    int s32temp;
    int* ps32Index;
    double* pd64seg;
    vmCriPara* pstrtemp;
    ps32Index = malloc(s32VMNum * sizeof(int));
    pstrtemp = pstrvmList;

    srand((unsigned int)rdtsc());
    pd64seg = malloc((s32VMNum + 1) * sizeof(double));
    pd64seg[0] = 0.0;
    for(s32i = 1; s32i < s32VMNum + 1; s32i++)
    {
        pd64seg[s32i] = (d64Threshold * rand()) / RAND_MAX;
    }
    qsort(pd64seg, s32VMNum + 1, sizeof(double), compare_doubles);

    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        pstrtemp->s32RegID = s32i;
        pstrtemp->d64Util = pd64seg[s32i + 1] - pd64seg[s32i];
        d64Threshold -= pstrtemp->d64Util;
        pstrtemp++;
    }

    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        ps32Index[s32i] = s32i;
    }
    /* random generate 0~s32VMNum - 1 as list of criticality */
    srand((unsigned int)rdtsc());
    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        s32j = rand() % s32VMNum;
        s32temp = ps32Index[s32i];
        ps32Index[s32i] = ps32Index[s32j];
        ps32Index[s32j] = s32temp;
    }

    pstrtemp = pstrvmList;
    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        pstrtemp->s32Crit = ps32Index[s32i];
        pstrtemp++;
    }

    free(ps32Index);
    free(pd64seg);

    return 0;
}

int genTransMode(int s32VMNum, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT)
{
    int s32i;

    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        pstrT->s32RegID = s32i;
        pstrT->s32Crit = pstrA->s32Crit;
        pstrT->d64Util = pstrA->d64Util - pstrB->d64Util;
        pstrT++;
        pstrB++;
        pstrA++;
    }
    return 0;
}

int verifySolution(int s32VMNum, double d64Threshold, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT)
{
    int s32i;
    int s32RetVal = 0;
    int s32PendingID;
    int s32IsFail = 0;

    double *pd64TotalUtilCurve;
    vmCriPara* pstrTemp;

    pstrTemp = malloc(s32VMNum * sizeof(vmCriPara));
    memcpy((void *)(pstrTemp), (void *)(pstrB), s32VMNum * sizeof(vmCriPara));

    pd64TotalUtilCurve = malloc((s32VMNum + 1) * sizeof(double));
    pd64TotalUtilCurve[0] = calTotalUtil(s32VMNum, pstrTemp);
    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        s32PendingID = (pstrT + s32i)->s32RegID;
        memcpy((void *)(pstrTemp + s32PendingID), (void *)(pstrA + s32PendingID), sizeof(vmCriPara));
        pd64TotalUtilCurve[s32i + 1] = calTotalUtil(s32VMNum, pstrTemp);
        /* Verification */
        if(pd64TotalUtilCurve[s32i + 1] >= d64Threshold)
        {
            s32IsFail = 1;
        }
    }

    if(1 == s32IsFail)
    {
        s32RetVal = -1;
    }
    else
    {
        s32RetVal = 0;
    }

    free(pd64TotalUtilCurve);
    free(pstrTemp);
    return s32RetVal;
}

int moveItemWOAffectFlagged(int s32SrcPos, int s32DstPos, vmCriPara* pstrT, int* ps32Flag)
{
    vmCriPara strTemp;
    int s32Head;
    int s32Tail;
    if(s32DstPos > s32SrcPos)
    {
        printf("Error! moveItemWOAffectFlagged() dst%d > src%d\n", s32DstPos, s32SrcPos);
        exit(-2);
    }
    if(1 == ps32Flag[s32DstPos] || 1 == ps32Flag[s32SrcPos])
    {
        printf("Error! moveItemWOAffectFlagged() dst%dflag=%d, src%dflag=%d\n", s32DstPos, ps32Flag[s32DstPos], s32SrcPos, ps32Flag[s32SrcPos]);
        exit(-2);
    }

    memcpy((void*)&strTemp, (void*)(pstrT + s32SrcPos), sizeof(vmCriPara));
    s32Tail = s32SrcPos;
    s32Head = s32Tail - 1;
    while(s32Head >= s32DstPos)
    {
        while(1 == ps32Flag[s32Head])
        {
            s32Head--;
        }
        memcpy((void*)(pstrT + s32Tail), (void*)(pstrT + s32Head), sizeof(vmCriPara));
        s32Tail = s32Head;
        s32Head--;
    }

    memcpy((void*)(pstrT + s32DstPos), (void*)&strTemp, sizeof(vmCriPara));

    /* After Moving, Flag it immediately */
    ps32Flag[s32DstPos] = 1;
    return 0;

}

int moveItemFromRear2Front(int s32SrcPos, int s32DstPos, vmCriPara* pstrT)
{
    vmCriPara strTemp;
    if(s32DstPos > s32SrcPos)
    {
        printf("I cannnot move from Front to Rear!\n");
        exit(-2);
    }

    memcpy((void*)&strTemp, (void*)(pstrT + s32SrcPos), sizeof(vmCriPara));

    memmove((void*)(pstrT + s32DstPos + 1), (void*)(pstrT + s32DstPos), (s32SrcPos - s32DstPos) * sizeof(vmCriPara));
    memcpy((void*)(pstrT + s32DstPos), (void*)&strTemp, sizeof(vmCriPara));

    return 0;
}

int moveFlagFromRear2Front(int s32SrcPos, int s32DstPos, int* ps32flag)
{
    int s32Temp;
    if(s32DstPos > s32SrcPos)
    {
        printf("I cannnot move from Front to Rear!\n");
        exit(-2);
    }

    memcpy((void*)&s32Temp, (void*)(ps32flag + s32SrcPos), sizeof(int));
    memmove((void*)(ps32flag + s32DstPos + 1), (void*)(ps32flag + s32DstPos), (s32SrcPos - s32DstPos) * sizeof(int));
    memcpy((void*)(ps32flag + s32DstPos), (void*)&s32Temp, sizeof(int));

    return 0;
}