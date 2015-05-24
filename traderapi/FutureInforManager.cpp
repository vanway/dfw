#include "FutureInforManager.h"
#include <iostream>
#include <Python.h>

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
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, this->BROKER_ID);
	strcpy(req.UserID, this->userAccount);
	strcpy(req.Password, this->pwd);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);

}

void CFutureInforManager::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();
	}
}

void CFutureInforManager::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> the result of SubscribeMarketData: " << ((iResult == 0) ? "success" : "failed") << endl;
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
	CFutureInfor infor;//将CTP的行情转化成CFutureInfor，然后存储到CInforDatabase中
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
	//memcpy(infor.szTimeStamp, pDepthMarketData->UpdateTime, 9);
    this->futureInfoFun(infor.dLastPrice);
	//cerr << pDepthMarketData->InstrumentID << infor.dLastPrice << endl;
}

bool CFutureInforManager::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
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
void CFutureInforManager::init(pInfoProcessFunc _futureInfoFun){
	//this->loadArgs();//加载参数
	this->addr = "tcp://219.143.242.147:41213";		// 前置地址
	this->BROKER_ID = "88888";				// 经纪公司代码
	this->userAccount = "8002008160";			// 投资者代码
	this->pwd = "443063";			// 用户密码
    this->futureInfoFun = _futureInfoFun;
	ppInstrumentID[0]="IF1505";			// 行情订阅列表
	iInstrumentID = 1;								// 行情订阅数量
	iRequestID = 101111;
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
	pUserApi->RegisterSpi(this);						// 注册事件类
	pUserApi->RegisterFront(this->addr);					// connect
	pUserApi->Init();
	//pUserApi->Join();
}

extern "C"
int StartSubjectFutureInfo(pInfoProcessFunc func){
    CFutureInforManager* A = new CFutureInforManager();
    cout << "runing!" << endl;
    A->init(func);
}
