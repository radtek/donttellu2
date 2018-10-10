/**
 * @file   gOgfA3k1901Helper.h
 * @brief  ��װ�����A3000����ɼ��ǣ�����һ�廯�ɼ���̬���׼�ӿ�
 * @author ����������ָ�Ƽ����޹�˾
 * @date   2012/12/28 13:24
 * @note   Edit by guosong 
 * @copyright  ��Ȩ����(@) 1996-2012
 *             ����������ָ�Ƽ����޹�˾
 */


#ifndef _INC_OGFA3K1901HELPER_H_20121215215311_
#define _INC_OGFA3K1901HELPER_H_20121215215311_


typedef	struct	tagGOGFA3K_AppData
{
	CFingerSensor::DEVICEPARAMS DeviceParams;

	GYTHIMAGE_HELPDATA	stData;

	int		nWidth, nHeight;			//!< ��׼Ҫ�������ͼ��Ŀ�Ⱥ͸߶�
	int		nCaptWidth, nCaptHeight;	//!< �ɼ��ǲɼ�����ͼ��Ŀ�Ⱥ͸߶�
	int		nMaxChannel;				//!< ���ͨ���������������1
	int		nChannelIndex;				//!< ��ǰѡ�е�ͨ���ţ�Ĭ��Ϊ0
	int		nBright, nContrast;			//!< ���ȡ��Աȶ�

	UCHAR	*pDataBuffer;				//!< ������Ųɼ�ͼ���Buffer

	int		bInited;		//!< �Ƿ��Ѿ���ʼ��
	int		bOpen;
	int		bActive;
}GOGFA3K_APPDATA;

extern	GOGFA3K_APPDATA	g_stOgfA3KData;
extern	CFingerSensor	g_stFingerSensor;



//!< A3000�ɼ��ǵ�ͼ���С����ʼ�����ȡ��Աȶ���Ϣ��ÿ����λ����ÿӢ��������
typedef	enum	GOGFA3K_DEVICE_INFO
{
	OGFA3K_PRODUCTID = 19,
	//OGFA3K_MAXWIDTH = 640,	OGFA3K_MAXHEIGHT = 640,
	//OGFA3K_WIDTH = 640, OGFA3K_HEIGHT = 640,
	OGFA3K_INITBRIGHT = 200, OGFA3K_INITCONTRAST = 128,
	OGFA3K_XOFFSET = 270, OGFA3K_YOFFSET = 140,
	OGFA3K_BPP = 8, OGFA3K_PPI = 500, 

	OGFA3K_VERSION = 100,

	OGFA3K_UNKNOW
};

#define OGFA3K_DEVICE_DESC		"A3000ָ�Ʋɼ���һ�廯�ɼ���׼�ӿ�"

int		InitialCaptureData();
int		UnInitCaptureData();
int		UTIL_ConvertBright(int nValue);
int		GetSetCaptBright(int nChannel, int* pnBright, UCHAR bGet);
int		UTIL_ConvertContrast(int nValue);
int     GetSetCaptContrast(int nChannel, int* pnContrast, UCHAR bGet);
int		GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
int		SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
int		UTIL_GetOneRawData(UCHAR *pbnCaptured);
DWORD	WINAPI	OgfGA3K_THREAD_StartVideo(LPVOID /*pParam*/);

#endif	// _INC_OGFA3K1901HELPER_H_20121215215311_