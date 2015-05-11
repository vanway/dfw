#include "FutureInforManager.h"
#include <iostream>

using namespace std;

CThostFtdcMdApi* pUserApi;

CFutureInforManager::CFutureInforManager(void)
{
}

CFutureInforManager::~CFutureInforManager(void)
{
}

void CFutureInforManager::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	
	IsErrorRspInfo(pRspInfo);
}

void CFutureInforManager::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void CFutureInforManager::OnHeartBeatWarning(int nTimeLapse)
{
	
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CFutureInforManager::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	ReqUserLogin();
}

void CFutureInforManager::ReqUserLogin()
{
	//CThostFtdcReqUserLoginField req;
	//memset(&req, 0, sizeof(req));
	//strcpy(req.BrokerID, (LPCTSTR)this->BROKER_ID);
	//strcpy(req.UserID, (LPCTSTR)this->userAccount);
	//strcpy(req.Password, (LPCTSTR)this->pwd);
	//int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);

}

void CFutureInforManager::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr << "--->>> " << __FUNCTION__ << endl;
	//if (bIsLast && !IsErrorRspInfo(pRspInfo))
	//{
		///��ȡ��ǰ������
	//	cerr << "--->>> ��ȡ��ǰ������ = " << pUserApi->GetTradingDay() << endl;
		// ����������
	//	SubscribeMarketData();	
	//}
}

void CFutureInforManager::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> �������鶩������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CFutureInforManager::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
}

void CFutureInforManager::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << __FUNCTION__ << endl;
}

void CFutureInforManager::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
//	CLog4C::logMessage("OnRtnDepthMarketData",".\\futureInfor.log");
	CFutureInfor infor;//��CTP������ת����CFutureInfor��Ȼ��洢��CInforDatabase��
	strCode.Format("%s",pDepthMarketData->InstrumentID);
	infor.dLastPrice = pDepthMarketData->LastPrice;
	infor.dAskPrice[0] = pDepthMarketData->AskPrice1;
	infor.dAskPrice[1] = pDepthMarketData->AskPrice2;
	infor.dAskPrice[2] = pDepthMarketData->AskPrice3;
	infor.dAskPrice[3] = pDepthMarketData->AskPrice4;
	infor.dAskPrice[4] = pDepthMarketData->AskPrice5;

	infor.dBidPrice[0] = pDepthMarketData->BidPrice1;
	infor.dBidPrice[1] = pDepthMarketData->BidPrice2;
	infor.dBidPrice[2] = pDepthMarketData->BidPrice3;
	infor.dBidPrice[3] = pDepthMarketData->BidPrice4;
	infor.dBidPrice[4] = pDepthMarketData->BidPrice5;

	infor.nAskVol[0] = pDepthMarketData->AskVolume1;
	infor.nAskVol[1] = pDepthMarketData->AskVolume2;
	infor.nAskVol[2] = pDepthMarketData->AskVolume3;
	infor.nAskVol[3] = pDepthMarketData->AskVolume4;
	infor.nAskVol[4] = pDepthMarketData->AskVolume5;

	infor.nBidVol[0] = pDepthMarketData->BidVolume1;
	infor.nBidVol[1] = pDepthMarketData->BidVolume2;
	infor.nBidVol[2] = pDepthMarketData->BidVolume3;
	infor.nBidVol[3] = pDepthMarketData->BidVolume4;
	infor.nBidVol[4] = pDepthMarketData->BidVolume5;
	memcpy(infor.szTimeStamp, pDepthMarketData->UpdateTime, 9);
}

bool CFutureInforManager::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void CFutureInforManager::loadArgs(){
	//CString temp;
	//temp="";
	//::GetPrivateProfileString("FUTURE_INFOR_SERVER","ADDR","",temp.GetBuffer(MAX_PATH),MAX_PATH,FUTURE_INFOR_INI_FILE_PATH);	
	//temp.ReleaseBuffer();
	//this->addr.Format("%s",temp);
	

	//::GetPrivateProfileString("FUTURE_INFOR_SERVER","USER_ACCOUNT","",temp.GetBuffer(MAX_PATH),MAX_PATH,FUTURE_INFOR_INI_FILE_PATH);	
	//temp.ReleaseBuffer();
	//this->userAccount.Format("%s",temp);

	//::GetPrivateProfileString("FUTURE_INFOR_SERVER","PWD","",temp.GetBuffer(MAX_PATH),MAX_PATH,FUTURE_INFOR_INI_FILE_PATH);	
	//temp.ReleaseBuffer();
	//this->pwd.Format("%s",temp);

	//::GetPrivateProfileString("FUTURE_INFOR_SERVER","SUBSCRIBED_CONTRACTS","",temp.GetBuffer(MAX_PATH),MAX_PATH,FUTURE_INFOR_INI_FILE_PATH);
	//temp.ReleaseBuffer();
	//int curPos= 0;
	//CString resToken="";
	//resToken= temp.Tokenize(",",curPos);
	//iInstrumentID =0;
	//while (resToken != "")
	//{
	//	ppInstrumentID[iInstrumentID] = new char[16];
	//	strcpy(ppInstrumentID[iInstrumentID],(LPCTSTR)resToken);
	//	resToken= temp.Tokenize(",",curPos);
	//	iInstrumentID++;
	//};

}
void CFutureInforManager::init(){

	//this->loadArgs();//���ز���
	//this->addr = "tcp://dxzd-front2.sywgqh.com:18215";		// ǰ�õ�ַ
	////strcpy(BROKER_ID, "88888");				// ���͹�˾����
	////strcpy(INVESTOR_ID , "8002008160");			// Ͷ���ߴ���
	////strcpy(PASSWORD , "443063");			// �û�����
	//ppInstrumentID[0]="IF1110";			// ���鶩���б�
	//iInstrumentID = 1;								// ���鶩������
	iRequestID = 0;
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi
	pUserApi->RegisterSpi(this);						// ע���¼���
	pUserApi->RegisterFront(this->addr.GetBuffer());					// connect
	pUserApi->Init();

	//pUserApi->Join();
}


