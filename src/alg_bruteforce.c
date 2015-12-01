
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysmodel.h"
#include "utility.h"


FILE* gpFile;
const char gFILENAME[20] = "./Permutation.dat";

void printIDs(int s32VMNum)
{
    int s32i;
    int s32len;
    vmCriPara* pstrT;
    s32len = s32VMNum * sizeof(vmCriPara);
    pstrT = malloc(s32len);

    if(NULL == (gpFile = fopen(gFILENAME, "rb")))
    {
        printf("File Open Error!\n");
        exit(-1);
    }

    while(s32VMNum == fread(pstrT, sizeof(vmCriPara), s32VMNum, gpFile))
    {
        printHostMode(s32VMNum, pstrT);
    }
    fclose(gpFile);
    free(pstrT);
    return;
}

void SwapvmCriParaItem(vmCriPara* pstrA, vmCriPara* pstrB)
{
    vmCriPara strTemp;
    memcpy(&strTemp, pstrA, sizeof(vmCriPara));
    memcpy(pstrA,    pstrB, sizeof(vmCriPara));
    memcpy(pstrB, &strTemp, sizeof(vmCriPara));
    return;
}

/* =========================== Brute Force ============================ */
void generatePermutation(int s32VMNum, vmCriPara* pstrT, int s32Start)
{
    int s32i;
    if(s32Start >= s32VMNum)
    {
        fwrite(pstrT, sizeof(vmCriPara), s32VMNum, gpFile);
    }
    else
    {
        for(s32i = s32Start; s32i < s32VMNum; s32i++)
        {
            SwapvmCriParaItem(pstrT + s32Start, pstrT + s32i);
            generatePermutation(s32VMNum, pstrT, s32Start + 1);
            SwapvmCriParaItem(pstrT + s32Start, pstrT + s32i);
        }
    }
    return;
}

void generatePermutation2File(int s32VMNum, vmCriPara* pstrT)
{
    if(NULL == (gpFile = fopen(gFILENAME, "wb")))
    {
        printf("File Open Error!\n");
        exit(-1);
    }
    generatePermutation(s32VMNum, pstrT, 0);
    fclose(gpFile);
    return;
}

int findCritIndex(int s32VMNum, int s32Crit, vmCriPara* pstrT)
{
    int s32i;
    for(s32i = 0; s32i < s32VMNum; s32i++)
    {
        if(s32Crit == (pstrT + s32i)->s32Crit)
        {
            return s32i;
        }
    }
    return 0;
}

int compareSeqSolution(int s32VMNum, vmCriPara* pstrA, vmCriPara* pstrB)
{
    int s32Crit;
    int s32PosA;
    int s32PosB;
    for(s32Crit = s32VMNum - 1; s32Crit >= 0; s32Crit--)
    {
        s32PosA = findCritIndex(s32VMNum, s32Crit, pstrA);
        s32PosB = findCritIndex(s32VMNum, s32Crit, pstrB);
        if(s32PosA < s32PosB)
        {
            return 1;
        }
        else if(s32PosA > s32PosB)
        {
            return -1;
        }
    }
    return 0;
}


int calModeChangeSeq_algbruteforce(int s32VMNum, double d64Threshold, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT)
{
    int s32i;
    int s32IsMaxInit = 0;
    vmCriPara* pstrMax;

    /* Step 1: Generate Permutation File */
    pstrMax = malloc(s32VMNum * sizeof(vmCriPara));
    (void)genTransMode(s32VMNum, pstrB, pstrA, pstrT);
    /* Important: Set an initial pstrMax */
    //memset((void*)pstrMax, 0, s32VMNum * sizeof(vmCriPara));


    generatePermutation2File(s32VMNum, pstrT);

    /* Step 2: Open File and Check the solution */
    if(NULL == (gpFile = fopen(gFILENAME, "rb")))
    {
        printf("File Open Error!\n");
        exit(-1);
    }

    while(s32VMNum == fread(pstrT, sizeof(vmCriPara), s32VMNum, gpFile))
    {
        if(0 == verifySolution(s32VMNum, d64Threshold, pstrB, pstrA, pstrT))
        {
            if(0 == s32IsMaxInit)
            {
                memcpy(pstrMax, pstrT, s32VMNum * sizeof(vmCriPara));
                s32IsMaxInit = 1;
            }
            else {
                if (1 == compareSeqSolution(s32VMNum, pstrT, pstrMax))
                {
                    memcpy(pstrMax, pstrT, s32VMNum * sizeof(vmCriPara));
                }
            }
        }
    }
    fclose(gpFile);

    memcpy(pstrT, pstrMax, s32VMNum * sizeof(vmCriPara));
    free(pstrMax);

    return 0;
}