/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GfStdLiveApi.h
// ������װ��׼�Ļ���ɼ���ƴ�Ӻ���
// CreateTime : 2006-04-26
//

#ifndef _INC_GFSTDLIVEAPI_H_
#define _INC_GFSTDLIVEAPI_H_
#define RELEASELIVETYPE_FINGER_SCAN 1; //���������İ汾����֧��ָ��ʶ��
#define RELEASELIVETYPE_PALM_SCAN 2;   //���������İ汾����֧������ʶ��
#define RELEASELIVETYPE_FINGER_PALM_SCAN 3; //���������İ汾��֧��ָ�ƺ�����ʶ���
#define RELEASELIVETYPE_FINGER_GUIZHOU 4; //������ݴ��ڵİ汾
extern int reType;

#include "gflstestconst.h"

// ����ɼ��ӿ�
typedef int	(__stdcall *	GFLS_LIVESCAN_INIT)();
typedef int	(__stdcall *	GFLS_LIVESCAN_CLOSE)();
typedef int	(__stdcall *	GFLS_LIVESCAN_GETCHANNELCOUNT)();
typedef int	(__stdcall *	GFLS_LIVESCAN_SETBRIGHT)(int nChannel, int nBright);
typedef int	(__stdcall *	GFLS_LIVESCAN_SETCONTRAST)(int nChannel, int nContrast);
typedef int	(__stdcall *	GFLS_LIVESCAN_GETBRIGHT)(int nChannel, int* pnBright);
typedef int	(__stdcall *	GFLS_LIVESCAN_GETCONTRAST)(int nChannel, int* pnContrast);
typedef int	(__stdcall *	GFLS_LIVESCAN_GETMAXIMAGESIZE)(int nChannel, int* pnWidth, int* pnHeight);
typedef int	(__stdcall *	GFLS_LIVESCAN_GETCAPTWINDOW)(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
typedef int	(__stdcall *	GFLS_LIVESCAN_SETCAPTWINDOW)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
typedef int	(__stdcall *	GFLS_LIVESCAN_SETUP)();
typedef int	(__stdcall *	GFLS_LIVESCAN_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);
typedef int	(__stdcall *	GFLS_LIVESCAN_BEGINCAPTURE)(int nChannel);
typedef int	(__stdcall *	GFLS_LIVESCAN_GETFPRAWDATA)(int nChannel, unsigned char* pRawData);
typedef int	(__stdcall *	GFLS_LIVESCAN_ENDCAPTURE)(int nChannel);
typedef int	(__stdcall *	GFLS_LIVESCAN_ISSUPPORTCAPTWINDOW)(int nChannel);
typedef int	(__stdcall *	GFLS_LIVESCAN_ISSUPPORTSETUP)();
typedef int	(__stdcall *	GFLS_LIVESCAN_GETVERSION)();
typedef int	(__stdcall *	GFLS_LIVESCAN_GETDESC)(char pszDesc[1024]);
typedef int	(__stdcall *	GFLS_LIVESCAN_SETSERIALNUM)(unsigned int nNum);
typedef int	(__stdcall *	GFLS_LIVESCAN_GETSERIALNUM)(unsigned int* pnNum);
typedef int	(__stdcall *	GFLS_LIVESCAN_INITEX)(unsigned int);
// ����ɼ��ӿ�
typedef int	(__stdcall *	GFLS_SCAN_INIT)();
typedef int	(__stdcall *	GFLS_SCAN_CLOSE)();
typedef int	(__stdcall *	GFLS_SCAN_GETCHANNELCOUNT)();
typedef int	(__stdcall *	GFLS_SCAN_SETBRIGHT)(int nChannel, int nBright);
typedef int	(__stdcall *	GFLS_SCAN_SETCONTRAST)(int nChannel, int nContrast);
typedef int	(__stdcall *	GFLS_SCAN_GETBRIGHT)(int nChannel, int* pnBright);
typedef int	(__stdcall *	GFLS_SCAN_GETCONTRAST)(int nChannel, int* pnContrast);
#ifdef UNSTD_BLACK_BALANCE
typedef int(__stdcall *		GFLS_SCAN_SETBLACKBALANCE)(int nChannel, int nBlackBalance);
typedef int(__stdcall *		GFLS_SCAN_GETBLACKBALANCE)(int nChannel, int* pnBlackBalance);
#endif
typedef int	(__stdcall *	GFLS_SCAN_GETMAXIMAGESIZE)(int nChannel, int* pnWidth, int* pnHeight);
typedef int	(__stdcall *	GFLS_SCAN_GETCAPTWINDOW)(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
typedef int	(__stdcall *	GFLS_SCAN_SETCAPTWINDOW)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
typedef int	(__stdcall *	GFLS_SCAN_SETUP)();
typedef int	(__stdcall *	GFLS_SCAN_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);
typedef int	(__stdcall *	GFLS_SCAN_BEGINCAPTURE)(int nChannel);
typedef int	(__stdcall *	GFLS_SCAN_GETFPRAWDATA)(int nChannel, unsigned char* pRawData);
typedef int	(__stdcall *	GFLS_SCAN_ENDCAPTURE)(int nChannel);
typedef int	(__stdcall *	GFLS_SCAN_ISSUPPORTCAPTWINDOW)(int nChannel);
typedef int	(__stdcall *	GFLS_SCAN_ISSUPPORTSETUP)();
typedef int	(__stdcall *	GFLS_SCAN_GETVERSION)();
typedef int	(__stdcall *	GFLS_SCAN_GETDESC)(char pszDesc[1024]);
typedef int	(__stdcall *	GFLS_SCAN_SETSERIALNUM)(unsigned int nNum);
typedef int	(__stdcall *	GFLS_SCAN_GETSERIALNUM)(unsigned int* pnNum);
typedef int	(__stdcall *	GFLS_SCAN_INITEX)(unsigned int);

//typedef int	(__stdcall *	GFLS_LIVESCAN_ISFASTDEVICE)(int nChannel);
typedef struct tagGfls_CaptApiHandler
{
	enum{
		FINGER_SCAN=0,
		PALM_SCAN,
		FOUR_FINGER_SCAN,
		RET_OTHER,
		RET_ERROR,
		RET_OK,
		LOAD_DLL_FAIL};
	tagGfls_CaptApiHandler(){

		pfnScan_Init	= NULL;
		pfnScan_Close	= NULL;
		pfnScan_GetChannelCount	= NULL;
		pfnScan_SetBright		= NULL;
		pfnScan_SetContrast		= NULL;
		pfnScan_GetBright		= NULL;
		pfnScan_GetContrast		= NULL;
#ifdef UNSTD_BLACK_BALANCE
		pfnScan_GetBlackBalance = NULL;
		pfnScan_SetBlackBalance = NULL;
#endif
		pfnScan_GetMaxImageSize	= NULL;
		pfnScan_GetCaptWindow	= NULL;
		pfnScan_SetCaptWindow	= NULL;
		pfnScan_Setup			= NULL;
		pfnScan_GetErrorInfo	= NULL;
		pfnScan_BeginCapture	= NULL;
		pfnScan_GetFPRawData	= NULL;
		pfnScan_EndCapture		= NULL;	
		pfnScan_IsSupportCaptWindow= NULL;
		pfnScan_IsSupportSetup	= NULL;
		//pfnScan_IsFastDevice	= NULL;
		pfnScan_GetVersion		= NULL;
		pfnScan_GetDesc			= NULL;
		pfnScan_SetSerialNum	= NULL;
		pfnScan_GetSerialNum	= NULL;
		pfnScan_InitEx			= NULL;
		
		hDllModule	= NULL;
		bLoaded		= 0;
		ZeroMemory(nReserved,sizeof(nReserved));
		nScanWidth = 0;
		nScanHeight= 0;
	}
	void SetWidth_Height(int nWidth,int nHeight){
		nScanWidth = nWidth;
		nScanHeight= nHeight;
	}
	int GetScanType(char *szFileName){
		HMODULE			tmpModule;
		GFLS_SCAN_INIT	tmpFnScan_Init;
		tmpModule = LoadLibraryA(szFileName);
		if(NULL == tmpModule)
		{
			return RET_ERROR;
		}

		tmpFnScan_Init	= (GFLS_LIVESCAN_INIT)GetProcAddress(tmpModule, "LIVESCAN_Init");
		if( NULL != tmpFnScan_Init ){
			FreeLibrary(tmpModule);
			return FINGER_SCAN;
		}
		tmpFnScan_Init = (GFLS_LIVESCAN_INIT)GetProcAddress(tmpModule, "PALM_Init");
		if( NULL != tmpFnScan_Init ){
			FreeLibrary(tmpModule);
			return PALM_SCAN;
		}
		tmpFnScan_Init = (GFLS_LIVESCAN_INIT)GetProcAddress(tmpModule, "LSFF_Init");
		if( NULL != tmpFnScan_Init ){
			FreeLibrary(tmpModule);
			return FOUR_FINGER_SCAN;
		}
		return RET_OTHER;
	}
	int LoadScan(char *szFileName){
		int		nType = GetScanType(szFileName);
		CString temp1;
		if (1 == reType)
		{
			
			switch(nType){
			case FINGER_SCAN:
				LoadFingerScan(szFileName);
				break;
			case PALM_SCAN:
				temp1 = "�ó���֧�����Ʋɼ�����";
				AfxMessageBox(temp1);
				return RET_ERROR;
			case FOUR_FINGER_SCAN:
				LoadFourFingerScan(szFileName);
				break;
			case RET_OTHER:
				return RET_OTHER;
			case RET_ERROR:
				return RET_OTHER;
			}
		}
		else if (2 == reType)
		{
			switch(nType){
			case FINGER_SCAN:
				temp1 = "�ó���֧��ָ�Ʋɼ�����";
				AfxMessageBox(temp1);
				return RET_ERROR;
			case PALM_SCAN:
				LoadPalmScan(szFileName);
				break;
			case FOUR_FINGER_SCAN:
				LoadFourFingerScan(szFileName);
				break;
			case RET_OTHER:
				return RET_OTHER;
			case RET_ERROR:
				return RET_OTHER;
			}
		}
		else if (3 == reType)
		{
			switch(nType){
			case FINGER_SCAN:
				LoadFingerScan(szFileName);
				break;
			case PALM_SCAN:
				LoadPalmScan(szFileName);
				break;
			case FOUR_FINGER_SCAN:
				LoadFourFingerScan(szFileName);
				break;
			case RET_OTHER:
				return RET_OTHER;
			case RET_ERROR:
				return RET_OTHER;
			}
		}
		else if (4 == reType)
		{
			switch(nType){
			case FINGER_SCAN:
				LoadFingerScan(szFileName);
				break;
			case PALM_SCAN:
				LoadPalmScan(szFileName);
				break;
			case FOUR_FINGER_SCAN:
				LoadFourFingerScan(szFileName);
				break;
			case RET_OTHER:
				return RET_OTHER;
			case RET_ERROR:
				return RET_OTHER;
			}
		}

// 		switch(nType){
// 		case FINGER_SCAN:
// 			if (1 == ReleaseLiveType)
// 			{
// 				LoadFingerScan(szFileName);
// 				break;
// 			} 
// 			else if(2 == ReleaseLiveType)
// 			{
// 				temp1 = "�ó���֧�����Ʋɼ���";
// 				AfxMessageBox(temp1);
// 				return RET_ERROR;
// 			}
// 
// 			break;
// 		case PALM_SCAN:
// 			LoadPalmScan(szFileName);
// 			break;
// 		case FOUR_FINGER_SCAN:
// 			LoadFourFingerScan(szFileName);
// 			break;
// 		case RET_OTHER:
// 			return RET_OTHER;
// 		case RET_ERROR:
// 			return RET_OTHER;
// 		}
		
		return RET_OK;
	}
	int LoadFourFingerScan(char *szFileName){

		hDllModule = LoadLibraryA(szFileName);
		CString temp2;
		if(NULL == hDllModule){
			return LOAD_DLL_FAIL;
		}

		pfnScan_Init = (GFLS_SCAN_INIT)GetProcAddress(hDllModule, "LSFF_Init");

		if( NULL == pfnScan_Init ) {
			temp2 += "����\"4.1��ʼ���ɼ��豸\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.1��ʼ���ɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_InitEx	= (GFLS_SCAN_INITEX)GetProcAddress(hDllModule, "LSFF_InitEx");

		if( NULL == pfnScan_InitEx ) {
			temp2 += "����\"4.1.1��ʼ���ɼ��豸\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.1.1��ʼ���ɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_Close = (GFLS_SCAN_CLOSE)GetProcAddress(hDllModule, "LSFF_Close");

		if( NULL == pfnScan_Close ) 
		{
			temp2 += "����\"4.2�ͷŲɼ��豸\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.2�ͷŲɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_GetChannelCount = (GFLS_SCAN_GETCHANNELCOUNT)GetProcAddress(hDllModule, "LSFF_GetChannelCount");

		if( NULL == pfnScan_GetChannelCount ) {
			temp2 += "����\"4.3��òɼ��豸ͨ������\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.3��òɼ��豸ͨ������\"�ɹ�\r\n";
		}

		pfnScan_SetBright = (GFLS_SCAN_SETBRIGHT)GetProcAddress(hDllModule, "LSFF_SetBright");
		if( NULL == pfnScan_SetBright ) {
			temp2 += "����\"4.4���òɼ��豸��ǰ������\"ʧ��\r\n";
		}else {
			temp2 += "����\"4.4���òɼ��豸��ǰ������\"�ɹ� \r\n";
		}

		pfnScan_SetContrast	= (GFLS_SCAN_SETCONTRAST)GetProcAddress(hDllModule, "LSFF_SetContrast");

		if( pfnScan_SetContrast == NULL ) {
			temp2 += "����\"4.5���òɼ��豸��ǰ�ĶԱȶ�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.5���òɼ��豸��ǰ�ĶԱȶ�\"�ɹ�\r\n";
		}

		pfnScan_GetBright = (GFLS_SCAN_GETBRIGHT)GetProcAddress(hDllModule, "LSFF_GetBright");

		if ( NULL == pfnScan_GetBright ) {
			temp2 += "����\"4.6��òɼ��豸��ǰ������\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.6��òɼ��豸��ǰ������\"�ɹ�  \r\n";
		}


		pfnScan_GetContrast	= (GFLS_SCAN_GETCONTRAST)GetProcAddress(hDllModule, "LSFF_GetContrast");

		if( NULL == pfnScan_GetContrast ) {
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĶԱȶ�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĶԱȶ�\"�ɹ�\r\n";
		}


		pfnScan_GetMaxImageSize = (GFLS_SCAN_GETMAXIMAGESIZE)GetProcAddress(hDllModule, "LSFF_GetMaxImageSize");	
		if ( NULL == pfnScan_GetMaxImageSize ) {
			temp2 += "����\"4.8��òɼ��豸�ɼ�ͼ��Ŀ�ȡ��߶ȵ����ֵ\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.8��òɼ��豸�ɼ�ͼ��Ŀ�ȡ��߶ȵ����ֵ\"�ɹ�\r\n";
		}
		pfnScan_GetCaptWindow = (GFLS_SCAN_GETCAPTWINDOW)GetProcAddress(hDllModule, "LSFF_GetCaptWindow");

		if ( NULL == pfnScan_GetCaptWindow) {
			temp2 += "����\"4.9��õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.9��õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"�ɹ� \r\n";
		}


		pfnScan_SetCaptWindow = (GFLS_SCAN_SETCAPTWINDOW)GetProcAddress(hDllModule, "LSFF_SetCaptWindow");
		if ( NULL == pfnScan_SetCaptWindow){
			temp2 += "����\"4.10���õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.10���õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"�ɹ� \r\n";
		}

		pfnScan_Setup = (GFLS_SCAN_SETUP)GetProcAddress(hDllModule, "LSFF_Setup");
		if( NULL == pfnScan_Setup ){
			temp2 += "����\"4.11���òɼ��豸���������öԻ���\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.11���òɼ��豸���������öԻ���\"�ɹ� \r\n";
		}

		pfnScan_GetErrorInfo = (GFLS_SCAN_GETERRORINFO)GetProcAddress(hDllModule, "LSFF_GetErrorInfo");
		if ( NULL == pfnScan_GetErrorInfo ){
			temp2 += "����\"4.12ȡ�òɼ��ӿڴ�����Ϣ\"ʧ��   \r\n";
		}else{
			temp2 += "����\"4.12ȡ�òɼ��ӿڴ�����Ϣ\"�ɹ�  \r\n";
		}

		pfnScan_BeginCapture	= (GFLS_SCAN_BEGINCAPTURE)GetProcAddress(hDllModule, "LSFF_BeginCapture");
		if ( NULL == pfnScan_BeginCapture) {
			temp2 += "����\"4.13׼���ɼ�һ֡ͼ��\"ʧ��  \r\n";
		}else{
			temp2 += "����\"4.13׼���ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}

		pfnScan_GetFPRawData	= (GFLS_SCAN_GETFPRAWDATA)GetProcAddress(hDllModule, "LSFF_GetFPRawData");
		if(NULL == pfnScan_GetFPRawData) {
			temp2 += "����\"4.14�ɼ�һ֡ͼ��\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.14�ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}


		pfnScan_EndCapture		= (GFLS_SCAN_ENDCAPTURE)GetProcAddress(hDllModule, "LSFF_EndCapture");
		if (NULL == pfnScan_EndCapture) {
			temp2 += "����\"4.15�����ɼ�һ֡ͼ��\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.15�����ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}

		pfnScan_IsSupportCaptWindow = (GFLS_SCAN_ISSUPPORTCAPTWINDOW)GetProcAddress(hDllModule, "LSFF_IsSupportCaptWindow");
		if (NULL == pfnScan_IsSupportCaptWindow) {
			temp2 += "����\"4.16�жϲɼ��豸�Ƿ�֧�ֲɼ���������\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.16�жϲɼ��豸�Ƿ�֧�ֲɼ���������\"�ɹ�\r\n";
		}

		pfnScan_IsSupportSetup	= (GFLS_SCAN_ISSUPPORTSETUP)GetProcAddress(hDllModule, "LSFF_IsSupportSetup");
		if( NULL == pfnScan_IsSupportSetup ){
			temp2 += "����\"4.17�ɼ��豸�Ƿ�֧�����öԻ���\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.17�ɼ��豸�Ƿ�֧�����öԻ���\"�ɹ� \r\n";
		}

		pfnScan_GetVersion = (GFLS_SCAN_GETVERSION)GetProcAddress(hDllModule, "LSFF_GetVersion");
		if( NULL == pfnScan_GetVersion	){
			temp2 += "����\"4.18ȡ�ýӿڵİ汾\"ʧ��   \r\n";
		}else{
			temp2 += "����\"4.18ȡ�ýӿڵİ汾\"�ɹ� \r\n";
		}
		pfnScan_GetDesc	= (GFLS_SCAN_GETDESC)GetProcAddress(hDllModule, "LSFF_GetDesc");
		if ( NULL == pfnScan_GetDesc ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}

		pfnScan_GetSerialNum = (GFLS_SCAN_GETSERIALNUM)GetProcAddress(hDllModule, "LSFF_GetSerialNo");
		if ( NULL == pfnScan_GetSerialNum ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}
		pfnScan_SetSerialNum = (GFLS_SCAN_SETSERIALNUM)GetProcAddress(hDllModule, "LSFF_SetSerialNo");
		if ( NULL == pfnScan_SetSerialNum ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}
		nScanWidth = 800;
		nScanHeight = 600;
		return RET_OK;
	}
	int LoadFingerScan(char *szFileName){
		hDllModule = LoadLibraryA(szFileName);
		CString temp2;
		if(NULL == hDllModule){
			return LOAD_DLL_FAIL;
		}

		pfnScan_Init = (GFLS_SCAN_INIT)GetProcAddress(hDllModule, "LIVESCAN_Init");

		if( NULL == pfnScan_Init ) {
			temp2 += "����\"4.1��ʼ���ɼ��豸\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.1��ʼ���ɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_InitEx	= (GFLS_SCAN_INITEX)GetProcAddress(hDllModule, "LIVESCAN_InitEx");

		if( NULL == pfnScan_InitEx ) {
			temp2 += "����\"4.1.1��ʼ���ɼ��豸\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.1.1��ʼ���ɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_Close = (GFLS_SCAN_CLOSE)GetProcAddress(hDllModule, "LIVESCAN_Close");

		if( NULL == pfnScan_Close ) 
		{
			temp2 += "����\"4.2�ͷŲɼ��豸\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.2�ͷŲɼ��豸\"�ɹ�\r\n";
		}
		
		pfnScan_GetChannelCount = (GFLS_SCAN_GETCHANNELCOUNT)GetProcAddress(hDllModule, "LIVESCAN_GetChannelCount");

		if( NULL == pfnScan_GetChannelCount ) {
			temp2 += "����\"4.3��òɼ��豸ͨ������\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.3��òɼ��豸ͨ������\"�ɹ�\r\n";
		}

		pfnScan_SetBright = (GFLS_SCAN_SETBRIGHT)GetProcAddress(hDllModule, "LIVESCAN_SetBright");
		if( NULL == pfnScan_SetBright ) {
			temp2 += "����\"4.4���òɼ��豸��ǰ������\"ʧ��\r\n";
		}else {
			temp2 += "����\"4.4���òɼ��豸��ǰ������\"�ɹ� \r\n";
		}

		pfnScan_SetContrast	= (GFLS_SCAN_SETCONTRAST)GetProcAddress(hDllModule, "LIVESCAN_SetContrast");

		if( pfnScan_SetContrast == NULL ) {
			temp2 += "����\"4.5���òɼ��豸��ǰ�ĶԱȶ�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.5���òɼ��豸��ǰ�ĶԱȶ�\"�ɹ�\r\n";
		}

		pfnScan_GetBright = (GFLS_SCAN_GETBRIGHT)GetProcAddress(hDllModule, "LIVESCAN_GetBright");

		if ( NULL == pfnScan_GetBright ) {
			temp2 += "����\"4.6��òɼ��豸��ǰ������\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.6��òɼ��豸��ǰ������\"�ɹ�  \r\n";
		}


		pfnScan_GetContrast	= (GFLS_SCAN_GETCONTRAST)GetProcAddress(hDllModule, "LIVESCAN_GetContrast");

		if( NULL == pfnScan_GetContrast ) {
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĶԱȶ�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĶԱȶ�\"�ɹ�\r\n";
		}

#ifdef UNSTD_BLACK_BALANCE
		pfnScan_SetBlackBalance = (GFLS_SCAN_SETBLACKBALANCE)GetProcAddress(hDllModule, "LIVESCAN_SetBlackBalance");

		if (NULL == pfnScan_SetBlackBalance) {
			temp2 += "����\"4.7���òɼ��豸��ǰ�ĺ�ƽ��\"ʧ��  \r\n";
		}
		else {
			temp2 += "����\"4.7���òɼ��豸��ǰ�ĺ�ƽ��\"�ɹ�\r\n";
		}

		pfnScan_GetBlackBalance = (GFLS_SCAN_GETBLACKBALANCE)GetProcAddress(hDllModule, "LIVESCAN_GetBlackBalance");

		if (NULL == pfnScan_GetBlackBalance) {
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĺ�ƽ��\"ʧ��  \r\n";
		}
		else {
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĺ�ƽ��\"�ɹ�\r\n";
		}
		
#endif

		pfnScan_GetMaxImageSize = (GFLS_SCAN_GETMAXIMAGESIZE)GetProcAddress(hDllModule, "LIVESCAN_GetMaxImageSize");	
		if ( NULL == pfnScan_GetMaxImageSize ) {
			temp2 += "����\"4.8��òɼ��豸�ɼ�ͼ��Ŀ�ȡ��߶ȵ����ֵ\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.8��òɼ��豸�ɼ�ͼ��Ŀ�ȡ��߶ȵ����ֵ\"�ɹ�\r\n";
		}
		pfnScan_GetCaptWindow = (GFLS_SCAN_GETCAPTWINDOW)GetProcAddress(hDllModule, "LIVESCAN_GetCaptWindow");

		if ( NULL == pfnScan_GetCaptWindow) {
			temp2 += "����\"4.9��õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.9��õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"�ɹ� \r\n";
		}


		pfnScan_SetCaptWindow = (GFLS_SCAN_SETCAPTWINDOW)GetProcAddress(hDllModule, "LIVESCAN_SetCaptWindow");
		if ( NULL == pfnScan_SetCaptWindow){
			temp2 += "����\"4.10���õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.10���õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"�ɹ� \r\n";
		}

		pfnScan_Setup = (GFLS_SCAN_SETUP)GetProcAddress(hDllModule, "LIVESCAN_Setup");
		if( NULL == pfnScan_Setup ){
			temp2 += "����\"4.11���òɼ��豸���������öԻ���\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.11���òɼ��豸���������öԻ���\"�ɹ� \r\n";
		}

		pfnScan_GetErrorInfo = (GFLS_SCAN_GETERRORINFO)GetProcAddress(hDllModule, "LIVESCAN_GetErrorInfo");
		if ( NULL == pfnScan_GetErrorInfo ){
			temp2 += "����\"4.12ȡ�òɼ��ӿڴ�����Ϣ\"ʧ��   \r\n";
		}else{
			temp2 += "����\"4.12ȡ�òɼ��ӿڴ�����Ϣ\"�ɹ�  \r\n";
		}

		pfnScan_BeginCapture	= (GFLS_SCAN_BEGINCAPTURE)GetProcAddress(hDllModule, "LIVESCAN_BeginCapture");
		if ( NULL == pfnScan_BeginCapture) {
			temp2 += "����\"4.13׼���ɼ�һ֡ͼ��\"ʧ��  \r\n";
		}else{
			temp2 += "����\"4.13׼���ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}

		pfnScan_GetFPRawData	= (GFLS_SCAN_GETFPRAWDATA)GetProcAddress(hDllModule, "LIVESCAN_GetFPRawData");
		if(NULL == pfnScan_GetFPRawData) {
			temp2 += "����\"4.14�ɼ�һ֡ͼ��\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.14�ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}


		pfnScan_EndCapture		= (GFLS_SCAN_ENDCAPTURE)GetProcAddress(hDllModule, "LIVESCAN_EndCapture");
		if (NULL == pfnScan_EndCapture) {
			temp2 += "����\"4.15�����ɼ�һ֡ͼ��\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.15�����ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}

		pfnScan_IsSupportCaptWindow = (GFLS_SCAN_ISSUPPORTCAPTWINDOW)GetProcAddress(hDllModule, "LIVESCAN_IsSupportCaptWindow");
		if (NULL == pfnScan_IsSupportCaptWindow) {
			temp2 += "����\"4.16�жϲɼ��豸�Ƿ�֧�ֲɼ���������\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.16�жϲɼ��豸�Ƿ�֧�ֲɼ���������\"�ɹ�\r\n";
		}

		pfnScan_IsSupportSetup	= (GFLS_SCAN_ISSUPPORTSETUP)GetProcAddress(hDllModule, "LIVESCAN_IsSupportSetup");
		if( NULL == pfnScan_IsSupportSetup ){
			temp2 += "����\"4.17�ɼ��豸�Ƿ�֧�����öԻ���\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.17�ɼ��豸�Ƿ�֧�����öԻ���\"�ɹ� \r\n";
		}

		pfnScan_GetVersion = (GFLS_SCAN_GETVERSION)GetProcAddress(hDllModule, "LIVESCAN_GetVersion");
		if( NULL == pfnScan_GetVersion	){
			temp2 += "����\"4.18ȡ�ýӿڵİ汾\"ʧ��   \r\n";
		}else{
			temp2 += "����\"4.18ȡ�ýӿڵİ汾\"�ɹ� \r\n";
		}
		pfnScan_GetDesc	= (GFLS_SCAN_GETDESC)GetProcAddress(hDllModule, "LIVESCAN_GetDesc");
		if ( NULL == pfnScan_GetDesc ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}

		pfnScan_GetSerialNum = (GFLS_SCAN_GETSERIALNUM)GetProcAddress(hDllModule, "LIVESCAN_GetSerialNo");
		if ( NULL == pfnScan_GetSerialNum ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}
		pfnScan_SetSerialNum = (GFLS_SCAN_SETSERIALNUM)GetProcAddress(hDllModule, "LIVESCAN_SetSerialNo");
		if ( NULL == pfnScan_SetSerialNum ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}
		nScanWidth = GFLS_SCANNER_FINGER_WIDTH;
		nScanHeight = GFLS_SCANNER_FINGER_HEIGHT;
		return RET_OK;
	}
	int LoadPalmScan(char *szFileName){
		hDllModule = LoadLibraryA(szFileName);
		CString temp2;
		if(NULL == hDllModule)
			return LOAD_DLL_FAIL;

		pfnScan_Init = (GFLS_SCAN_INIT)GetProcAddress(hDllModule, "PALM_Init");

		if( NULL == pfnScan_Init ) {
			temp2 += "����\"4.1��ʼ���ɼ��豸\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.1��ʼ���ɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_InitEx	= (GFLS_SCAN_INITEX)GetProcAddress(hDllModule, "PALM_InitEx");

		if( NULL == pfnScan_InitEx ) {
			temp2 += "����\"4.1.1��ʼ���ɼ��豸\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.1.1��ʼ���ɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_Close = (GFLS_SCAN_CLOSE)GetProcAddress(hDllModule, "PALM_Close");

		if( NULL == pfnScan_Close ) {
			temp2 += "����\"4.2�ͷŲɼ��豸\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.2�ͷŲɼ��豸\"�ɹ�\r\n";
		}

		pfnScan_GetChannelCount = (GFLS_SCAN_GETCHANNELCOUNT)GetProcAddress(hDllModule, "PALM_GetChannelCount");

		if( NULL == pfnScan_GetChannelCount ) {
			temp2 += "����\"4.3��òɼ��豸ͨ������\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.3��òɼ��豸ͨ������\"�ɹ�\r\n";
		}

		pfnScan_SetBright = (GFLS_SCAN_SETBRIGHT)GetProcAddress(hDllModule, "PALM_SetBright");
		if( NULL == pfnScan_SetBright ) {
			temp2 += "����\"4.4���òɼ��豸��ǰ������\"ʧ��\r\n";
		}else {
			temp2 += "����\"4.4���òɼ��豸��ǰ������\"�ɹ� \r\n";
		}

		pfnScan_SetContrast	= (GFLS_SCAN_SETCONTRAST)GetProcAddress(hDllModule, "PALM_SetContrast");

		if( pfnScan_SetContrast == NULL ) {
			temp2 += "����\"4.5���òɼ��豸��ǰ�ĶԱȶ�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.5���òɼ��豸��ǰ�ĶԱȶ�\"�ɹ�\r\n";
		}

		pfnScan_GetBright = (GFLS_SCAN_GETBRIGHT)GetProcAddress(hDllModule, "PALM_GetBright");

		if ( NULL == pfnScan_GetBright ) {
			temp2 += "����\"4.6��òɼ��豸��ǰ������\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.6��òɼ��豸��ǰ������\"�ɹ�  \r\n";
		}


		pfnScan_GetContrast	= (GFLS_SCAN_GETCONTRAST)GetProcAddress(hDllModule, "PALM_GetContrast");

		if( NULL == pfnScan_GetContrast ) {
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĶԱȶ�\"ʧ��  \r\n";
		}else{
			temp2 += "����\"4.7��òɼ��豸��ǰ�ĶԱȶ�\"�ɹ�\r\n";
		}


		pfnScan_GetMaxImageSize = (GFLS_SCAN_GETMAXIMAGESIZE)GetProcAddress(hDllModule, "PALM_GetMaxImageSize");	
		if ( NULL == pfnScan_GetMaxImageSize ) {
			temp2 += "����\"4.8��òɼ��豸�ɼ�ͼ��Ŀ�ȡ��߶ȵ����ֵ\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.8��òɼ��豸�ɼ�ͼ��Ŀ�ȡ��߶ȵ����ֵ\"�ɹ�\r\n";
		}
		pfnScan_GetCaptWindow = (GFLS_SCAN_GETCAPTWINDOW)GetProcAddress(hDllModule, "PALM_GetCaptWindow");

		if ( NULL == pfnScan_GetCaptWindow) {
			temp2 += "����\"4.9��õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.9��õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"�ɹ� \r\n";
		}


		pfnScan_SetCaptWindow = (GFLS_SCAN_SETCAPTWINDOW)GetProcAddress(hDllModule, "PALM_SetCaptWindow");
		if ( NULL == pfnScan_SetCaptWindow){
			temp2 += "����\"4.10���õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.10���õ�ǰͼ��Ĳɼ�λ�á���Ⱥ͸߶�\"�ɹ� \r\n";
		}

		pfnScan_Setup = (GFLS_SCAN_SETUP)GetProcAddress(hDllModule, "PALM_Setup");
		if( NULL == pfnScan_Setup ){
			temp2 += "����\"4.11���òɼ��豸���������öԻ���\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.11���òɼ��豸���������öԻ���\"�ɹ� \r\n";
		}

		pfnScan_GetErrorInfo = (GFLS_SCAN_GETERRORINFO)GetProcAddress(hDllModule, "PALM_GetErrorInfo");
		if ( NULL == pfnScan_GetErrorInfo ){
			temp2 += "����\"4.12ȡ�òɼ��ӿڴ�����Ϣ\"ʧ��   \r\n";
		}else{
			temp2 += "����\"4.12ȡ�òɼ��ӿڴ�����Ϣ\"�ɹ�  \r\n";
		}

		pfnScan_BeginCapture	= (GFLS_SCAN_BEGINCAPTURE)GetProcAddress(hDllModule, "PALM_BeginCapture");
		if ( NULL == pfnScan_BeginCapture) {
			temp2 += "����\"4.13׼���ɼ�һ֡ͼ��\"ʧ��  \r\n";
		}else{
			temp2 += "����\"4.13׼���ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}

		pfnScan_GetFPRawData	= (GFLS_SCAN_GETFPRAWDATA)GetProcAddress(hDllModule, "PALM_GetFPRawData");
		if(NULL == pfnScan_GetFPRawData) {
			temp2 += "����\"4.14�ɼ�һ֡ͼ��\"ʧ��  \r\n";
		}else {
			temp2 += "����\"4.14�ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}


		pfnScan_EndCapture		= (GFLS_SCAN_ENDCAPTURE)GetProcAddress(hDllModule, "PALM_EndCapture");
		if (NULL == pfnScan_EndCapture) {
			temp2 += "����\"4.15�����ɼ�һ֡ͼ��\"ʧ�� \r\n";
		}else {
			temp2 += "����\"4.15�����ɼ�һ֡ͼ��\"�ɹ�\r\n";
		}

		pfnScan_IsSupportCaptWindow = (GFLS_SCAN_ISSUPPORTCAPTWINDOW)GetProcAddress(hDllModule, "PALM_IsSupportCaptWindow");
		if (NULL == pfnScan_IsSupportCaptWindow) {
			temp2 += "����\"4.16�жϲɼ��豸�Ƿ�֧�ֲɼ���������\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.16�жϲɼ��豸�Ƿ�֧�ֲɼ���������\"�ɹ�\r\n";
		}

		pfnScan_IsSupportSetup	= (GFLS_SCAN_ISSUPPORTSETUP)GetProcAddress(hDllModule, "PALM_IsSupportSetup");
		if( NULL == pfnScan_IsSupportSetup ){
			temp2 += "����\"4.17�ɼ��豸�Ƿ�֧�����öԻ���\"ʧ�� \r\n";
		}else{
			temp2 += "����\"4.17�ɼ��豸�Ƿ�֧�����öԻ���\"�ɹ� \r\n";
		}

		pfnScan_GetVersion = (GFLS_SCAN_GETVERSION)GetProcAddress(hDllModule, "PALM_GetVersion");
		if( NULL == pfnScan_GetVersion	){
			temp2 += "����\"4.18ȡ�ýӿڵİ汾\"ʧ��   \r\n";
		}else{
			temp2 += "����\"4.18ȡ�ýӿڵİ汾\"�ɹ� \r\n";
		}
		pfnScan_GetDesc	= (GFLS_SCAN_GETDESC)GetProcAddress(hDllModule, "PALM_GetDesc");
		if ( NULL == pfnScan_GetDesc ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}

		pfnScan_GetSerialNum = (GFLS_SCAN_GETSERIALNUM)GetProcAddress(hDllModule, "PALM_GetSerialNo");
		if ( NULL == pfnScan_GetSerialNum ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}
		pfnScan_SetSerialNum = (GFLS_SCAN_SETSERIALNUM)GetProcAddress(hDllModule, "PALM_SetSerialNo");
		if ( NULL == pfnScan_SetSerialNum ) {
			temp2 += "����\"4.20��ýӿڵ�˵��\"ʧ�� \r\n";\
		}else {
			temp2 += "����\"4.20��ýӿڵ�˵��\"�ɹ� \r\n";
		}
		nScanWidth = 2304;
		nScanHeight = 2304;
		return RET_OK;
	}

	GFLS_SCAN_INIT						pfnScan_Init;
	GFLS_SCAN_CLOSE						pfnScan_Close;
	GFLS_SCAN_GETCHANNELCOUNT			pfnScan_GetChannelCount;
	GFLS_SCAN_SETBRIGHT					pfnScan_SetBright;
	GFLS_SCAN_SETCONTRAST				pfnScan_SetContrast;
	GFLS_SCAN_GETBRIGHT					pfnScan_GetBright;
	GFLS_SCAN_GETCONTRAST				pfnScan_GetContrast;
#ifdef UNSTD_BLACK_BALANCE
	GFLS_SCAN_SETBLACKBALANCE			pfnScan_SetBlackBalance;
	GFLS_SCAN_GETBLACKBALANCE			pfnScan_GetBlackBalance;
#endif
	GFLS_SCAN_GETMAXIMAGESIZE			pfnScan_GetMaxImageSize;
	GFLS_SCAN_GETCAPTWINDOW				pfnScan_GetCaptWindow;
	GFLS_SCAN_SETCAPTWINDOW				pfnScan_SetCaptWindow;
	GFLS_SCAN_SETUP						pfnScan_Setup;
	GFLS_SCAN_GETERRORINFO				pfnScan_GetErrorInfo;
	GFLS_SCAN_BEGINCAPTURE				pfnScan_BeginCapture;
	GFLS_SCAN_GETFPRAWDATA				pfnScan_GetFPRawData;
	GFLS_SCAN_ENDCAPTURE				pfnScan_EndCapture;
	GFLS_SCAN_ISSUPPORTCAPTWINDOW		pfnScan_IsSupportCaptWindow;
	GFLS_SCAN_ISSUPPORTSETUP			pfnScan_IsSupportSetup;
	//	GFLS_SCAN_ISFASTDEVICE				pfnScan_IsFastDevice;
	GFLS_SCAN_GETVERSION				pfnScan_GetVersion;
	GFLS_SCAN_GETDESC					pfnScan_GetDesc;
	GFLS_SCAN_SETSERIALNUM				pfnScan_SetSerialNum;
	GFLS_SCAN_GETSERIALNUM				pfnScan_GetSerialNum;

	GFLS_SCAN_INITEX					pfnScan_InitEx;

	int			nScanWidth,nScanHeight;
	HMODULE		hDllModule;
	UCHAR		bLoaded;
	UCHAR		nReserved[7];
}GFLS_CAPT_API_HANDLER;		//֧��ָ�ƺ����������ṹ

typedef struct tagGfls_LiveCaptApiHandler
{
	GFLS_LIVESCAN_INIT						pfnLiveScan_Init;
	GFLS_LIVESCAN_CLOSE						pfnLiveScan_Close;
	GFLS_LIVESCAN_GETCHANNELCOUNT			pfnLiveScan_GetChannelCount;
	GFLS_LIVESCAN_SETBRIGHT					pfnLiveScan_SetBright;
	GFLS_LIVESCAN_SETCONTRAST				pfnLiveScan_SetContrast;
	GFLS_LIVESCAN_GETBRIGHT					pfnLiveScan_GetBright;
	GFLS_LIVESCAN_GETCONTRAST				pfnLiveScan_GetContrast;
	GFLS_LIVESCAN_GETMAXIMAGESIZE			pfnLiveScan_GetMaxImageSize;
	GFLS_LIVESCAN_GETCAPTWINDOW				pfnLiveScan_GetCaptWindow;
	GFLS_LIVESCAN_SETCAPTWINDOW				pfnLivescan_SetCaptWindow;
	GFLS_LIVESCAN_SETUP						pfnLiveScan_Setup;
	GFLS_LIVESCAN_GETERRORINFO				pfnLiveScan_GetErrorInfo;
	GFLS_LIVESCAN_BEGINCAPTURE				pfnLiveScan_BeginCapture;
	GFLS_LIVESCAN_GETFPRAWDATA				pfnLiveScan_GetFPRawData;
	GFLS_LIVESCAN_ENDCAPTURE				pfnLiveScan_EndCapture;
	GFLS_LIVESCAN_ISSUPPORTCAPTWINDOW		pfnLiveScan_IsSupportCaptWindow;
	GFLS_LIVESCAN_ISSUPPORTSETUP			pfnLiveScan_IsSupportSetup;
//	GFLS_LIVESCAN_ISFASTDEVICE				pfnLiveScan_IsFastDevice;
	GFLS_LIVESCAN_GETVERSION				pfnLiveScan_GetVersion;
	GFLS_LIVESCAN_GETDESC					pfnLivescan_GetDesc;
	GFLS_LIVESCAN_SETSERIALNUM				pfnLiveScan_SetSerialNum;
	GFLS_LIVESCAN_GETSERIALNUM				pfnLiveScan_GetSerialNum;

	GFLS_LIVESCAN_INITEX					pfnLiveScan_InitEx;

	
	HMODULE		hDllModule;
	UCHAR		bLoaded;
	UCHAR		nReserved[7];
}GFLS_LIVECAPTAPIHANDLER;


//#if reType == 4
	extern GFLS_LIVECAPTAPIHANDLER	g_stLiveCaptDll;
//#else
	extern GFLS_CAPT_API_HANDLER	g_stCaptDll;
//#endif


int LoadLiveCaptureDll(CWnd* pParentWnd ,bool bIsGafisTest = 0 );
int	DownLiveCaptureDll();
int IsStandardCaptureDll(char* szError, int ncbSize, const char* pszFileName);
int InitialLiveCapture(CWnd* pParentWnd);
int InitialLiveCaptureEx(CWnd* pParentWnd, unsigned int nSerialNo);
void SetBrightContrastValue();
void PutInfo2Log(LPCSTR szFileName,CStringA temp);

// ƴ�ӽӿ�

enum IM_STATUS 
{
	IM_EN_IDLE=0,			//�ȴ�
	IM_EN_BUSY,				//���ڲɼ�
	IM_EN_OVERTIME,			//������ʱ
	IM_EN_END,				//�ɼ�����
	IM_EN_MEMERR,			//�ڴ�����
	IM_EN_TOOQUICK,			//�����ٶ�̫��
	IM_EN_ROLLBACK			//�ع�
};

typedef int	(__stdcall *	GFLS_MOSAIC_INIT)();
typedef int	(__stdcall *	GFLS_MOSAIC_CLOSE)();
typedef int	(__stdcall *	GFLS_MOSAIC_ISSUPPORTIDENTIFYFINGER)();
typedef int	(__stdcall *	GFLS_MOSAIC_ISSUPPORTIMAGEQUALITY)();	
typedef int	(__stdcall *	GFLS_MOSAIC_ISSUPPORTFINGERQUALITY)();
typedef int	(__stdcall *	GFLS_MOSAIC_ISSUPPORTIMAGEENHANCE)();
typedef int	(__stdcall *	GFLS_MOSAIC_ISSUPPORTROLLCAP)();
typedef int	(__stdcall *	GFLS_MOSAIC_SETROLLMODE)(int nRollMode);
typedef int	(__stdcall *	GFLS_MOSAIC_START)(unsigned char* pFingerBuf, int nWidth, int nHeight);
typedef int	(__stdcall *	GFLS_MOSAIC_DOMOSAIC)(unsigned char* pDataBuf, int nWidht, int nHeight);
typedef int	(__stdcall *	GFLS_MOSAIC_STOP)();
typedef int	(__stdcall *	GFLS_MOSAIC_IMAGEQUALITY)(unsigned char* pDataBuf, int nWidth, int nHeight);
typedef int	(__stdcall *	GFLS_MOSAIC_FINGERQUALITY)(unsigned char* pDataBuf, int nWidth, int nHeight);
typedef int	(__stdcall *	GFLS_MOSAIC_IMAGEENHANCE)(unsigned char* pSrcImg, int nWidth, int nHeight, unsigned char* pTargetImg);
typedef int	(__stdcall *	GFLS_MOSAIC_ISFINGER)(unsigned char* pDataBuf, int nWidth, int nHeight);
typedef int	(__stdcall *	GFLS_MOSAIC_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);
typedef int	(__stdcall *	GFLS_MOSAIC_GETVERSION)();
typedef int	(__stdcall *	GFLS_MOSAIC_GETDESC)(char pszDesc[1024]);


typedef struct tagGfls_MosaicApiHandler
{
	GFLS_MOSAIC_INIT					pfnMosaic_Init;
	GFLS_MOSAIC_CLOSE					pfnMosaic_Close;
	GFLS_MOSAIC_ISSUPPORTIDENTIFYFINGER	pfnMosaic_IsSupportIdentifyFinger;
	GFLS_MOSAIC_ISSUPPORTIMAGEQUALITY	pfnMosaic_IsSupportImageQuality;
	GFLS_MOSAIC_ISSUPPORTFINGERQUALITY	pfnMosaic_IsSupportFingerQuality;
	GFLS_MOSAIC_ISSUPPORTIMAGEENHANCE	pfnMosaic_IsSupportImageEnhance;
	GFLS_MOSAIC_ISSUPPORTROLLCAP		pfnMosaic_IsSupportRollCap;
	GFLS_MOSAIC_SETROLLMODE				pfnMosaic_SetRollMode;
	GFLS_MOSAIC_START					pfnMosaic_Start;
	GFLS_MOSAIC_DOMOSAIC				pfnMosaic_DoMosaic;
	GFLS_MOSAIC_STOP					pfnMosaic_Stop;
	GFLS_MOSAIC_IMAGEQUALITY 			pfnMosaic_ImageQuality;
	GFLS_MOSAIC_FINGERQUALITY			pfnMosaic_FingerQuality;
	GFLS_MOSAIC_IMAGEENHANCE 			pfnMosaic_ImageEnhance;
	GFLS_MOSAIC_ISFINGER				pfnMosaic_IsFinger;
	GFLS_MOSAIC_GETERRORINFO			pfnMosaic_GetErrorInfo;
	GFLS_MOSAIC_GETVERSION				pfnMosaic_GetVersion;
	GFLS_MOSAIC_GETDESC					pfnMosaic_GetDesc;

	HMODULE		hDllModule;
	UCHAR		bLoaded;
	int		bMosaicRoll;
	UCHAR		nRollMode;
	UCHAR		nReserved[5];
}GFLS_MOSAICAPIHANDLER;

extern GFLS_MOSAICAPIHANDLER g_stMosaicDll;

int LoadMosaicDll(CWnd* pParentWnd);
int	DownMosaicDll();
int IsStandardMosaicDll(char* szError, int ncbSize, const char* pszFileName);

#endif	// _INC_GFSTDLIVEAPI_H_