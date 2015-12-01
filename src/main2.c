
#include <stdio.h>
#include <stdlib.h>
#include "sysmodel.h"
#include "utility.h"
#include "alg_bruteforce.h"
#include "alg_dinsert.h"


#define TEST_VM_NUM (3)

int main(int argc, char* argv[])
{
	int s32i;
	int s32VMNum = TEST_VM_NUM;
	int s32IsPass = 0;
	vmCriPara pstrB[TEST_VM_NUM] =
{
		{0, 2,  0.20},
		{1, 0,  0.30},
		{2, 1,  0.45}
};

	vmCriPara pstrA[TEST_VM_NUM] =
{
		{0,  2, 0.45},
		{1,  0, 0.03},
		{2,  1, 0.35}
};

	vmCriPara pstrT[TEST_VM_NUM] = {0};
	vmCriPara pstrT_cmp[TEST_VM_NUM] = {0};
#if 0
			{
					{0,     2,  0.25},
					{2,     1,  -0.10},
					{1,     0,  -0.27}
			};
#endif
	double d64Threshold = 1.0;

	printf("Before:\n");
	(void)printHostMode(s32VMNum, pstrB);
	printf("\n\n\nAfter:\n");
	(void)printHostMode(s32VMNum, pstrA);
	(void)calModeChangeSeq_algdinsert(s32VMNum, d64Threshold, pstrB, pstrA, pstrT);
	(void)calModeChangeSeq_algbruteforce(s32VMNum, d64Threshold, pstrB, pstrA, pstrT_cmp);

	printf("\n\n\nTrans:\n");
	(void)printHostMode(s32VMNum, pstrT);
	printf("\n\n\nTrans_cmp:\n");
	(void)printHostMode(s32VMNum, pstrT_cmp);

	return 0;
}
