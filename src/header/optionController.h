#pragma once



#include "ThostFtdcMdApi.h"
#include <vector>
#include "StructFunction.h"
#include <string>
#include <iostream>
#include <map>
#include "traderspi.h"

//#include "LibXL\include\libxl.h"
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


	// 剩余时间
	bool valideDate(time_t _settlementDate,time_t _todayDate,time_t _maturityDate);
	void getStandAnnualTime(time_t _todayDate,double &elapsedTime,double &remainTime);

	// 数学算法
	//double normal_pdf(double var,double mean,double sigma);
	//double normal_cdf(double x,double step);
	//double normdist(double x, double mean, double standard_dev);
	//greeks
	//double get_greeks(const char _optionType,const char _valueType,const  double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime);
	// 加载配置
	void loadConf(QString);
	//每日结算
	void everDaySettle(CThostFtdcDepthMarketDataField *closePrice,time_t remainTime,time_t carryRemainTime,optionHedgeHands lastHands);
	//设置合约-合约信息结构体的map
	void set_instMessage_map_stgy(map<string, CThostFtdcInstrumentField*>& instMessage_map_stgy);
	//设置交易的合约代码
	void setInstId(string instId);
	//设置投资者代码
	void setUserId(string instId);
	//返回手数
	double getHands(int scaleUnit,double scale);
	//载入成交数据
	bool loadHistoryTrade(string date,string s_userId);
	//计算当期 该账户下 该合约下 期货盈亏
	bool calculateFuturePnl(string date,double yesterdayClose,double closePrice,optionHedgeHands lastHedgeHands,double& intradeDay,double& holdPosition,double& newPosition);
	
	~optionController(void);



private:
	map<string, CThostFtdcInstrumentField*> m_instMessage_map_stgy;//保存合约信息的map,通过set_instMessage_map_stgy()函数从TD复制
	vector<CThostFtdcTradeField> m_instTradeList;
	//投资者代码
	string m_userId;

	TThostFtdcInstrumentIDType m_instId;//合约代码
	// 期权名称
	string m_sOptionName;
	//期权代码
	string m_sOptionCode;
	//期初价格
	double m_dInitialPrice;
	// 期权种类
	optionKind m_iOptionKind;
	// 规模
	int m_iCallScale;
	// 看跌
	int m_iPutScale;
	//期权发行期
	time_t m_tSettlementDate;
	string m_sSettlementDate;
	//期权到期日
	time_t m_tMaturityDate;
	string m_sMaturityDate;
	//无风险利率
	double m_dInterest;
	//红利率 股息
	double m_dDividend;
	//对冲波动率
	double m_dVolability;
	//定价波动率
	double m_priceVolability;
	//执行价格百分比
	double m_dCallExecPricePercent;
	double m_dPutExecPricePercent;
	//执行价格
	double m_dCallExecPrice;
	double m_dPutExecPrice;
	// ==========亚式==========
	//亚式均价
	double m_dAveragePrice;

	// ==========障碍==========
	//障碍值
	double m_dBarrierValue;

	// 每点价值
	int m_iTickValue;

	//规模单位

	int m_iScaleUnit;

	//最小报价单位
	double  m_priceTick;
	//年华剩余时间
	double m_reaminTime;
	//年华消耗时间
	double m_elaspedTme;
	//对冲方向
	int m_iCallHedgeDirection;
	int m_iPutHedgeDirection;
	//上一个交易日
	string m_yesterdayDate;
	//资金量
	double m_accountMargin;
	//手续费
	double m_commission;

	//第一天
	bool m_firstDay;
	optionModel::optionEngine m_optionEngine;

     //libxl::Book* m_book;//book 就代表内存中的一个xlsx实例
	
};
