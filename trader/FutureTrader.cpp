#include "StdAfx.h"
#include "FutureTrader.h"
#include "ThostFtdcTraderApi.h"
#include <iostream>
using namespace std;

#pragma warning(disable : 4996)

// USER_API����

CThostFtdcTraderApi* pTraderApi;


// �Ự����
TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
TThostFtdcSessionIDType	SESSION_ID;	//�Ự���


//TThostFtdcOrderRefType	ORDER_REF;	//��������

CFutureTrader::CFutureTrader(void){		

}
CFutureTrader::~CFutureTrader(){
//	pTraderApi->Join();
}
void CFutureTrader::OnFrontConnected()
{
	
	///�û���¼����
	//CSysLoger::logMessage("CFutureTrader::OnFrontConnected:���ӳɹ�");
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
	cerr << "--->>> �����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CFutureTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ����Ự����
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;  //ȫ�ֱ���      ��ø÷������ڽ�������־�������Ϣ
		
		//m_nNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		//m_nNextOrderRef++;
		//sprintf(ORDER_REF, "%d", m_nNextOrderRef);
		///��ȡ��ǰ������
		cerr << "--->>> ��ȡ��ǰ������ = " << pTraderApi->GetTradingDay() << endl;
		///Ͷ���߽�����ȷ��
		ReqSettlementInfoConfirm();		
	}else{
	
		//AfxMessageBox("�ڻ������˻���½ʧ��");
	}
}

void CFutureTrader::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, this->userAccount);
	int iResult = pTraderApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	cerr << "--->>> Ͷ���߽�����ȷ��: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CFutureTrader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:ȷ�ϳɹ�");
		//AfxMessageBox("�ڻ������˻���½�ɹ�");
		this->bRunning = true;
	}else{
		//CSysLoger::logMessage("CFutureTrader::OnRspSettlementInfoConfirm:ȷ��ʧ��");
		//AfxMessageBox("�ڻ������˻���½ʧ��");
		this->bRunning = false;
	}
}



//�쳣�ص�����
void CFutureTrader:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}		
void CFutureTrader::OnHeartBeatWarning(int nTimeLapse)
{
	//AfxMessageBox("�ڻ��������ӶϿ���");
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
		//CSysLoger::logMessage(pRspInfo->ErrorMsg);
	}
	return bResult;
}


bool CFutureTrader::IsMyOrder(CThostFtdcOrderField *pOrder)   //���ݻر���FrontID  SessionID��Ϣ�ж϶����Ƿ�Ϊ�÷���������
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
	BROKER_ID = "88888";				// ����ľ��͹�˾������"4070"
}
void CFutureTrader::init(){

	this->loadArgs();
	this->bRunning = false;
	pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi
	pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)this);			// ע���¼���
	pTraderApi->SubscribePublicTopic(TERT_QUICK);					// ע�ṫ����
	pTraderApi->SubscribePrivateTopic(TERT_QUICK);					// ע��˽����
	pTraderApi->RegisterFront(this->addr.GetBuffer());							// connect
	pTraderApi->Init();

	
}

bool CFutureTrader::executeMarketOrder(char * code, int Qty, int direct, int openClose,TThostFtdcOrderRefType	ORDER_REF)
{

	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///��Լ����
	strcpy(req.InstrumentID, code.GetBuffer());

	///��������
	if (direct == 0) req.Direction = THOST_FTDC_D_Buy;
	if (direct == 1) req.Direction = THOST_FTDC_D_Sell;

	///��Ͽ�ƽ��־
	if (openClose == 0) req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	if (openClose == 1) req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	if (openClose == 3) req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	
	///��������
	strcpy(req.OrderRef, ORDER_REF);  //����ͻ���־����
	

	///�۸�
	req.VolumeTotalOriginal = Qty;	
	req.LimitPrice = 0;

	///�û�����
	///�����۸�����: �м�
	req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	///���͹�˾����
	strcpy(req.BrokerID, BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, this->userAccount.GetBuffer());	
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	
	///��Ч������: ������ɣ�������
	req.TimeCondition = THOST_FTDC_TC_IOC;
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

		//returnMsg.Format("�м۷����ɹ��� %s %.2f %d %d %d", code, Qty, direct, openClose);
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
	cerr << "--->>> �����ѯ�ʽ��˻�: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

///�ɽ�֪ͨ
void CFutureTrader::OnRtnTrade(CThostFtdcTradeField *pTrade)
{	
	//CSysLoger::logMessage("CFutureTrader::OnRtnTrade");
	//CSysLoger::logMessage(this->CThostFdcTradeField2String(pTrade))	;
}

//����֪ͨ
void CFutureTrader::OnRtnOrder(CThostFtdcOrderField *pOrder)
{	

  //�����ر�������� ����ORDER_REF�ж�֮ǰ�µ�  ���忴CThostFtdcOrderField�ṹ��������
	if (IsMyOrder(pOrder))
	{		
	
		switch(pOrder->OrderSubmitStatus){
			case THOST_FTDC_OSS_InsertRejected:{
				//CTaoLiDialog::pThis->bLongMgrRunning = false;
				//CTaoLiDialog::pThis->bShortMgrRunning= false;
				//AfxMessageBox("����ʧ��:�����Ѿ����ܾ�");
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
		//AfxMessageBox("��ѯ�ʽ�ʧ��");
	}
}
