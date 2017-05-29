/**
 * \file hw.h
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: hw.h 100 2006-01-17 02:21:25Z qzlong $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef __HW_H
#define __HW_H

//------------------ʶ��Χ����----------------------
//˵��:
//25������:�����������һ�������
//6������:�س�(0x000D)���ո�(0x0020)����ɾ(0x0008,2��)��ɾ��(0x0010,2��)
//  |--------------------------------------------------------------|
//  | ���ú���   | 4096 |                                          |
//  | �γ��ú��� | 2688 |                                          |
//  | �� �� ��   | 2778 |  д���ü�+������                         |
//  | ��    ��   | 27   |  0..9                                    |
//  | ��д��ĸ   | 76   |  A..Z                                    |
//  | Сд��ĸ   | 63   |  a..z                                    |
//  | ��    ��   | 18   |  !",:;?����'()��������������		          |
//  | ��    ��   | 25   |  #$%&*+-./<=>@��[ \ ] ^ _ ` { | } ~      |
//  | ���Ʒ���   | 6    |  Enter+Blank+BackSpace+Delete            |
//  |--------------------------------------------------------------|
#define HW_CHINESE_COMMON      0x0001  //���ú���
#define HW_CHINESE_RARE        0x0002  //�γ��ú���	
#define HW_CHINESE_VARIANT     0x0008  //������	
#define HW_NUMERIC             0x0010  //����	
#define HW_UCALPHA             0x0020  //��д��ĸ	
#define HW_LCALPHA             0x0040  //Сд��ĸ	
#define	HW_PUNCTUATION         0x0080  //���	
#define	HW_SYMBOLS             0x0100  //����	
#define	HW_GESTURE             0x0200  //����

#define HW_CAPSLOCK			0x4000	//�Զ���Сд��ĸת�ɴ�д��ĸ
#define HW_SBCCASE				0x8000	//���ȫ���ַ�

#define	DATASIZE	12288

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#endif
