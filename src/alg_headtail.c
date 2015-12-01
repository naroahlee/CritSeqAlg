
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysmodel.h"
#include "utility.h"


int calMCSeqHead(int s32HeadNum, int s32VMNum, double* pd64Remain, vmCriPara* pstrT)
{
	int s32i = 0;
	int s32MaxCrit;
	int s32MaxIndex;
	int s32Head = 0;
	int *ps32flag;
	double d64Remain;
	double d64Temp;
	d64Remain = *pd64Remain;

	//printf("calMCSeqHead: s32HeadNum = %d, s32VMNum = %d, d64Remain = %2.2lf\n", s32HeadNum, s32VMNum, d64Remain * 100.0);
	//(void)printHostMode(s32VMNum, pstrT);
	ps32flag = malloc(s32VMNum * sizeof(int));
	memset(ps32flag, 0, s32VMNum * sizeof(int));

	while(s32Head < s32HeadNum)
	{
		/* Step1 Find the one with highest criticality */
		s32MaxCrit = -100;
		s32MaxIndex = 0;
		for(s32i = s32Head; s32i < s32VMNum; s32i++)
		{
			if(1 == ps32flag[s32i])
			{
				continue;
			}
			else
			{
				if((pstrT + s32i)->s32Crit > s32MaxCrit)
				{
					s32MaxCrit = (pstrT + s32i)->s32Crit;
					s32MaxIndex = s32i;
				}
			}
		}

		if(s32MaxIndex < s32HeadNum)
		{
			/* d64Remain would not change since we do not change the item */
			ps32flag[s32MaxIndex] = 1;
			moveItemFromRear2Front(s32MaxIndex, s32Head, pstrT);
			moveFlagFromRear2Front(s32MaxIndex, s32Head, ps32flag);
			s32Head++;
		}
		else
		{
			d64Temp = d64Remain - ((pstrT + s32MaxIndex)->d64Util - (pstrT + s32HeadNum - 1)->d64Util);
			if(d64Temp > 0)
			{
				d64Remain = d64Temp;
				ps32flag[s32MaxIndex] = 1;
				moveItemFromRear2Front(s32MaxIndex, s32Head, pstrT);
				moveFlagFromRear2Front(s32MaxIndex, s32Head, ps32flag);
				s32Head++;
			}
			else
			{
				ps32flag[s32MaxIndex] = 1;
			}
		}
	}

	*pd64Remain = d64Remain;
	free(ps32flag);
	return 0;
}
	

int calMCSeqTail(int s32VMNum, double d64Remain, vmCriPara* pstrT)
{
	int s32i;
	int s32HeadNum;
	int s32NegUtilNum;
	int s32MaxIndex;
	int s32MaxCrit;
	double d64UtilSum;
	double d64DestUtil;
	vmCriPara* pstrTemp;

	//printf("calMCSeqTail: s32VMNum = %d, d64Remain = %2.2lf\n", s32VMNum, d64Remain * 100.0);
	//(void)printHostMode(s32VMNum, pstrT);
	/* The exit for this recursive fxn */
	if(0 == s32VMNum)
	{
		return 0;
	}
	/* Step1 Find the Pivot */
	pstrTemp = pstrT;
	s32MaxCrit = -1;
	for(s32i = 0; s32i < s32VMNum; s32i++)
	{
		if(pstrTemp->s32Crit > s32MaxCrit)
		{
			s32MaxIndex = s32i;
			s32MaxCrit = pstrTemp->s32Crit;
		}
		pstrTemp++;
	}
	d64DestUtil = d64Remain - (pstrT + s32MaxIndex)->d64Util;

	/* Find how many neg util items do we have */
	pstrTemp = pstrT;
	s32i = 0;
	while((s32i < s32VMNum) && (pstrTemp->d64Util < 0))
	{
		s32i++;
		pstrTemp++;	
	}
	s32NegUtilNum = s32i;


	/* Step2 Deal with the Head and tail corresponed to pivot */
	if(d64DestUtil > 0)
	{
		(void)moveItemFromRear2Front(s32MaxIndex, 0, pstrT);
		(void)calMCSeqTail(s32VMNum - 1, d64DestUtil, pstrT + 1);
	}
	else
	{
		/* We must release some capacity in order to change the mode of Pivot */
		/* First thing, we must figure out how many item at least we need to change(release) */
		s32HeadNum = 0;
		d64UtilSum = 0.0;
		s32i = 0;
		pstrTemp = pstrT;
	   	while(d64UtilSum > d64DestUtil)
		{
			d64UtilSum += pstrTemp->d64Util; 
			s32i++;
			pstrTemp++;
		}
		s32HeadNum = s32i;
		d64Remain = d64DestUtil - d64UtilSum;
		/* Deal with the Head */
		/* Attention: d64Remain will be changed by invoking this procedure */
		// printf("Before Head: d64Remain = %2.2lf\n", d64Remain * 100.0);
		(void)calMCSeqHead(s32HeadNum, s32NegUtilNum, &d64Remain, pstrT);
		/* Move the Pivot */
		(void)moveItemFromRear2Front(s32MaxIndex, s32HeadNum, pstrT);
		/* Deal with the Tail */
		(void)calMCSeqTail(s32VMNum - s32HeadNum - 1, d64Remain, pstrT + s32HeadNum + 1);
	}

	return 0;
}

int calModeChangeSeq_algheadtail(int s32VMNum, double d64Threshold, vmCriPara* pstrB, vmCriPara* pstrA, vmCriPara* pstrT)
{
	double d64RemainCapacity = 0.0;
	(void)genTransMode(s32VMNum, pstrB, pstrA, pstrT);
	d64RemainCapacity = d64Threshold - calTotalUtil(s32VMNum, pstrB);
	qsort(pstrT, s32VMNum, sizeof(vmCriPara), compare_vmCriPara);
	/* Do the sequencing */
	(void)calMCSeqTail(s32VMNum, d64RemainCapacity, pstrT);
	return 0;
}
