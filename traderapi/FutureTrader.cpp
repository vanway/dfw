#include "StdAfx.h"
#include "FutureTrader.h"
#include "defines.h"
#include "ThostFtdcTraderApi.h"
#include "TaoLiDialog.h"
#include "SysLoger.h"
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)

// USER_API参数

CThostFtdcTraderApi* pTraderApi;
extern bool g_simTaoli;


// 会话参数
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号
TThostFtdcOrderRefType	ORDER_REF;	//报单引用
CFutureTrader::CFutureTrader(void){		

}
CFutureTrader::~CFutureTrader(){
//	pTraderApi->Join();
}
void CFutureTrader::OnFrontConnected()
{
	
	///用户登录请求
	CSysLoger::logMessage("CFutureTrader::OnFrontConnected:连接成功");
	ReqUserLogin();
}
void CFutureTrader::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, this->BROKER_ID);
	strcpy(req.UserID, (LPCTSTR)this->userAccount);
	strcpy(req.Password, (LPCTSTR)pwd);
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
		CSysLoger::logMessage("CFutureTrader::OnRspUserLogin:登陆成功");
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		m_nNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		m_nNextOrderRef++;
		sprintf(ORDER_REF, "%d", m_nNextOrderRef);
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pTraderApi->GetTradingDay() << endl;
		///投资者结算结果确认
		ReqSettlementInfoConfirm();		
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRspUserLogin:登陆失败");
		AfxMessageBox("期货交易账户登陆失败");
	}
}

void CFutureTrader::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);
	int iResult = pTraderApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	cerr << "--->>> 投资者结算结果确认: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CFutureTrader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:确认成功");
		AfxMessageBox("期货交易账户登陆成功");
		this->bRunning = true;
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:确认失败");
		AfxMessageBox("期货交易账户登陆失败");
		this->bRunning = false;
	}
}

void CFutureTrader::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, "IF1111");
	int iResult = pTraderApi->ReqQryInstrument(&req, ++iRequestID);
	cerr << "--->>> 请求查询合约: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CFutureTrader::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInstrument" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询合约
		ReqQryTradingAccount();
	}
}

void CFutureTrader::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, (LPCTSTR)userAccount);
	int iResult = pTraderApi->ReqQryTradingAccount(&req, ++iRequestID);
	cerr << "--->>> 请求查询资金账户: " << ((iResult == 0) ? "成功" : "失败") << endl;
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
		///请求查询投资者持仓
		//ReqQryInvestorPosition();
		this->dUsable = pTradingAccount->Available;
		this->dMargin = pTradingAccount->CurrMargin;
		this->dBalance = pTradingAccount->Balance;
		this->dDeposit = pTradingAccount->Deposit;

	}else{
		AfxMessageBox("查询资金失败");
	}
}

void CFutureTrader::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	int iResult = pTraderApi->ReqQryInvestorPosition(&req, ++iRequestID);
	cerr << "--->>> 请求查询投资者持仓: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CFutureTrader::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///报单录入请求
		//ReqOrderInsert();
	}
}


//只有在报单失败时才会回调这个函数
void CFutureTrader::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if(IsErrorRspInfo(pRspInfo)){		
		CTaoLiDialog::pThis->bLongMgrRunning = false;
		CTaoLiDialog::pThis->bShortMgrRunning= false;		
		CSysLoger::logMessage(this->CThostFtdcInputOrderField2String(pInputOrder));
		CSysLoger::logMessage("CFutureTrader::OnRspOrderInsert:报单失败");	
		AfxMessageBox(pRspInfo->ErrorMsg);
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRspOrderInsert:报单成功");	//不可能打出来
	}
}
//暂时没有用到
void CFutureTrader::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool ORDER_ACTION_SENT = false;		//是否发送了报单
	if (ORDER_ACTION_SENT)
		return;

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, pOrder->BrokerID);
	///投资者代码
	strcpy(req.InvestorID, pOrder->InvestorID);
	///报单操作引用
//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	strcpy(req.OrderRef, pOrder->OrderRef);
	///请求编号
//	TThostFtdcRequestIDType	RequestID;
	///前置编号
	req.FrontID = FRONT_ID;
	///会话编号
	req.SessionID = SESSION_ID;
	///交易所代码
//	TThostFtdcExchangeIDType	ExchangeID;
	///报单编号
//	TThostFtdcOrderSysIDType	OrderSysID;
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///价格
//	TThostFtdcPriceType	LimitPrice;
	///数量变化
//	TThostFtdcVolumeType	VolumeChange;
	///用户代码
//	TThostFtdcUserIDType	UserID;
	///合约代码
	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = pTraderApi->ReqOrderAction(&req, ++iRequestID);
	cerr << "--->>> 报单操作请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
	ORDER_ACTION_SENT = true;
}
//暂时没用到
void CFutureTrader::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

///报单通知
void CFutureTrader::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	
	if (IsMyOrder(pOrder))
	{		
		CSysLoger::logMessage("CFutureTrader::OnRtnOrder:MyOrder");
		switch(pOrder->OrderSubmitStatus){
			case THOST_FTDC_OSS_InsertRejected:{
				CTaoLiDialog::pThis->bLongMgrRunning = false;
				CTaoLiDialog::pThis->bShortMgrRunning= false;
				AfxMessageBox("报单失败:报单已经被拒绝");
			}
		}
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRtnOrder:OtherOrder");
	}
	CSysLoger::logMessage(this->CThostFtdcOrderField2String(pOrder));	
}

///成交通知
void CFutureTrader::OnRtnTrade(CThostFtdcTradeField *pTrade)
{	
	CSysLoger::logMessage("CFutureTrader::OnRtnTrade");
	CSysLoger::logMessage(this->CThostFdcTradeField2String(pTrade))	;
}

void CFutureTrader:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void CFutureTrader::OnHeartBeatWarning(int nTimeLapse)
{
	AfxMessageBox("期货交易连接断开！");
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
		CSysLoger::logMessage(pRspInfo->ErrorMsg);
	}
	return bResult;
}

bool CFutureTrader::IsMyOrder(CThostFtdcOrderField *pOrder)
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
void CFutureTrader::loadArgs(){

	char temp[512];
	this->addr ="";
	this->pwd="";
	this->userAccount="";	
	::GetPrivateProfileString("FUTURE_TRADE_SERVER","ADDR","",temp,MAX_PATH,FUTURE_TRADE_INI_FILE_PATH);	
	this->addr.Format("%s",temp);

	::GetPrivateProfileString("FUTURE_TRADE_SERVER","USER_ACCOUNT","",temp,MAX_PATH,FUTURE_TRADE_INI_FILE_PATH);	
	this->userAccount.Format("%s",temp);

	::GetPrivateProfileString("FUTURE_TRADE_SERVER","PWD","",temp,MAX_PATH,FUTURE_TRADE_INI_FILE_PATH);	
	this->pwd.Format("%s",temp);
	::GetPrivateProfileString("FUTURE_TRADE_SERVER","BROKERID","",temp,MAX_PATH,FUTURE_TRADE_INI_FILE_PATH);	
	strcpy(BROKER_ID,temp);
	
}
void CFutureTrader::init(){
	if(this->bRunning){
		return;
	}
	this->loadArgs();
	//BROKER_ID = "88888";				// 申万的经纪公司代码是"4070"
	this->bRunning = false;
	pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
	pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)this);			// 注册事件类
	pTraderApi->SubscribePublicTopic(TERT_QUICK);					// 注册公有流
	pTraderApi->SubscribePrivateTopic(TERT_QUICK);					// 注册私有流
	pTraderApi->RegisterFront(this->addr.GetBuffer());							// connect
	pTraderApi->Init();

	
}
bool CFutureTrader::buyOpen(CString code,double price,int Qty){
	if(g_simTaoli)
		return true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///合约代码
	strcpy(req.InstrumentID, (LPCTSTR)code);
	///买卖方向: 
	req.Direction = THOST_FTDC_D_Buy;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///价格
	req.VolumeTotalOriginal = Qty;
	///数量: 1
	req.LimitPrice = price;
		
	///报单引用
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///用户代码
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///经纪公司代码
	strcpy(req.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///有效期类型: 当日有效
	req.TimeCondition = THOST_FTDC_TC_GFD;
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
		CString logMsg;
		logMsg.Format("买入开仓发单成功;合约:%s;价格:%.2f;数量:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
		CSysLoger::logMessage(logMsg);
		return true;
	}
	return false;
}
bool CFutureTrader::sellOpen(CString code,double price,int Qty){//卖出开仓
//	return true;
	if(g_simTaoli)
		return true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///合约代码
	strcpy(req.InstrumentID, (LPCTSTR)code);
	///买卖方向: 
	req.Direction = THOST_FTDC_D_Sell;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///价格
	req.VolumeTotalOriginal = Qty;
	///数量: 1
	req.LimitPrice =price;

	///报单引用
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///用户代码
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///经纪公司代码
	strcpy(req.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///有效期类型: 当日有效
	req.TimeCondition = THOST_FTDC_TC_GFD;
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
		CString logMsg;
		logMsg.Format("卖出开仓发单成功;合约:%s;价格:%.2f;数量:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
		CSysLoger::logMessage(logMsg);
		return true;
	}
	return false;
}
bool CFutureTrader::buyClose(CString code,double price,int Qty){//买入平仓
	if(g_simTaoli)
		return true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///合约代码
	strcpy(req.InstrumentID,(LPCTSTR) code);
	///买卖方向: 
	req.Direction = THOST_FTDC_D_Buy;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	///价格
	req.VolumeTotalOriginal = Qty;
	///数量: 1
	req.LimitPrice = price;

	///报单引用
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///用户代码
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///经纪公司代码
	strcpy(req.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///有效期类型: 当日有效
	req.TimeCondition = THOST_FTDC_TC_GFD;
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
		CString logMsg;
		logMsg.Format("买入平仓发单成功;合约:%s;价格:%.2f;数量:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
		CSysLoger::logMessage(logMsg);
		return true;
	}
	return false;
}
bool CFutureTrader::sellClose(CString code,double price,int Qty){
	if(g_simTaoli)
		return true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///合约代码
	strcpy(req.InstrumentID, (LPCTSTR)code);
	///买卖方向: 
	req.Direction = THOST_FTDC_D_Sell;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	///价格
	req.VolumeTotalOriginal = Qty;
	///数量: 1
	req.LimitPrice = price;

	///报单引用
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///用户代码
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///经纪公司代码
	strcpy(req.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///有效期类型: 当日有效
	req.TimeCondition = THOST_FTDC_TC_GFD;
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
		CString logMsg;
		logMsg.Format("卖出平仓发单成功;合约:%s;价格:%.2f;数量:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
		CSysLoger::logMessage(logMsg);
		return true;
	}
	return false;
}
CString CFutureTrader::CThostFtdcOrderField2String(CThostFtdcOrderField * pOrder){
	CString msg;
	msg.Format("编号:%s;合约%s;操作:报单;",pOrder->OrderSysID,pOrder->InstrumentID);
	switch(pOrder->Direction){
	case THOST_FTDC_D_Buy:msg.AppendFormat("买;");break;
	case THOST_FTDC_D_Sell:msg.AppendFormat("卖;");break;
	}
	switch(pOrder->CombOffsetFlag[0]){
		case THOST_FTDC_OF_Open:msg.AppendFormat("开;");break;
		case THOST_FTDC_OF_Close:msg.AppendFormat("平;");break;
	}
	msg.AppendFormat("价格:%.2f;数量:%d;状态:%s",pOrder->LimitPrice,pOrder->VolumeTotalOriginal,pOrder->StatusMsg);
	return msg;
}
CString CFutureTrader::CThostFdcTradeField2String(CThostFtdcTradeField * pTrade){
	CString msg;
	msg.Format("编号:%s;合约%s;操作:成交; ",pTrade->OrderSysID,pTrade->InstrumentID);
	switch(pTrade->Direction){
	case THOST_FTDC_D_Buy:msg.AppendFormat("买;");break;
	case THOST_FTDC_D_Sell:msg.AppendFormat("卖;");break;
	}
	switch(pTrade->OffsetFlag){
	case THOST_FTDC_OF_Open:msg.AppendFormat("开;");break;
	case THOST_FTDC_OF_Close:msg.AppendFormat("平;");break;
	case THOST_FTDC_OF_ForceClose:msg.AppendFormat("强平;");break;
	case THOST_FTDC_OF_CloseToday:msg.AppendFormat("平今;");break;
	case THOST_FTDC_OF_CloseYesterday:msg.AppendFormat("平昨;");break;
	}
	msg.AppendFormat("价格:%.2f;数量:%d",pTrade->Price,pTrade->Volume);
	return msg;
}
CString CFutureTrader::CThostFtdcInputOrderField2String(CThostFtdcInputOrderField *pInput){
	CString msg;
	msg.Format("编号:无;合约%s;操作:报单错误;",pInput->InstrumentID);
	switch(pInput->Direction){
	case THOST_FTDC_D_Buy:msg.AppendFormat("买;");break;
	case THOST_FTDC_D_Sell:msg.AppendFormat("卖;");break;
	}
	switch(pInput->CombOffsetFlag[0]){
	case THOST_FTDC_OF_Open:msg.AppendFormat("开;");break;
	case THOST_FTDC_OF_Close:msg.AppendFormat("平;");break;
	}
	msg.AppendFormat("价格:%.2f;数量:%d",pInput->LimitPrice,pInput->VolumeTotalOriginal);
	return msg;
}
