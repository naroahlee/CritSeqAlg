#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysmodel.h"
#include "utility.h"
#include "alg_dinsert.h"

int printPivotQueue(PivotQueue* pstrPivotQ)
{
    int s32i;
    printf("Len = %d\n", pstrPivotQ->s32Len);
    for(s32i = 0; s32i < pstrPivotQ->s32Len; s32i++)
    {
        printf("[%3d,%3d],", (pstrPivotQ->pstrHead + s32i)->s32S, (pstrPivotQ->pstrHead + s32i)->s32E);
    }
    printf("\n");
    return 0;
}

int initPivotQueue(PivotQueue* pstrPivotQ, int s32MaxLen)
{
    pstrPivotQ->s32Len = 0;
    pstrPivotQ->pstrPos = malloc(sizeof(Pivot) * s32MaxLen);
    if(NULL == pstrPivotQ->pstrPos)
    {
        printf("Malloc Error: initPivotQueue()\n");
        exit(-2);
    }
    pstrPivotQ->pstrHead = pstrPivotQ->pstrPos;
    return 0;
}

int destroyPivotQueue(PivotQueue* pstrPivotQ)
{
    free(pstrPivotQ->pstrPos);
    return 0;
}

int getPivotHeadPos(PivotQueue* pstrPivotQ)
{
    if(0 == pstrPivotQ->s32Len)
    {
        return -1;
    }
    else
    {
        return pstrPivotQ->pstrHead->s32S;
    }
}

int addPivotMem(PivotQueue* pstrPivotQ, int s32Lst)
{
    if(s32Lst >= pstrPivotQ->s32Len)
    {
        printf("Error! addPivotMem() Insert item in %d\n", s32Lst);
        exit(-2);
    }

    ((pstrPivotQ->pstrHead) + s32Lst)->s32E++;
    /* Check Fusion */
    if(s32Lst < pstrPivotQ->s32Len - 1)
    {
        /* Not the Last One */
        if(((pstrPivotQ->pstrHead) + s32Lst)->s32E + 1 == ((pstrPivotQ->pstrHead) + s32Lst + 1)->s32S)
        {
            /* Fusion */
            ((pstrPivotQ->pstrHead) + s32Lst)->s32E = ((pstrPivotQ->pstrHead) + s32Lst + 1)->s32E;
            memmove((void*)(pstrPivotQ->pstrHead + s32Lst + 1), (void*)(pstrPivotQ->pstrHead + s32Lst + 2), ((pstrPivotQ->s32Len - s32Lst - 2) * sizeof(Pivot)));
            pstrPivotQ->s32Len--;
        }
    }
    return s32Lst;
}

int enquePivotQueue(PivotQueue* pstrPivotQ, int s32PivotPos)
{
    int s32End;
    if(pstrPivotQ->s32Len > 0)
    {
        s32End = (pstrPivotQ->pstrHead + (pstrPivotQ->s32Len - 1))->s32E;
        if(s32PivotPos <= s32End)
        {
            printf("Error! enquePivotQueue()\n");
            exit(-5);
        }
        if(s32PivotPos == s32End + 1)
        {
            addPivotMem(pstrPivotQ, pstrPivotQ->s32Len - 1);
        }
        else
        {
            ((pstrPivotQ->pstrHead) + pstrPivotQ->s32Len)->s32S = s32PivotPos;
            ((pstrPivotQ->pstrHead) + pstrPivotQ->s32Len)->s32E = s32PivotPos;
            pstrPivotQ->s32Len++;
        }
        return pstrPivotQ->s32Len;
    }
    else
    {
        ((pstrPivotQ->pstrHead) + pstrPivotQ->s32Len)->s32S = s32PivotPos;
        ((pstrPivotQ->pstrHead) + pstrPivotQ->s32Len)->s32E = s32PivotPos;
        pstrPivotQ->s32Len++;
        return pstrPivotQ->s32Len;
    }

}

int dequePivotQueue(PivotQueue* pstrPivotQ)
{
    int s32RetVal;
    if(0 == pstrPivotQ->s32Len)
    {
        printf("Error! dequePivotQueue()\n");
        exit(-2);
    }
    pstrPivotQ->s32Len--;
    pstrPivotQ->pstrHead++;
    return 0;
}

int findWhichSegment(int s32MaxIndex, PivotQueue* pstrPivotQ)
{
    int s32i;
    int s32Len;
    s32Len = pstrPivotQ->s32Len;
    for(s32i = 0; s32i < s32Len; s32i++)
    {
        if(s32MaxIndex < ((pstrPivotQ->pstrHead) + s32i)->s32S)
        {
            break;
        }
    }
    return s32i;
}

int updatePivotQueue(int s32NewHead, PivotQueue* pstrPivotQ)
{
    while((pstrPivotQ->s32Len !=0) && (pstrPivotQ->pstrHead->s32S < s32NewHead))
    {
        (void)dequePivotQueue(pstrPivotQ);
    }
    return 0;
}

double maxTurbulence(PivotQueue* pstrPivotQ, int s32Seg, vmCriPara* pstrT)
{
    double d64maxTurbulence = -100.0;
    double d64Sum;
    int s32S;
    int s32E;
    int s32i;
    if(s32Seg >= pstrPivotQ->s32Len)
    {
        printf("Error! maxTurbulence() Seg %d does not exist!\n", s32Seg);
        exit(-4);
    }
    s32S = (pstrPivotQ->pstrHead + s32Seg)->s32S;
    s32E = (pstrPivotQ->pstrHead + s32Seg)->s32E;
    d64Sum = 0.0;
    for(s32i = s32S; s32i <= s32E; s32i++)
    {
        d64Sum += (pstrT + s32i)->d64Util;
        if(d64Sum > d64maxTurbulence)
        {
            d64maxTurbulence = d64Sum;
        }
    }
    return d64maxTurbulence;
}


/********************** Alg *********************/

int findMaxCritIndex(int s32Head, int s32VMNum, vmCriPara* pstrT, int* ps32Flag)
{
    int s32MaxCrit;
    int s32MaxIndex;
    int s32i;

    s32MaxCrit = -99;
    for (s32i = s32Head; s32i < s32VMNum; s32i++)
    {
        /* if it was not flagged */
        if(0 == ps32Flag[s32i])
        {
            if((pstrT + s32i)->s32Crit > s32MaxCrit)
            {
                s32MaxCrit = (pstrT + s32i)->s32Crit;
                s32MaxIndex = s32i;
            }
        }
    }
    if(s32MaxIndex < 0 || s32MaxIndex >= s32VMNum)
    {
        printf("Error! findMaxCrit: s32MaxIndex = %d\n", s32MaxIndex);
        exit(-2);
    }
    return s32MaxIndex;
}

int updateHead(int s32VMNum, int* ps32Head, int* ps32Flag, vmCriPara* pstrT)
{
    int s32i;
    for(s32i = *ps32Head; s32i < s32VMNum; s32i++)
    {
        if(0 == ps32Flag[s32i])
        {
            break;
        }
    }
    *ps32Head = s32i;
    return 0;
}

int updateRemain(int s32VMNum, int s32Start, double d64StartRem, double* pd64RemList, vmCriPara* pstrT)
{
    int s32i;
    pd64RemList[s32Start] = d64StartRem;
    for(s32i = s32Start + 1; s32i < s32VMNum + 1; s32i++)
    {
        pd64RemList[s32i] = pd64RemList[s32i - 1] - (pstrT + s32i - 1)->d64Util;
    }
    return 0;
}

int printRemainList(int s32VMNum, double* pd64RemList)
{
    int s32i;
    for(s32i = 0; s32i < s32VMNum + 1; s32i++)
    {
        printf("%2.2lf, ", pd64RemList[s32i] * 100.0);
    }
    printf("\n");
    return 0;
}

int IsSuitInSeg(int s32Seg, int s32MaxIndex, PivotQueue* pstrPivotQ, double* pd64RemList, vmCriPara* pstrT)
{
    int s32ItemTobeReplaced;
    double d64Turb;
    if(s32Seg >= pstrPivotQ->s32Len)
    {
        printf("Error! IsSuitInSeg()\n");
        exit(-6);
    }
    s32ItemTobeReplaced = (pstrPivotQ->pstrHead + s32Seg)->s32S - 1;
    d64Turb = maxTurbulence(pstrPivotQ, s32Seg, pstrT);
    if((pd64RemList[s32ItemTobeReplaced] - (pstrT + s32MaxIndex)->d64Util) > d64Turb)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int calSeq_dinsert(int s32VMNum, double d64Remain, vmCriPara* pstrT)
{
    int s32i;
    int s32Head;
    int *ps32Flag;
    int s32MaxIndex;
    int s32Segment;
    int s32TestSeg;
    int s32PivotStart;
    int s32PivotEnd;
    int s32InsertPos;
    double* pd64RemList;
    PivotQueue strPivotQ;

    (void)initPivotQueue(&strPivotQ, s32VMNum);
    if (NULL == (ps32Flag = malloc(s32VMNum * sizeof(int))))
    {
        printf("Malloc Error! calSeq_dinsert()\n");
        exit(-2);
    }
    memset(ps32Flag, 0, s32VMNum * sizeof(int));

    if (NULL == (pd64RemList = malloc((s32VMNum + 1)* sizeof(double))))
    {
        printf("Malloc Error! calSeq_dinsert()\n");
        exit(-2);
    }

    /* Init the pd64RemList */
    (void)updateRemain(s32VMNum, 0, d64Remain, pd64RemList, pstrT);

    /* Main Alg */
    s32Head = 0;
    while (s32Head < s32VMNum)
    {
        /* Find the potential pivot and flag it */
        s32MaxIndex = findMaxCritIndex(s32Head, s32VMNum, pstrT, ps32Flag);
        s32Segment = findWhichSegment(s32MaxIndex, &strPivotQ);
        if(s32Segment == strPivotQ.s32Len)
        {
            if(s32Segment > 0)
            {
                /* We have several segment, let's do it from the last */
                // We are at the last segment
                s32PivotEnd = (strPivotQ.pstrHead + s32Segment - 1)->s32E;


                if(pd64RemList[s32PivotEnd + 1] < (pstrT + s32MaxIndex)->d64Util)
                {
                    /* We must create a new pivot */
                    for(s32i = s32PivotEnd + 1; s32i < s32VMNum; s32i++)
                    {
                        if(pd64RemList[s32i] > (pstrT + s32MaxIndex)->d64Util)
                        {
                            (void)moveItemWOAffectFlagged(s32MaxIndex, s32i, pstrT, ps32Flag);
                            (void)enquePivotQueue(&strPivotQ, s32i);
                            (void)updateRemain(s32VMNum, s32i, pd64RemList[s32i], pd64RemList, pstrT);
                            break;
                        }
                    }
                }
                else
                {
                    /* We can somewhat merge into some pivot */
                    s32TestSeg = s32Segment;
                    while(s32TestSeg > 0 && 1 == IsSuitInSeg(s32TestSeg - 1, s32MaxIndex, &strPivotQ, pd64RemList, pstrT))
                    {
                        s32TestSeg--;
                    }
                    if(0 == s32TestSeg)
                    {
                        (void)moveItemWOAffectFlagged(s32MaxIndex, s32Head, pstrT, ps32Flag);
                        (void)updateRemain(s32VMNum, s32Head, pd64RemList[s32Head], pd64RemList, pstrT);
                    }
                    else
                    {
                        s32InsertPos = (strPivotQ.pstrHead + s32TestSeg - 1)->s32E + 1;
                        (void)moveItemWOAffectFlagged(s32MaxIndex, s32InsertPos, pstrT, ps32Flag);
                        (void)addPivotMem(&strPivotQ, s32TestSeg - 1);
                        (void)updateRemain(s32VMNum, s32Head, pd64RemList[s32Head], pd64RemList, pstrT);
                    }
                }


            }
            else
            {
                /* Seg == 0 */
                /* We only have one segment, let's do it from head */
                for(s32i = s32Head; s32i < s32VMNum; s32i++)
                {
                    if(pd64RemList[s32i] > (pstrT + s32MaxIndex)->d64Util)
                    {
                        (void)moveItemWOAffectFlagged(s32MaxIndex, s32i, pstrT, ps32Flag);
                        (void)enquePivotQueue(&strPivotQ, s32i);
                        (void)updateRemain(s32VMNum, s32i, pd64RemList[s32i], pd64RemList, pstrT);
                        break;
                    }
                }
            }

        }
        else
        {
            /* We are not in the last Segment */
            /* We can somewhat merge into some pivot */
            s32TestSeg = s32Segment;
            while(s32TestSeg > 0 && 1 == IsSuitInSeg(s32TestSeg - 1, s32MaxIndex, &strPivotQ, pd64RemList, pstrT))
            {
                s32TestSeg--;
            }
            if(0 == s32TestSeg)
            {
                (void)moveItemWOAffectFlagged(s32MaxIndex, s32Head, pstrT, ps32Flag);
                (void)updateRemain(s32VMNum, s32Head, pd64RemList[s32Head], pd64RemList, pstrT);
            }
            else
            {
                s32InsertPos = (strPivotQ.pstrHead + s32TestSeg - 1)->s32E + 1;
                (void)moveItemWOAffectFlagged(s32MaxIndex, s32InsertPos, pstrT, ps32Flag);
                (void)addPivotMem(&strPivotQ, s32TestSeg - 1);
                (void)updateRemain(s32VMNum, s32Head, pd64RemList[s32Head], pd64RemList, pstrT);
            }
        }

        (void)updateHead(s32VMNum, &s32Head, ps32Flag, pstrT);
        (void)updatePivotQueue(s32Head, &strPivotQ);
    }


    (void)destroyPivotQueue(&strPivotQ);
    free(ps32Flag);
    free(pd64RemList);
    return 0;
}

int calModeChangeSeq_algdinsert(int s32VMNum, double d64Threshold, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT)
{
    double d64RemainCapacity = 0.0;
    (void)genTransMode(s32VMNum, pstrB, pstrA, pstrT);
    d64RemainCapacity = d64Threshold - calTotalUtil(s32VMNum, pstrB);
    qsort(pstrT, s32VMNum, sizeof(vmCriPara), compare_vmCriPara);
    /* Do the sequencing */
    (void)calSeq_dinsert(s32VMNum, d64RemainCapacity, pstrT);
    return 0;
}