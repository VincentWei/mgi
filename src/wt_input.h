#ifndef __WTPEN_HEAD_H
#define __WTPEN_HEAD_H

/*
 * Variable type definition 
 */
typedef short	WTError;
typedef long	WT_int32;	/*����һ�����ֽڵı���*/
/*
 * Define Macros
 */
#define WTPEN_abs(value)	  (((value)<0)?(-(value)):(value))
/*
 * Declaration of functioins				
 */
#ifdef __cplusplus
extern "C" {
#endif

WTError WTRecognizeInit(char *RamAddress,WT_int32 RamSize,char *LibStartAddress);	
/*
 *	���ܣ���ʼ�����ú���������ʶ��ϵͳʱ����һ�μ���						
 *	������																			
 *		RamAddress:															
 *			�������������ٵĿռ䣬����2K									
 *		RamSize:															
 *			RamAddress��ָ��Ŀռ�Ĵ�С									
 *		LibStartAddress:													
 *			�����ڴ��д�ŵ���ʼλ��										
 *	����ֵ��																
 *		0--��ʾ�ɹ�
 *		����ֵ--��ʾ����
 */

WTError WTRecognize(unsigned char * PointBuf,short PointsNumber,unsigned short *CandidateResult);
/*
 *	���ܣ�����ʶ�𡣸ú���ÿ�ζ�����Ĺ켣����ʶ�𣬽������CandidateResult��
 *	������
 *		PointBuf��
 *			����Ĺ켣�㣬����Ϊ(X1,Y1,X2,Y2,...0xff,0x00,Xn,Yn,Xn+1,Yn+1,
 *			...0xff,0x00,0xff,0xff),���еĵ��(0xff,0x00)��ʾһ���ʻ�������
 *			���(0xff,0xff)��ʾ���ֽ���
 *		PointsNumber:
 *			�ܹ��켣��ĵ���
 *		CandidateResult:
 *			���ʶ����,�������������٣�����Ӧ��2*10=20���ֽ�
 *	����ֵ:
 *		0--��ʾ�ɹ�
 *		����ֵ--��ʾ����
 */
									
WTError WTRecognizeEnd();
/*
 *	���ܣ���Ҫ�������������ú������˳�ʶ��ϵͳʱ����һ�μ���
 *	����:none
 *	����ֵ:
 *		0--��ʾ�ɹ�
 *		����ֵ--��ʾ����
 */

WTError WTSetRange(short Range);
/*
 *	���ܣ�����ʶ��Χ�����������Ķ�Ӧλ����Ϊ1ʱ�����Χ��������Ӧ�ַ�����
 *		����Ϊ0ʱ����Ӧ�ַ���������ʶ��Χ�ڡ����⣬�����ܷ������û�ȡ������
 *		����Ƿ�֧�ֶ�Ӧ�ַ�����
 *	������
 *		Range:
 *			bit0 ����1��(3755)
 * 			bit1 ����2��(6763)
 *			bit2 ������
 *			bit5 ����������
 *			bit6 Сд��ĸ
 *			bit7 ��д��ĸ
 *			bit8 ������
 *			bit14 ʵ���ҹ涨�Ĺ��ܷ���
 * 			bit15 USER Specific
 *			����  ����
 *	����ֵ:
 *		0--��ʾ�ɹ�
 *		����ֵ--��ʾ����
 */

WTError WTSetSpeed(short Speed);
/*
 *	���ܣ�����ʶ���ٶ� (Limited function)
 *	������
 *		Speed:
 *			0-10, 0����,10���
 *	����ֵ:
 *		0--��ʾ�ɹ�
 *		����ֵ--��ʾ����
 */

#ifdef __cplusplus
}
#endif

#endif
