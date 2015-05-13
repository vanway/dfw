#pragma once
#include "ThostFtdcMdApi.h"
#include "FutureInfor.h"
#include<linux/string.h>
#pragma pack(4)
typedef void (*pInfoProcessFunc) (double lastPrice);

class CFutureInforManager: public CThostFtdcMdSpi
{
private:
	void ReqUserLogin();//�����½
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//�жϷ��صĽ���ǲ��Ǵ���

public:	
	/****************************����*********************/
	char*  addr ;		     // ǰ�õ�ַ��Ҳ���Ƿ�������ַ
	char*	BROKER_ID ;		// ����ľ��͹�˾������"88888"
	char* userAccount;	// Ͷ���ߴ���
	char*  pwd;			// �û�����
	char *ppInstrumentID[8];// ���鶩���б�
	int iInstrumentID;	// ���鶩������	
	int iRequestID ;
    pInfoProcessFunc futureInfoFun;
	
	bool bConnected;//���ӳɹ�
	bool bLogined;//��½�ɹ�
	/**************************����********************/
	void loadArgs();//���ز�������������ַ���˻�����
	void init(pInfoProcessFunc futureInfoFun);	//��ʼ�������ز�������ʼ��CTP��API)
	CFutureInforManager(void);
	~CFutureInforManager(void);
	/*****************************************/
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason);
		
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	
	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	void SubscribeMarketData();	//�������鶩��


};

