#pragma once
#include "ThostFtdcMdApi.h"
#include "FutureInfor.h"
#include<linux/string.h>
#pragma pack(4)
typedef void (*pInfoProcessFunc) (double lastPrice);

class CFutureInforManager: public CThostFtdcMdSpi
{
private:
	void ReqUserLogin();//请求登陆
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//判断返回的结果是不是错误

public:	
	/****************************变量*********************/
	char*  addr ;		     // 前置地址，也就是服务器地址
	char*	BROKER_ID ;		// 申万的经纪公司代码是"88888"
	char* userAccount;	// 投资者代码
	char*  pwd;			// 用户密码
	char *ppInstrumentID[8];// 行情订阅列表
	int iInstrumentID;	// 行情订阅数量	
	int iRequestID ;
    pInfoProcessFunc futureInfoFun;
	
	bool bConnected;//连接成功
	bool bLogined;//登陆成功
	/**************************方法********************/
	void loadArgs();//加载参数，服务器地址，账户密码
	void init(pInfoProcessFunc futureInfoFun);	//初始化（加载参数，初始化CTP的API)
	CFutureInforManager(void);
	~CFutureInforManager(void);
	/*****************************************/
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);
		
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	void SubscribeMarketData();	//请求行情订阅


};

