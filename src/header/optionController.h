#pragma once



#include "ThostFtdcMdApi.h"
#include <vector>
#include "StructFunction.h"
#include <string>
#include <iostream>
#include <map>
#include "traderspi.h"

#include "LibXL\include\libxl.h"
#include "option.h"
using optionModel::Greeks;
using optionModel::OptionType;
class optionController
{
public:
	optionController();
	void setData(int iOptionKind,string sSettlementDate,string sMaturityDate,double iCallScale,double iPutScale,double sInterest,double sDividend,double sVolability,double sAveragePrice,double sBarrierValue,double sExecPricePercent,double sInitialPrice,int iTickValue);

	bool optionCalculate(double underlyingPrice,const time_t _curDateTime,optionGreeks &_CallgreeksValue,optionGreeks &_putGreeksValue,optionHedgeHands &_hedgeHands,optionParam &_optionParam);

	/*double EuropeanCalculate	(const char _optionType,const char _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime);
	double AsianCalculate		(const char _optionType,const char _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime);
	double AmericanCalculate	(const char _optionType,const char _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime);
	double BarrierCalculate		(const char _optionType,const char _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime){return 0;}
	double LookBacCalculate		(const char _optionType,const char _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime){return 0;}*/


	// ʣ��ʱ��
	bool valideDate(time_t _settlementDate,time_t _todayDate,time_t _maturityDate);
	void getStandAnnualTime(time_t _todayDate,double &elapsedTime,double &remainTime);

	// ��ѧ�㷨
	//double normal_pdf(double var,double mean,double sigma);
	//double normal_cdf(double x,double step);
	//double normdist(double x, double mean, double standard_dev);
	//greeks
	//double get_greeks(const char _optionType,const char _valueType,const  double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime);
	// ��������
	void loadConf(QString);
	//ÿ�ս���
	void everDaySettle(CThostFtdcDepthMarketDataField *closePrice,time_t remainTime,time_t carryRemainTime,optionHedgeHands lastHands);
	//���ú�Լ-��Լ��Ϣ�ṹ���map
	void set_instMessage_map_stgy(map<string, CThostFtdcInstrumentField*>& instMessage_map_stgy);
	//���ý��׵ĺ�Լ����
	void setInstId(string instId);
	//����Ͷ���ߴ���
	void setUserId(string instId);
	//��������
	double getHands(int scaleUnit,double scale);
	//����ɽ�����
	bool loadHistoryTrade(string date,string s_userId);
	//���㵱�� ���˻��� �ú�Լ�� �ڻ�ӯ��
	bool calculateFuturePnl(string date,double yesterdayClose,double closePrice,optionHedgeHands lastHedgeHands,double& intradeDay,double& holdPosition,double& newPosition);
	
	~optionController(void);



private:
	map<string, CThostFtdcInstrumentField*> m_instMessage_map_stgy;//�����Լ��Ϣ��map,ͨ��set_instMessage_map_stgy()������TD����
	vector<CThostFtdcTradeField> m_instTradeList;
	//Ͷ���ߴ���
	string m_userId;

	TThostFtdcInstrumentIDType m_instId;//��Լ����
	// ��Ȩ����
	string m_sOptionName;
	//��Ȩ����
	string m_sOptionCode;
	//�ڳ��۸�
	double m_dInitialPrice;
	// ��Ȩ����
	optionKind m_iOptionKind;
	// ��ģ
	int m_iCallScale;
	// ����
	int m_iPutScale;
	//��Ȩ������
	time_t m_tSettlementDate;
	string m_sSettlementDate;
	//��Ȩ������
	time_t m_tMaturityDate;
	string m_sMaturityDate;
	//�޷�������
	double m_dInterest;
	//������ ��Ϣ
	double m_dDividend;
	//�Գ岨����
	double m_dVolability;
	//���۲�����
	double m_priceVolability;
	//ִ�м۸�ٷֱ�
	double m_dCallExecPricePercent;
	double m_dPutExecPricePercent;
	//ִ�м۸�
	double m_dCallExecPrice;
	double m_dPutExecPrice;
	// ==========��ʽ==========
	//��ʽ����
	double m_dAveragePrice;

	// ==========�ϰ�==========
	//�ϰ�ֵ
	double m_dBarrierValue;

	// ÿ���ֵ
	int m_iTickValue;

	//��ģ��λ

	int m_iScaleUnit;

	//��С���۵�λ
	double  m_priceTick;
	//�껪ʣ��ʱ��
	double m_reaminTime;
	//�껪����ʱ��
	double m_elaspedTme;
	//�Գ巽��
	int m_iCallHedgeDirection;
	int m_iPutHedgeDirection;
	//��һ��������
	string m_yesterdayDate;
	//�ʽ���
	double m_accountMargin;
	//������
	double m_commission;

	//��һ��
	bool m_firstDay;
	optionModel::optionEngine m_optionEngine;

	 libxl::Book* m_book;//book �ʹ����ڴ��е�һ��xlsxʵ��
	
};
