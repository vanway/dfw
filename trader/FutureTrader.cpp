#include "StdAfx.h"
#include "FutureTrader.h"
#include "ThostFtdcTraderApi.h"
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)

// USER_API参数

CThostFtdcTraderApi* pTraderApi;


// 会话参数
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号


//TThostFtdcOrderRefType	ORDER_REF;	//报单引用

CFutureTrader::CFutureTrader(void){		

}
CFutureTrader::~CFutureTrader(){
//	pTraderApi->Join();
}
void CFutureTrader::OnFrontConnected()
{
	
	///用户登录请求
	//CSysLoger::logMessage("CFutureTrader::OnFrontConnected:连接成功");
	ReqUserLogin();
}

void CFutureTrader::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, this->BROKER_ID);
	strcpy(req.UserID, this->userAccount);
	strcpy(req.Password, pwd);
	int iResult = pTraderApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CFutureTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;  //全局变量      获得该服务器在交易所标志的相关信息
		
		//m_nNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		//m_nNextOrderRef++;
		//sprintf(ORDER_REF, "%d", m_nNextOrderRef);
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pTraderApi->GetTradingDay() << endl;
		///投资者结算结果确认
		ReqSettlementInfoConfirm();		
	}else{
	
		//AfxMessageBox("期货交易账户登陆失败");
	}
}

void CFutureTrader::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, this->userAccount);
	int iResult = pTraderApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	cerr << "--->>> 投资者结算结果确认: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CFutureTrader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:确认成功");
		//AfxMessageBox("期货交易账户登陆成功");
		this->bRunning = true;
	}else{
		//CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:确认失败");
		//AfxMessageBox("期货交易账户登陆失败");
		this->bRunning = false;
	}
}



//异常回调函数
void CFutureTrader:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}		
void CFutureTrader::OnHeartBeatWarning(int nTimeLapse)
{
	//AfxMessageBox("期货交易连接断开！");
	this->bRunning = false;
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CFutureTrader::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

bool CFutureTrader::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult){
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		//CSysLoger::logMessage(pRspInfo->ErrorMsg);
	}
	return bResult;
}


bool CFutureTrader::IsMyOrder(CThostFtdcOrderField *pOrder)   //根据回报中FrontID  SessionID信息判断订单是否为该服务器发出
{
	return ((pOrder->FrontID == FRONT_ID) &&
		(pOrder->SessionID == SESSION_ID));
}


bool CFutureTrader::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

void CFutureTrader::loadArgs()
{

	this->addr ="";
	this->pwd="";
	this->userAccount="";	
	BROKER_ID = "88888";				// 申万的经纪公司代码是"4070"
}
void CFutureTrader::init(){

	this->loadArgs();
	this->bRunning = false;
	pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
	pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)this);			// 注册事件类
	pTraderApi->SubscribePublicTopic(TERT_QUICK);					// 注册公有流
	pTraderApi->SubscribePrivateTopic(TERT_QUICK);					// 注册私有流
	pTraderApi->RegisterFront(this->addr.GetBuffer());							// connect
	pTraderApi->Init();

	
}

bool CFutureTrader::executeMarketOrder(char * code, int Qty, int direct, int openClose,TThostFtdcOrderRefType	ORDER_REF)
{

	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///合约代码
	strcpy(req.InstrumentID, code.GetBuffer());

	///买卖方向
	if (direct == 0) req.Direction = THOST_FTDC_D_Buy;
	if (direct == 1) req.Direction = THOST_FTDC_D_Sell;

	///组合开平标志
	if (openClose == 0) req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	if (openClose == 1) req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	if (openClose == 3) req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	
	///报单引用
	strcpy(req.OrderRef, ORDER_REF);  //传入客户标志数据
	

	///价格
	req.VolumeTotalOriginal = Qty;	
	req.LimitPrice = 0;

	///用户代码
	///报单价格条件: 市价
	req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	///经纪公司代码
	strcpy(req.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, this->userAccount.GetBuffer());	
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///有效期类型: 立即完成，否则撤销
	req.TimeCondition = THOST_FTDC_TC_IOC;
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	req.MinVolume = 1;
	///触发条件: 立即
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///止损价
	//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///用户强评标志: 否
	req.UserForceClose = 0;

	int iResult = pTraderApi->ReqOrderInsert(&req, ++iRequestID);

	if(iResult ==0){

		//returnMsg.Format("市价发单成功： %s %.2f %d %d %d", code, Qty, direct, openClose);
		//CSysLoger::logMessage(returnMsg);

		return true;
	}
	return false;
}

void CFutureTrader::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, userAccount);
	int iResult = pTraderApi->ReqQryTradingAccount(&req, ++iRequestID);
	cerr << "--->>> 请求查询资金账户: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

///成交通知
void CFutureTrader::OnRtnTrade(CThostFtdcTradeField *pTrade)
{	
	//CSysLoger::logMessage("CFutureTrader::OnRtnTrade");
	//CSysLoger::logMessage(this->CThostFdcTradeField2String(pTrade))	;
}

//报单通知
void CFutureTrader::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	

  //报单回报处理程序 根据ORDER_REF判断之前下单  具体看CThostFtdcOrderField结构数据类型
	if (IsMyOrder(pOrder))
	{		
	
		switch(pOrder->OrderSubmitStatus){
			case THOST_FTDC_OSS_InsertRejected:{
				//CTaoLiDialog::pThis->bLongMgrRunning = false;
				//CTaoLiDialog::pThis->bShortMgrRunning= false;
				//AfxMessageBox("报单失败:报单已经被拒绝");
			}
		}
	}else{

		//CSysLoger::logMessage("CFutureTrader::OnRtnOrder:OtherOrder");
	}
	//CSysLoger::logMessage(this->CThostFtdcOrderField2String(pOrder));	
}

void CFutureTrader::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	this->dBalance=0;
	this->dDeposit=0;	
	this->dUsable =0;
	this->dMargin=0;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{

		this->dUsable = pTradingAccount->Available;
		this->dMargin = pTradingAccount->CurrMargin;
		this->dBalance = pTradingAccount->Balance;
		this->dDeposit = pTradingAccount->Deposit;

	}else{
		//AfxMessageBox("查询资金失败");
	}
}
