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

	//��ʼ��
	void init();
	//�������ò���
	AccountParam* getAccountParam();
	

private:
		CThostFtdcMdApi *pUserApi_md; // ����ӿ�
	
		CtpMdSpi  *pUserSpi_md;			// �����ع�

		CThostFtdcTraderApi *pUserApi_trade;	// ���׽ӿ�

		CtpTraderSpi *pUserSpi_trade;			// �����ع�

		AccountParam m_accountParam;		// �����ṹ
};