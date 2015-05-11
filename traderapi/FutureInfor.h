#pragma once
#pragma pack(4)
class CFutureInfor
{
public:
	CFutureInfor(void);
	~CFutureInfor(void);
	//现在的行情只有一档行情（买一、卖一），其它的四档行情暂时没有使用
	/***********变量*********************/		
	double dLastPrice;      //最新价
	double dAskPrice[5];	//申卖价
	int    nAskVol[5];	    //申卖量
	double dBidPrice[5];   //申买价
	int    nBidVol[5];	  //申买量
	char   szTimeStamp[9];//时间hh:mm:ss
};

