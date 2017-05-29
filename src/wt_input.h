#ifndef __WTPEN_HEAD_H
#define __WTPEN_HEAD_H

/*
 * Variable type definition 
 */
typedef short	WTError;
typedef long	WT_int32;	/*定义一个四字节的变量*/
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
 *	功能：初始化。该函数在启动识别系统时调用一次即可						
 *	参数：																			
 *		RamAddress:															
 *			由主调函数开辟的空间，至少2K									
 *		RamSize:															
 *			RamAddress所指向的空间的大小									
 *		LibStartAddress:													
 *			库在内存中存放的起始位置										
 *	返回值：																
 *		0--表示成功
 *		其它值--表示出错
 */

WTError WTRecognize(unsigned char * PointBuf,short PointsNumber,unsigned short *CandidateResult);
/*
 *	功能：进行识别。该函数每次对输入的轨迹进行识别，结果放在CandidateResult中
 *	参数：
 *		PointBuf：
 *			输入的轨迹点，依次为(X1,Y1,X2,Y2,...0xff,0x00,Xn,Yn,Xn+1,Yn+1,
 *			...0xff,0x00,0xff,0xff),其中的点对(0xff,0x00)表示一个笔划结束，
 *			点对(0xff,0xff)表示该字结束
 *		PointsNumber:
 *			总共轨迹点的点数
 *		CandidateResult:
 *			存放识别结果,由主调函数开辟，至少应有2*10=20个字节
 *	返回值:
 *		0--表示成功
 *		其它值--表示出错
 */
									
WTError WTRecognizeEnd();
/*
 *	功能：必要的其它工作。该函数在退出识别系统时调用一次即可
 *	参数:none
 *	返回值:
 *		0--表示成功
 *		其它值--表示出错
 */

WTError WTSetRange(short Range);
/*
 *	功能：设置识别范围，函数参数的对应位设置为1时，设别范围将包括对应字符集，
 *		设置为0时，对应字符集将不在识别范围内。另外，设置能否起作用还取决于设
 *		别库是否支持对应字符集。
 *	参数：
 *		Range:
 *			bit0 国标1级(3755)
 * 			bit1 国标2级(6763)
 *			bit2 繁体字
 *			bit5 阿拉伯数字
 *			bit6 小写字母
 *			bit7 大写字母
 *			bit8 标点符号
 *			bit14 实验室规定的功能符号
 * 			bit15 USER Specific
 *			其它  保留
 *	返回值:
 *		0--表示成功
 *		其它值--表示出错
 */

WTError WTSetSpeed(short Speed);
/*
 *	功能：设置识别速度 (Limited function)
 *	参数：
 *		Speed:
 *			0-10, 0最慢,10最快
 *	返回值:
 *		0--表示成功
 *		其它值--表示出错
 */

#ifdef __cplusplus
}
#endif

#endif
