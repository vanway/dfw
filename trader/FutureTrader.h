#pragma once
#include "ThostFtdcTraderApi.h"
#pragma pack(4)
class CFutureTrader : public CThostFtdcTraderSpi
{
		
public:
	/************����******************/
	// ���ò���

	char  BROKER_ID[15]  ;  //��½��Ϣ
	char  addr[255];
	char  pwd[15];
	char  userAccount[25];
	
	int iRequestID;

	bool bRunning;//��½�ɹ���Ϊtrue
	
	TThostFtdcMoneyType dBalance;  //�ʽ���Ϣ
	TThostFtdcMoneyType dDeposit;
	TThostFtdcMoneyType dUsable;	
	TThostFtdcMoneyType dMargin;
	
	/************����******************/
	CFutureTrader(void);
	~CFutureTrader();
	
	void loadArgs();
	void init();
    bool executeMarketOrder(char * code, int Qty, int direct, int openClose,TThostFtdcOrderRefType	ORDER_REF);  //�м۽��׺���


	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	

	///�����ѯ�ʽ��˻���Ӧ
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);



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
	///�����ѯ�ʽ��˻�
	void ReqQryTradingAccount();



	// �Ƿ��յ��ɹ�����Ӧ
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	// �Ƿ��ҵı����ر�
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	// �Ƿ����ڽ��׵ı���
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);

	
};

