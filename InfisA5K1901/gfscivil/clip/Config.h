/*-----------------------------------------------------------------------------
FILE NAME:      psfpApiClass.h
DESCRIPTION:	Configure for the library interface.

AUTHOR:			Xie Yonghui [xieyhfirst@gmail.com]
DATE:			01/15/2009
VERSION:		1.0
REVISION:		
ALL RIGHTS RESERVED.
-----------------------------------------------------------------------------*/

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PSFPDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PSFPDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef PSFPDLL_EXPORTS
#define PSFPDLL_API __declspec(dllexport)
#else
#define PSFPDLL_API __declspec(dllimport)
#endif

