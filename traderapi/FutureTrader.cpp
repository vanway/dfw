#include "StdAfx.h"
#include "FutureTrader.h"
#include "defines.h"
#include "ThostFtdcTraderApi.h"
#include "TaoLiDialog.h"
#include "SysLoger.h"
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)

// USER_API����

CThostFtdcTraderApi* pTraderApi;
extern bool g_simTaoli;


// �Ự����
TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
TThostFtdcOrderRefType	ORDER_REF;	//��������
CFutureTrader::CFutureTrader(void){		

}
CFutureTrader::~CFutureTrader(){
//	pTraderApi->Join();
}
void CFutureTrader::OnFrontConnected()
{
	
	///�û���¼����
	CSysLoger::logMessage("CFutureTrader::OnFrontConnected:���ӳɹ�");
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
	cerr << "--->>> �����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CFutureTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ����Ự����
		CSysLoger::logMessage("CFutureTrader::OnRspUserLogin:��½�ɹ�");
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		m_nNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		m_nNextOrderRef++;
		sprintf(ORDER_REF, "%d", m_nNextOrderRef);
		///��ȡ��ǰ������
		cerr << "--->>> ��ȡ��ǰ������ = " << pTraderApi->GetTradingDay() << endl;
		///Ͷ���߽�����ȷ��
		ReqSettlementInfoConfirm();		
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRspUserLogin:��½ʧ��");
		AfxMessageBox("�ڻ������˻���½ʧ��");
	}
}

void CFutureTrader::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);
	int iResult = pTraderApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	cerr << "--->>> Ͷ���߽�����ȷ��: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CFutureTrader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:ȷ�ϳɹ�");
		AfxMessageBox("�ڻ������˻���½�ɹ�");
		this->bRunning = true;
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:ȷ��ʧ��");
		AfxMessageBox("�ڻ������˻���½ʧ��");
		this->bRunning = false;
	}
}

void CFutureTrader::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, "IF1111");
	int iResult = pTraderApi->ReqQryInstrument(&req, ++iRequestID);
	cerr << "--->>> �����ѯ��Լ: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CFutureTrader::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInstrument" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///�����ѯ��Լ
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
	cerr << "--->>> �����ѯ�ʽ��˻�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
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
		///�����ѯͶ���ֲ߳�
		//ReqQryInvestorPosition();
		this->dUsable = pTradingAccount->Available;
		this->dMargin = pTradingAccount->CurrMargin;
		this->dBalance = pTradingAccount->Balance;
		this->dDeposit = pTradingAccount->Deposit;

	}else{
		AfxMessageBox("��ѯ�ʽ�ʧ��");
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
	cerr << "--->>> �����ѯͶ���ֲ߳�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CFutureTrader::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///����¼������
		//ReqOrderInsert();
	}
}


//ֻ���ڱ���ʧ��ʱ�Ż�ص��������
void CFutureTrader::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if(IsErrorRspInfo(pRspInfo)){		
		CTaoLiDialog::pThis->bLongMgrRunning = false;
		CTaoLiDialog::pThis->bShortMgrRunning= false;		
		CSysLoger::logMessage(this->CThostFtdcInputOrderField2String(pInputOrder));
		CSysLoger::logMessage("CFutureTrader::OnRspOrderInsert:����ʧ��");	
		AfxMessageBox(pRspInfo->ErrorMsg);
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRspOrderInsert:�����ɹ�");	//�����ܴ����
	}
}
//��ʱû���õ�
void CFutureTrader::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool ORDER_ACTION_SENT = false;		//�Ƿ����˱���
	if (ORDER_ACTION_SENT)
		return;

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pOrder->InvestorID);
	///������������
//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrder->OrderRef);
	///������
//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID = FRONT_ID;
	///�Ự���
	req.SessionID = SESSION_ID;
	///����������
//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
//	TThostFtdcOrderSysIDType	OrderSysID;
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///�۸�
//	TThostFtdcPriceType	LimitPrice;
	///�����仯
//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
//	TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = pTraderApi->ReqOrderAction(&req, ++iRequestID);
	cerr << "--->>> ������������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	ORDER_ACTION_SENT = true;
}
//��ʱû�õ�
void CFutureTrader::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

///����֪ͨ
void CFutureTrader::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	
	if (IsMyOrder(pOrder))
	{		
		CSysLoger::logMessage("CFutureTrader::OnRtnOrder:MyOrder");
		switch(pOrder->OrderSubmitStatus){
			case THOST_FTDC_OSS_InsertRejected:{
				CTaoLiDialog::pThis->bLongMgrRunning = false;
				CTaoLiDialog::pThis->bShortMgrRunning= false;
				AfxMessageBox("����ʧ��:�����Ѿ����ܾ�");
			}
		}
	}else{
		CSysLoger::logMessage("CFutureTrader::OnRtnOrder:OtherOrder");
	}
	CSysLoger::logMessage(this->CThostFtdcOrderField2String(pOrder));	
}

///�ɽ�֪ͨ
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
	AfxMessageBox("�ڻ��������ӶϿ���");
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
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
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
	//BROKER_ID = "88888";				// ����ľ��͹�˾������"4070"
	this->bRunning = false;
	pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi
	pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)this);			// ע���¼���
	pTraderApi->SubscribePublicTopic(TERT_QUICK);					// ע�ṫ����
	pTraderApi->SubscribePrivateTopic(TERT_QUICK);					// ע��˽����
	pTraderApi->RegisterFront(this->addr.GetBuffer());							// connect
	pTraderApi->Init();

	
}
bool CFutureTrader::buyOpen(CString code,double price,int Qty){
	if(g_simTaoli)
		return true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///��Լ����
	strcpy(req.InstrumentID, (LPCTSTR)code);
	///��������: 
	req.Direction = THOST_FTDC_D_Buy;
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///�۸�
	req.VolumeTotalOriginal = Qty;
	///����: 1
	req.LimitPrice = price;
		
	///��������
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///�û�����
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///���͹�˾����
	strcpy(req.BrokerID, BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	int iResult = pTraderApi->ReqOrderInsert(&req, ++iRequestID);
	if(iResult ==0){		
		CString logMsg;
		logMsg.Format("���뿪�ַ����ɹ�;��Լ:%s;�۸�:%.2f;����:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
		CSysLoger::logMessage(logMsg);
		return true;
	}
	return false;
}
bool CFutureTrader::sellOpen(CString code,double price,int Qty){//��������
//	return true;
	if(g_simTaoli)
		return true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///��Լ����
	strcpy(req.InstrumentID, (LPCTSTR)code);
	///��������: 
	req.Direction = THOST_FTDC_D_Sell;
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	///�۸�
	req.VolumeTotalOriginal = Qty;
	///����: 1
	req.LimitPrice =price;

	///��������
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///�û�����
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///���͹�˾����
	strcpy(req.BrokerID, BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	int iResult = pTraderApi->ReqOrderInsert(&req, ++iRequestID);
	if(iResult ==0){
		CString logMsg;
		logMsg.Format("�������ַ����ɹ�;��Լ:%s;�۸�:%.2f;����:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
		CSysLoger::logMessage(logMsg);
		return true;
	}
	return false;
}
bool CFutureTrader::buyClose(CString code,double price,int Qty){//����ƽ��
	if(g_simTaoli)
		return true;
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///��Լ����
	strcpy(req.InstrumentID,(LPCTSTR) code);
	///��������: 
	req.Direction = THOST_FTDC_D_Buy;
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	///�۸�
	req.VolumeTotalOriginal = Qty;
	///����: 1
	req.LimitPrice = price;

	///��������
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///�û�����
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///���͹�˾����
	strcpy(req.BrokerID, BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	int iResult = pTraderApi->ReqOrderInsert(&req, ++iRequestID);
	if(iResult ==0){		
		CString logMsg;
		logMsg.Format("����ƽ�ַ����ɹ�;��Լ:%s;�۸�:%.2f;����:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
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
	///��Լ����
	strcpy(req.InstrumentID, (LPCTSTR)code);
	///��������: 
	req.Direction = THOST_FTDC_D_Sell;
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	///�۸�
	req.VolumeTotalOriginal = Qty;
	///����: 1
	req.LimitPrice = price;

	///��������
	strcpy(req.OrderRef, ORDER_REF);
	m_nNextOrderRef++;
	sprintf(ORDER_REF, "%d", m_nNextOrderRef);
	///�û�����
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///���͹�˾����
	strcpy(req.BrokerID, BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, (LPCTSTR)this->userAccount);	
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	//	TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	int iResult = pTraderApi->ReqOrderInsert(&req, ++iRequestID);
	if(iResult ==0){
		CString logMsg;
		logMsg.Format("����ƽ�ַ����ɹ�;��Լ:%s;�۸�:%.2f;����:%d;order_ref:%s",code,price,Qty,ORDER_REF);		
		CSysLoger::logMessage(logMsg);
		return true;
	}
	return false;
}
CString CFutureTrader::CThostFtdcOrderField2String(CThostFtdcOrderField * pOrder){
	CString msg;
	msg.Format("���:%s;��Լ%s;����:����;",pOrder->OrderSysID,pOrder->InstrumentID);
	switch(pOrder->Direction){
	case THOST_FTDC_D_Buy:msg.AppendFormat("��;");break;
	case THOST_FTDC_D_Sell:msg.AppendFormat("��;");break;
	}
	switch(pOrder->CombOffsetFlag[0]){
		case THOST_FTDC_OF_Open:msg.AppendFormat("��;");break;
		case THOST_FTDC_OF_Close:msg.AppendFormat("ƽ;");break;
	}
	msg.AppendFormat("�۸�:%.2f;����:%d;״̬:%s",pOrder->LimitPrice,pOrder->VolumeTotalOriginal,pOrder->StatusMsg);
	return msg;
}
CString CFutureTrader::CThostFdcTradeField2String(CThostFtdcTradeField * pTrade){
	CString msg;
	msg.Format("���:%s;��Լ%s;����:�ɽ�; ",pTrade->OrderSysID,pTrade->InstrumentID);
	switch(pTrade->Direction){
	case THOST_FTDC_D_Buy:msg.AppendFormat("��;");break;
	case THOST_FTDC_D_Sell:msg.AppendFormat("��;");break;
	}
	switch(pTrade->OffsetFlag){
	case THOST_FTDC_OF_Open:msg.AppendFormat("��;");break;
	case THOST_FTDC_OF_Close:msg.AppendFormat("ƽ;");break;
	case THOST_FTDC_OF_ForceClose:msg.AppendFormat("ǿƽ;");break;
	case THOST_FTDC_OF_CloseToday:msg.AppendFormat("ƽ��;");break;
	case THOST_FTDC_OF_CloseYesterday:msg.AppendFormat("ƽ��;");break;
	}
	msg.AppendFormat("�۸�:%.2f;����:%d",pTrade->Price,pTrade->Volume);
	return msg;
}
CString CFutureTrader::CThostFtdcInputOrderField2String(CThostFtdcInputOrderField *pInput){
	CString msg;
	msg.Format("���:��;��Լ%s;����:��������;",pInput->InstrumentID);
	switch(pInput->Direction){
	case THOST_FTDC_D_Buy:msg.AppendFormat("��;");break;
	case THOST_FTDC_D_Sell:msg.AppendFormat("��;");break;
	}
	switch(pInput->CombOffsetFlag[0]){
	case THOST_FTDC_OF_Open:msg.AppendFormat("��;");break;
	case THOST_FTDC_OF_Close:msg.AppendFormat("ƽ;");break;
	}
	msg.AppendFormat("�۸�:%.2f;����:%d",pInput->LimitPrice,pInput->VolumeTotalOriginal);
	return msg;
}
