#ifndef MD_SPI_H_
#define MD_SPI_H_

#include "ThostFtdcMdApi.h"
#include <vector>
//#include <qstring.h>
#include <qstandarditemmodel.h>
#include <string.h>
using std::string;
class CtpMdSpi : public QObject,public CThostFtdcMdSpi
{
	Q_OBJECT
public:
	CtpMdSpi(CThostFtdcMdApi* api,	QStandardItemModel * quote_model):m_pUserApi_md(api)
	{
		m_charNewIdList_all = NULL;

		m_charNewIdList_holding_md = NULL;
		this->m_quote_model = quote_model;
		
	};
	///错误应答
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



public:
	
	//登录期货账户
	void ReqUserLogin(TThostFtdcBrokerIDType	appId,
		TThostFtdcUserIDType	userId,	TThostFtdcPasswordType	passwd);

	//订阅行情
	void SubscribeMarketData(char* instIdList);

	//保存全市场合约，在TD进行
	void set_instIdList_all(string instIdList_all);

	//订阅全市场行情，有需要时才调用，要先在TD调用set_instIdList_all()函数，保存全市场的合约。
	//交易时只需订阅所需的合约即可
	void SubscribeMarketData_all(); 

	//错误响应
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

	//账号设置
	void setAccount(TThostFtdcBrokerIDType	appId1,	TThostFtdcUserIDType	userId1,	TThostFtdcPasswordType	passwd1);

	//设置要订阅行情的合约代码
	void setInstId(string instId);

	//设置程序启动前的留仓，需要订阅行情的合约
	void setInstIdList_holding_md(string instId);
		
	void setModelData(CThostFtdcDepthMarketDataField *pDethMarket);
	
	void printData(CThostFtdcDepthMarketDataField *pDepthMarketData);
signals:
	void dataChange(const QModelIndex&,const QModelIndex&);
	void disconnection ();
private:

	CThostFtdcMdApi* m_pUserApi_md;//行情API指针

	char* m_charNewIdList_all;//全市场合约

	char* m_charNewIdList_holding_md;//程序启动前的留仓，需要订阅行情的合约

	TThostFtdcInstrumentIDType m_instId;//要订阅行情的合约代码，即策略要交易的合约
	
	TThostFtdcBrokerIDType	m_appId;//经纪公司代码
	TThostFtdcUserIDType	m_userId;//用户名
	char	m_passwd[252];//密码

	QStandardItemModel * m_quote_model;
	QMutex m_mutex;
	
};

typedef int (*ccbf_secureApi_LoginMd)(CThostFtdcMdApi* ctp_futures_pMdApi, TThostFtdcBrokerIDType brokeId, TThostFtdcUserIDType userId, char* pChar_passwd, int& ctp_futures_requestId);

#endif