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

//------------------识别范围设置----------------------
//说明:
//25个符号:￥下面可以有一横或两横
//6个手势:回车(0x000D)、空格(0x0020)、回删(0x0008,2个)、删除(0x0010,2个)
//  |--------------------------------------------------------------|
//  | 常用汉字   | 4096 |                                          |
//  | 次常用汉字 | 2688 |                                          |
//  | 异 体 字   | 2778 |  写繁得简+异体字                         |
//  | 数    字   | 27   |  0..9                                    |
//  | 大写字母   | 76   |  A..Z                                    |
//  | 小写字母   | 63   |  a..z                                    |
//  | 标    点   | 18   |  !",:;?、。'()·—…〈〉《》		          |
//  | 符    号   | 25   |  #$%&*+-./<=>@￥[ \ ] ^ _ ` { | } ~      |
//  | 手势符号   | 6    |  Enter+Blank+BackSpace+Delete            |
//  |--------------------------------------------------------------|
#define HW_CHINESE_COMMON      0x0001  //常用汉字
#define HW_CHINESE_RARE        0x0002  //次常用汉字	
#define HW_CHINESE_VARIANT     0x0008  //异体字	
#define HW_NUMERIC             0x0010  //数字	
#define HW_UCALPHA             0x0020  //大写字母	
#define HW_LCALPHA             0x0040  //小写字母	
#define	HW_PUNCTUATION         0x0080  //标点	
#define	HW_SYMBOLS             0x0100  //符号	
#define	HW_GESTURE             0x0200  //手势

#define HW_CAPSLOCK			0x4000	//自动将小写字母转成大写字母
#define HW_SBCCASE				0x8000	//输出全角字符

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
