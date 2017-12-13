#pragma once


#include "ThostFtdcUserApiStruct.h"
#include <string>
#include <iostream>
#include <direct.h>
#include "StructFunction.h"
#include "mdspi.h"
#include "traderspi.h"

class ctp{
public:
	ctp( );

	bool startUp();
	
	bool checkRunningTime();

	bool cutOut();  

	CtpTraderSpi* getCtpTraderSpi();

	void loadConf();

	//初始化
	void init();
	//返回配置参数
	AccountParam* getAccountParam();
	

private:
		CThostFtdcMdApi *pUserApi_md; // 行情接口
	
		CtpMdSpi  *pUserSpi_md;			// 行情重构

		CThostFtdcTraderApi *pUserApi_trade;	// 交易接口

		CtpTraderSpi *pUserSpi_trade;			// 交易重构

		AccountParam m_accountParam;		// 参数结构
};