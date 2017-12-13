//交易所代码：SHFE/DCE/CZCE/CFFEX(上期所/大连/郑州/中金所)

/**********************************************************************

项目名称: OTC场外期权
修改时间:2015/11/16
开发：李勇

**********************************************************************/

#include <afx.h>
#include <fstream>
#include <atomic>



#define ELPP_THREAD_SAFE
#include "easylogging++.h"


#include "ctp.h"
#include "mdspi.h"
#include "traderspi.h"
#include "strategy.h"
#include "Mysql.h"



ctp::ctp(){
	// do something
	memset(&this->m_accountParam, 0, sizeof(AccountParam));
}


void ctp::init( ){


	//--------------初始化行情UserApi，创建行情API实例----------------------------------
	this->pUserApi_md = CThostFtdcMdApi::CreateFtdcMdApi(".\\MDflow\\");
	this->pUserSpi_md = new CtpMdSpi(pUserApi_md);//创建回调处理类对象MdSpi
	pUserApi_md->RegisterSpi(pUserSpi_md);// 回调对象注入接口类
	pUserApi_md->RegisterFront(this->m_accountParam.m_mdFront);// 注册行情前置地址	

	pUserSpi_md->setAccount(this->m_accountParam.m_appId, this->m_accountParam.m_userId, this->m_accountParam.m_passwd);//经纪公司编号，用户名，密码
	pUserSpi_md->setInstId(this->m_accountParam.m_read_contract);//MD所需订阅行情的合约，即策略交易的合约

	//--------------初始化交易UserApi，创建交易API实例----------------------------------
	this->pUserApi_trade = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\TDflow\\");

	this->pUserSpi_trade = new CtpTraderSpi(pUserApi_trade, pUserApi_md, pUserSpi_md);//构造函数初始化三个变量
	pUserApi_trade->RegisterSpi((CThostFtdcTraderSpi*)pUserSpi_trade);// 注册事件类
	pUserApi_trade->SubscribePublicTopic(THOST_TERT_RESTART);// 注册公有流
	pUserApi_trade->SubscribePrivateTopic(THOST_TERT_QUICK);// 注册私有流THOST_TERT_QUICK
	pUserApi_trade->RegisterFront(this->m_accountParam.m_tradeFront);// 注册交易前置地址

	pUserSpi_trade->setAccount(this->m_accountParam.m_appId, this->m_accountParam.m_userId, this->m_accountParam.m_passwd);//经纪公司编号，用户名，密码
	pUserSpi_trade->setInstId(this->m_accountParam.m_read_contract);//策略交易的合约

}

bool ctp::checkRunningTime(){

	int minute_times = 0;
	bool mark = false;
	cout << "please input waiting times(minutes like: 30 ): " << endl;
	cin>>minute_times;
	if(minute_times == 0){
		mark = true;
		return mark;
	}
	for(int i = minute_times-1;i>=0;i--){
		for(int j = 60; j > 0;j--){
			cout << "Time Left   " << i << " m : " << j  << " s  "<< endl;
			Sleep(1000);
		}
	};
	return mark;
}



bool ctp::startUp(){

	this->pUserApi_trade->Init(); // 启动行情
	
	return true;
}

CtpTraderSpi *ctp::getCtpTraderSpi(){

	return this->pUserSpi_trade; // 返回交易接口

}
AccountParam* ctp::getAccountParam(){
	return &(this->m_accountParam);
}
bool ctp::cutOut(){
	pUserApi_md->Join();//等待接口线程退出
	pUserApi_trade->Join();

	//delete this->pUserSpi_md;
	//delete this->pUserSpi_trade;
	return true;
}
//--------------读取配置文件，获取账户信息、服务器地址、交易的合约代码--------------
void ctp::loadConf(){
	
	//-------------------------------读取账号模块-------------------------------
	CString read_brokerID;
	GetPrivateProfileString("Account","brokerID","brokerID_error",read_brokerID.GetBuffer(MAX_PATH),MAX_PATH,"./input/AccountParam.ini");
	
	CString read_userId;
	GetPrivateProfileString("Account","userId","userId_error",read_userId.GetBuffer(MAX_PATH),MAX_PATH,"./input/AccountParam.ini");

	CString read_passwd;
	GetPrivateProfileString("Account","passwd","passwd_error",read_passwd.GetBuffer(MAX_PATH),MAX_PATH,"./input/AccountParam.ini");

	strcpy_s(this->m_accountParam.m_appId, read_brokerID);
	strcpy_s(this->m_accountParam.m_userId, read_userId);
	strcpy_s(this->m_accountParam.m_passwd, read_passwd);


	//-------------------------------读取地址模块-------------------------------
	CString read_MDAddress;
	GetPrivateProfileString("FrontAddress","MDAddress","MDAddress_error",read_MDAddress.GetBuffer(MAX_PATH),MAX_PATH,"./input/AccountParam.ini");
	
	CString read_TDAddress;
	GetPrivateProfileString("FrontAddress","TDAddress","TDAddress_error",read_TDAddress.GetBuffer(MAX_PATH),MAX_PATH,"./input/AccountParam.ini");
		
	strcpy_s(this->m_accountParam.m_mdFront, read_MDAddress);
	strcpy_s(this->m_accountParam.m_tradeFront, read_TDAddress);
	

	//-------------------------------设置合约模块-------------------------------
	CString read_contract;
	GetPrivateProfileString("Contract","contract","contract_error",read_contract.GetBuffer(MAX_PATH),MAX_PATH,"./input/AccountParam.ini");
	//CString test(_T(read_contract));
	//CString test1(_T("asd"));
	//CString DirContract =test + _T("output//3213") ;
	this->m_accountParam.m_read_contract = (LPCTSTR)read_contract;
	//CString DirC = _T("output//");
	
	//_mkdir(TEXT(DirContract));
	
}