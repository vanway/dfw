#pragma once
#include "ThostFtdcTraderApi.h"
#pragma pack(4)

class CFutureTrader : public CThostFtdcTraderSpi
{
		
public:
	/************����******************/
	// ���ò���
	char * FRONT_ADDR ;
	char 	BROKER_ID[15]  ;
	char * INVESTOR_ID ;
	char *  PASSWORD ;
	TThostFtdcInstrumentIDType INSTRUMENT_ID ;	// ��Լ����
	TThostFtdcDirectionType	DIRECTION ;	// ��������
	TThostFtdcPriceType	LIMIT_PRICE ;				// �۸�
	int iRequestID;
	CString addr;
	CString pwd;
	CString userAccount;
	bool bRunning;//��½�ɹ���Ϊtrue
	int m_nNextOrderRef;
	
	TThostFtdcMoneyType dBalance;
	TThostFtdcMoneyType dDeposit;
	TThostFtdcMoneyType dUsable;	
	TThostFtdcMoneyType dMargin;
	/************����******************/
	CFutureTrader(void);
	~CFutureTrader();
	void loadArgs();
	void init();
	bool buyOpen(CString code,double price,int Qty);
	bool buyClose(CString code,double price,int Qty);
	bool sellOpen(CString code,double price,int Qty);
	bool sellClose(CString code,double price,int Qty);
	CString CThostFtdcOrderField2String(CThostFtdcOrderField * pOrder);
	CString CThostFdcTradeField2String(CThostFtdcTradeField * pTrade);
	CString CThostFtdcInputOrderField2String(CThostFtdcInputOrderField *pInput);
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯͶ���ֲ߳���Ӧ
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);
		
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	

	///�û���¼����
	void ReqUserLogin();
	///Ͷ���߽�����ȷ��
	void ReqSettlementInfoConfirm();
	///�����ѯ��Լ
	void ReqQryInstrument();
	///�����ѯ�ʽ��˻�
	void ReqQryTradingAccount();
	///�����ѯͶ���ֲ߳�
	void ReqQryInvestorPosition();

	///������������
	void ReqOrderAction(CThostFtdcOrderField *pOrder);

	// �Ƿ��յ��ɹ�����Ӧ
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	// �Ƿ��ҵı����ر�
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	// �Ƿ����ڽ��׵ı���
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);

	
};

