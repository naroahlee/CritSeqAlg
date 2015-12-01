
#ifndef __ALG_UTILITY__
#define __ALG_UTILITY__

extern int compare_doubles (const void *a, const void *b);
extern int compare_vmCriPara (const void *a, const void *b);
extern int printHostMode(int s32VMNum, vmCriPara* pstrvmList);
extern double calTotalUtil(int s32VMNum, vmCriPara* pstrThis);

extern int genVMNum(int s32MaxVMNum);
extern double genThreshold(double d64MaxThre);
extern int genHostMode(int s32VMNum, double d64Threshold, vmCriPara* pstrvmList);
extern int genTransMode(int s32VMNum, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT);
extern int verifySolution(int s32VMNum, double d64Threshold, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT);
extern int moveItemWOAffectFlagged(int s32SrcPos, int s32DstPos, vmCriPara* pstrT, int* ps32Flag);
extern int moveItemFromRear2Front(int s32SrcPos, int s32DstPos, vmCriPara* pstrT);
extern int moveFlagFromRear2Front(int s32SrcPos, int s32DstPos, int* ps32flag);


#endif