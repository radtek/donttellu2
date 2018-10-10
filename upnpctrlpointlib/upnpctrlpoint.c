#include "upnp.h"
#include "upnpdebug.h"
#include "sample_util.h"
#include "tv_ctrlpt.h"
#include "upnpctrlpoint.h"

extern struct TvDeviceNode *GlobalDeviceList;

extern ithread_mutex_t actionresult_mutex;

static int	g_bStoped = 1;

extern ActionResult g_ActionResultArray[];

extern int	g_nActionErrCode;

static int ClearResult(EnumAction enAction)
{
	if(g_bStoped || enAction < 0 || enAction >= ENUM_MAX)
		return -1;

	ithread_mutex_lock(&actionresult_mutex);

	ActionResult	*pResult = NULL;
	pResult = &g_ActionResultArray[enAction];

	pResult->nErrCode = 0;
	SAFE_FREE(pResult->pszActionRequest);
	SAFE_FREE(pResult->pszActionResult);
	pResult->bValid = 0;

	g_nActionErrCode = 0;
	ithread_mutex_unlock(&actionresult_mutex);

	return 0;
}

int CTRLPOINT_Start()
{
	if(!g_bStoped)
		return -1;

	int nRet;

	nRet = TvCtrlPointStart(linux_print, NULL, 0);
	if (nRet != TV_SUCCESS) {
		SampleUtil_Print("Error starting UPnP TV Control Point\n");
		return nRet;
	}

	g_bStoped = 0;
	return nRet;
}


int CTRLPOINT_Stop()
{
	if(g_bStoped)
		return -1;

	int nRet = TvCtrlPointStop();

	g_bStoped = 1;

	return nRet;
}


/*	action function, async	*/
int CTRLPOINT_ACTION_AddPortMapping(int nDevNum, const PortMapping *pObjPM)
{
	if(nDevNum <= 0 || pObjPM == NULL)
		return -1;

	if(g_bStoped)
		return -1;

	ClearResult(ADDPORTMAPPING);

	char		szBuf[8][100];
	char		*argval[8] = {0};

	memset(szBuf, 0, sizeof(szBuf));
	int i;
	for(i = 0; i < 8; ++i)
	{
		argval[i] = szBuf[i];
	}

	if(pObjPM->pszRemoteHost)
		sprintf(szBuf[0], "%s", pObjPM->pszRemoteHost);

    sprintf(szBuf[1], "%d", pObjPM->nExternPort);
    sprintf(szBuf[2], "%s", (pObjPM->bUDP ? "UDP" : "TCP"));
	sprintf(szBuf[3], "%d", pObjPM->nInterPort);

	if(pObjPM->pszClient == NULL)
		return -1;

	sprintf(szBuf[4], "%s", pObjPM->pszClient);
	sprintf(szBuf[5], "%d", pObjPM->bEnable ? 1 : 0);

	if(pObjPM->pszDesc)
		sprintf(szBuf[6], "%s", pObjPM->pszDesc);

	sprintf(szBuf[7], "%d", pObjPM->nLeaseDuration);

	TvCtrlPointSendAddPortMapping(nDevNum, (char **)argval, 8);
	//"AddPortMapping	<devnum><NewRemoteHost(string)>"
	//"<NewExternalPort(int2)><NewProtocol(string)><NewInternalPort(int2)><NewInternalClient(string)>"
	//"<ewEnabled(bool?)><NewPortMappingDescription(string)><NewLeaseDuration(int)>\n";

	return 0;
}


int CTRLPOINT_ACTION_DelPortMapping(int nDevNum, const PortMapping *pObjPM)
{
	if(nDevNum <= 0 || pObjPM == NULL)
		return -1;

	if(g_bStoped)
		return -1;

	ClearResult(DELPORTMAPPING);

	char		szBuf[3][100];
	char		*argval[3] = {0};

	memset(szBuf, 0, sizeof(szBuf));
	int i;
	for(i = 0; i < 3; ++i)
	{
		argval[i] = szBuf[i];
	}

	if(pObjPM->pszRemoteHost)
		sprintf(szBuf[0], "%s", pObjPM->pszRemoteHost);

    sprintf(szBuf[1], "%d", pObjPM->nExternPort);
    sprintf(szBuf[2], "%s", (pObjPM->bUDP ? "UDP" : "TCP"));

	TvCtrlPointSendDelPortMapping(nDevNum, (char **)argval, 3);
	//"DeletePortMapping	<devnum><NewRemoteHost(string)>"
	//"<NewExternalPort(int2)><NewProtocol(string)>\n";

	return 0;
}


int CTRLPOINT_ACTION_QueryPortMapping(int nDevNum, const PortMapping *pObjPM)
{
	if(nDevNum <= 0 || pObjPM == NULL)
		return -1;

	if(g_bStoped)
		return -1;

	ClearResult(QUERYPORTMAPPING);

	char		szBuf[3][100];
	char		*argval[3] = {0};

	memset(szBuf, 0, sizeof(szBuf));
	int i;
	for(i = 0; i < 3; ++i)
	{
		argval[i] = szBuf[i];
	}

	if(pObjPM->pszRemoteHost)
		sprintf(szBuf[0], "%s", pObjPM->pszRemoteHost);

    sprintf(szBuf[1], "%d", pObjPM->nExternPort);
    sprintf(szBuf[2], "%s", (pObjPM->bUDP ? "UDP" : "TCP"));

	TvCtrlPointSendQueryPortMapping(nDevNum, (char **)argval, 3);
	//"QueryPortMapping	<devnum><NewRemoteHost(string)>"
	//"<NewExternalPort(int2)><NewProtocol(string)>\n";

	return 0;
}


int CTRLPOINT_ACTION_GetExternalIP(int nDevNum)
{
	if(nDevNum <= 0)
		return -1;

	if(g_bStoped)
		return -1;

	ClearResult(GETEXTERNALIP);

	TvCtrlPointSendAction(TV_SERVICE_CONTROL, nDevNum, "GetExternalIPAddress",
				NULL, NULL, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

/*	action result function	*/
int CTRLPOINT_TravelDeviceList()
{
	struct TvDeviceNode *tmpdevnode;
	int i = 0;

	ithread_mutex_lock(&DeviceListMutex);

	SampleUtil_Print("TvCtrlPointPrintList:\n");
	tmpdevnode = GlobalDeviceList;

	if(tmpdevnode == NULL)
	{
		SampleUtil_Print("\n");
		ithread_mutex_unlock(&DeviceListMutex);
		return -1;
	}

	while (tmpdevnode) {
		SampleUtil_Print(" %3d -- %s\n", ++i, tmpdevnode->device.UDN);
		tmpdevnode = tmpdevnode->next;
	}
	SampleUtil_Print("\n");
	ithread_mutex_unlock(&DeviceListMutex);

	return i;
}


int CTRLPOINT_GetResult(EnumAction enAction, int *pnErrCode)
{
	if(g_bStoped || enAction < 0 || enAction >= ENUM_MAX || pnErrCode == NULL)
		return -1;

	ithread_mutex_lock(&actionresult_mutex);

	ActionResult	*pResult = NULL;
	char					bValid = 0;
	int						nErrCode = 0;

	pResult = &g_ActionResultArray[enAction];
	bValid = pResult->bValid;
	nErrCode = pResult->nErrCode;

	ithread_mutex_unlock(&actionresult_mutex);

	if(!bValid)
	{
		//可能action出现错误，导致未能正确填充结果
		*pnErrCode = g_nActionErrCode;
		return -1;
	}

	*pnErrCode = nErrCode;

	return 0;
}


static int Str2XmlDoc(char *xml_buf, IXML_Document **xmlDoc)
{
	int ret_code;

	if (xml_buf == NULL || xmlDoc == NULL) {
		return UPNP_E_INVALID_PARAM;
	}

	ret_code = ixmlParseBufferEx(xml_buf, xmlDoc);

	if (ret_code != IXML_SUCCESS) {
		if (ret_code == IXML_INSUFFICIENT_MEMORY) {
			UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
				"Out of memory, ixml error code: %d\n",
				ret_code);
			return UPNP_E_OUTOF_MEMORY;
		} else {
			UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
				"Invalid Description, ixml error code: %d\n",
				ret_code);
			return UPNP_E_INVALID_DESC;
		}
	} else {
#ifdef DEBUG
		xml_buf = ixmlPrintNode((IXML_Node *)*xmlDoc);
		UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
			"Printing the Parsed xml document \n %s\n", xml_buf);
		UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
			"****************** END OF Parsed XML Doc *****************\n");
		ixmlFreeDOMString(xml_buf);
#endif
		UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
			"Exiting Str2XmlDoc\n");

		return UPNP_E_SUCCESS;
	}
}


int CTRLPOINT_RESULT_GetExternalIP(char szIP[], int *pnErrCode)
{
	int 		nRet = -1;
	char		*pszIPAddr = NULL;

	if(szIP == NULL || pnErrCode == NULL)
		return -1;

	ithread_mutex_lock(&actionresult_mutex);

	ActionResult	*pResult = NULL;
	pResult = &g_ActionResultArray[GETEXTERNALIP];

	if(!pResult->bValid)
	{
		goto ERR_EXIT;
	}

	*pnErrCode = pResult->nErrCode;

	IXML_Document	*DescDoc = NULL;
	if(0 > Str2XmlDoc(pResult->pszActionResult, &DescDoc))
	{
		goto ERR_EXIT;
	}

	pszIPAddr = SampleUtil_GetFirstDocumentItem(DescDoc, "NewExternalIPAddress");
	if (pszIPAddr && *pszIPAddr)
		memcpy(szIP, pszIPAddr, strlen(pszIPAddr));

	nRet = 0;

ERR_EXIT:
	SAFE_FREE(pszIPAddr);

	if(DescDoc)
		ixmlDocument_free(DescDoc);

	ithread_mutex_unlock(&actionresult_mutex);

	return nRet;
}

int CTRLPOINT_RESULT_QueryPortMapping(PortMapping *pObjPM, int *pnErrCode)
{
	int 		nRet = -1;
	char		*pszDesc = NULL;
	char		*pszClient = NULL;
	char		*pszPort = NULL;
	char		*pszEnabled = NULL;
	char		*pszLeaseDuration = NULL;

	if(pObjPM == NULL || pnErrCode == NULL)
		return -1;

	ithread_mutex_lock(&actionresult_mutex);

	//memset(pObjPM, 0, sizeof(*pObjPM));
	pObjPM->bEnable = 0;
	pObjPM->nInterPort = 0;
	pObjPM->nLeaseDuration = 0;
	pObjPM->pszClient = 0;
	pObjPM->pszDesc = 0;

	ActionResult	*pResult = NULL;
	pResult = &g_ActionResultArray[QUERYPORTMAPPING];

	if(!pResult->bValid)
	{
		goto ERR_EXIT;
	}

	*pnErrCode = pResult->nErrCode;

	IXML_Document	*DescDoc = NULL;
	if(0 > Str2XmlDoc(pResult->pszActionResult, &DescDoc))
	{
		goto ERR_EXIT;
	}

	pszPort = SampleUtil_GetFirstDocumentItem(DescDoc, "NewInternalPort");
	pszClient = SampleUtil_GetFirstDocumentItem(DescDoc, "NewInternalClient");
	pszEnabled = SampleUtil_GetFirstDocumentItem(DescDoc, "NewEnabled");
	pszDesc = SampleUtil_GetFirstDocumentItem(DescDoc, "NewPortMappingDescription");
	pszLeaseDuration = SampleUtil_GetFirstDocumentItem(DescDoc, "NewLeaseDuration");

	if (pszPort && *pszPort)
		pObjPM->nInterPort = atoi(pszPort);

	if (pszClient && *pszClient)
	{
		int	nLen = strlen(pszClient);
		pObjPM->pszClient = (char *)malloc(nLen + 1);
		if(pObjPM->pszClient == NULL)
			goto ERR_EXIT;

		memcpy(pObjPM->pszClient, pszClient, nLen);
		pObjPM->pszClient[nLen] = 0;
	}

	if (pszEnabled)
		pObjPM->bEnable = atoi(pszEnabled);

	if (pszDesc && *pszDesc)
	{
		int	nLen = strlen(pszDesc);
		pObjPM->pszDesc = (char *)malloc(nLen + 1);
		if(pObjPM->pszDesc == NULL)
			goto ERR_EXIT;

		memcpy(pObjPM->pszDesc, pszDesc, nLen);
		pObjPM->pszDesc[nLen] = 0;
	}

	if (pszLeaseDuration)
		pObjPM->nLeaseDuration = atoi(pszLeaseDuration);

	nRet = 0;

ERR_EXIT:
	SAFE_FREE(pszPort);
	SAFE_FREE(pszClient);
	SAFE_FREE(pszEnabled);
	SAFE_FREE(pszDesc);
	SAFE_FREE(pszLeaseDuration);

	if(DescDoc)
		ixmlDocument_free(DescDoc);

	ithread_mutex_unlock(&actionresult_mutex);

	return nRet;
}



