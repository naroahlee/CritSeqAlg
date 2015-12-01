
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sysmodel.h"
#include "utility.h"
#include "alg_dinsert.h"
#include "alg_bruteforce.h"

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
#define TEST_TIME (20000)

int IsIdentical(int s32VMNum, vmCriPara* pstrA, vmCriPara* pstrB)
{
	char *Tmp1;
	char *Tmp2;
	int s32Len;
	int s32i;
	s32Len = s32VMNum * sizeof(vmCriPara);
	Tmp1 = (char*)(pstrA);
	Tmp2 = (char*)(pstrB);
	for(s32i = 0; s32i < s32Len; s32i++)
	{
		if(*Tmp1 != *Tmp2)
		{
			return 0;
		}
		Tmp1++;
		Tmp2++;
	}
	return 1;
}


int main(int argc, char* argv[])
{
	int s32i;
	long s64i;
	int s32VMNum = 6;
	double d64Time[1000];
	double d64Mean;
	double d64Var;
	double d64Sum;
	double d64Threshold = 1.0;
	unsigned long long timeB;
	unsigned long long timeA;
	unsigned long long recT, recT_cmp;


	vmCriPara* pstrB;
	vmCriPara* pstrA;
	vmCriPara* pstrT;
	vmCriPara* pstrT_cmp;
	pstrB = malloc(s32VMNum * sizeof(vmCriPara));
	pstrA = malloc(s32VMNum * sizeof(vmCriPara));
	pstrT = malloc(s32VMNum * sizeof(vmCriPara));
	pstrT_cmp = malloc(s32VMNum * sizeof(vmCriPara));

	for(s32i = 1; s32i <= TEST_TIME; s32i++)
	{
		(void) genHostMode(s32VMNum, d64Threshold, pstrB);
		(void) genHostMode(s32VMNum, d64Threshold, pstrA);

		timeB = rdtsc();
		(void)calModeChangeSeq_algdinsert(s32VMNum, d64Threshold, pstrB, pstrA, pstrT);
		(void)calModeChangeSeq_algbruteforce(s32VMNum, d64Threshold, pstrB, pstrA, pstrT_cmp);

		if(0 == IsIdentical(s32VMNum, pstrT, pstrT_cmp))
		{
			printf("\n\n\nFail!\n");
			printf("Before:\n");
			printHostMode(s32VMNum, pstrB);
			printf("After:\n");
			printHostMode(s32VMNum, pstrA);
			printf("Trans:\n");
			printHostMode(s32VMNum, pstrT);
			printf("Trans_cmp:\n");
			printHostMode(s32VMNum, pstrT_cmp);
			break;

		}
		else
		{
			printf("VM# = %3d Pass! Time = %6d\r", s32VMNum, s32i);
			fflush(stdout);
		}

	}
	printf("\n");

	free(pstrB);
	free(pstrA);
	free(pstrT);
	free(pstrT_cmp);

	return 0;
}
