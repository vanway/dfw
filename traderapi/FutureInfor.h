#pragma once
#pragma pack(4)
class CFutureInfor
{
public:
	CFutureInfor(void);
	~CFutureInfor(void);
	//���ڵ�����ֻ��һ�����飨��һ����һ�����������ĵ�������ʱû��ʹ��
	/***********����*********************/		
	double dLastPrice;      //���¼�
	double dAskPrice[5];	//������
	int    nAskVol[5];	    //������
	double dBidPrice[5];   //�����
	int    nBidVol[5];	  //������
	char   szTimeStamp[9];//ʱ��hh:mm:ss
};

