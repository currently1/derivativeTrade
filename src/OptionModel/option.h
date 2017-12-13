#pragma once
//#ifdef  DLL_API
//#define DLL_API_CLASS __declspec(dllexport)  
//#else
//#define DLL_API_CLASS _declspec(dllimport)
//#endif

#include <vector>
#include <string>
#include <iostream>
#include <map>
using namespace std;
namespace optionModel{
	enum OptionType {CALL,PUT,CallUpIn,CallUpOut,CallDownIn,CallDownOut,PutUpIn,PutUpOut,PutDownIn,PutDownOut};
	enum Greeks {PRICE,DELTA,GAMMA,VEGA,THETA,RHO};
	enum PayoutStyle{AtExpiry,Immediate};
	enum underlyingType{Stock,Future};
	//class DLL_API_CLASS  optionEngine
	class optionEngine
	{
	public:
		 optionEngine();
	//	void convertTime(cDate settlementDate,cDate nowDate,cDate maturityDate,double &_elapsedTime, double &_remianTime);  
		//double BlackCalculate(OptionType _optionType, Greeks _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remainTime);
		double EuropeanCalculate	(OptionType _optionType, Greeks _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remainTime);
		double AmericanCalculate	(OptionType _optionType, Greeks _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remainTime);
		double BarrierCalculate	(OptionType _optionType, Greeks _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remainTime){return 0;};
		double LookBacCalculate	(OptionType _optionType, Greeks _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remainTime){return 0;};
		//默认使用控制变量法， 默认步长为1000
		double americanBinomial (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanBTree (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanBTreeDelta (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanBTreeGamma(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanDigitalImmediate(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanDigitalAtExpiry(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		//Binary Options 注意对参数的限制 应保证 remainTime 再区域范围内
		double cashOrNothing(OptionType _o,Greeks _v,const double _strike,const double __riskFreeRate,const double _payAmountK,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double assetOrNothing(OptionType _o,Greeks _v,const double _strike,const double _rishFreeRate,const double _dividendYield,const double _underlyingPrice ,const double _vol, const double _elapsedTime,const double _remainTime);
		//Barier Option
		double BarrierStaticReplication(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double BarrierPrice(OptionType _optionType,Greeks _valueType,const double _strike,const double __riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		//Asian Option
		void setParameterAsian(double _SA,int n,int m,double _t1){this->m_dAveragePrice = _SA;this->_n = n;this->_m = m; this->_t1 = _t1;}
		// 数学算法
		double  normal_pdf(double var,double mean,double sigma);
		double  normal_cdf(double x,double step);
		double  normdist(double x, double mean, double standard_dev);
		//greeks
		double  get_greeks( OptionType _optionType, Greeks _valueType,const  double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remianTime);
		void setCashOrNothingPayoff(double payoff){this->m_payoffAmount = payoff;}
		void setBarrierParameters(double Barrier,double rebate,PayoutStyle rebateStyle){this->m_dBarrierValue = Barrier,this->m_dBarrierRebate = rebate;this->m_rebateStyle = rebateStyle;}
		void setUnderlyingType(underlyingType ut){this->m_underlyingType = ut;}
		void setEurpeanParameters(underlyingType ut){this->m_underlyingType = ut;}
		void setBarrierTouchTag(bool tag){this->m_bTouchBarrierTag = tag;}
		void setdBump(double bump){this->m_dBump = bump;}
		void setgBump(double bump){this->m_gBump = bump;}
		void setvBump(double bump){this->m_vBump = bump;}
		void settBump(double bump){this->m_tBump = bump;}
		void setrBump(double bump){this->m_rBump = bump;}
		void setBTreeStep(int step){this->m_step = step;}
		void testHelloWord();
		double (optionEngine::*m_pOptionCalculate)(OptionType _optionType,Greeks _valueType, double _strike,double _riskFreeRate,double _dividendYield,double _underlyingPrice,double _vol,const double _elapsedTime,const double _remianTime);
		~optionEngine(void);

	private:
		// 基础
		underlyingType m_underlyingType;
		//期初价格
		double m_dInitialPrice;
		// 规模
		int m_iCallScale;
		// 看跌
		int m_iPutScale;
		//无风险利率
		double m_dInterest;
		//红利率 股息
		double m_dDividend;
		//对冲波动率
		double m_dVolability;
		//定价波动率
		double m_priceVolability;
		//执行价格百分比
		double m_dExecPricePercent;
		//执行价格
		double m_dExecPrice;
		// ==========亚式==========
		//亚式均价
		double m_dAveragePrice;
		int _n;
		int _m;
		int _t1;
		// ==========障碍==========
		//障碍值
		double m_dBarrierValue;
		double m_dBarrierRebate;//障碍赔付
		PayoutStyle m_rebateStyle;
		bool m_bTouchBarrierTag;
		//===========美式数字==============
		bool m_bOneTouchAtExpiryTag;
		bool m_bOneTouchImmediateTag;
		
		// 每点价值
		int m_iTickValue;
		//bump
		double m_dBump;
		double m_gBump;
		double m_vBump;
		double m_tBump;
		double m_rBump;
		//规模单位

		int m_iScaleUnit;

		//最小报价单位
		double  m_priceTick;
		//年华剩余时间
		double m_reaminTime;
		//年华消耗时间
		double m_elaspedTme;
		//对冲方向
		int m_iHedgeDirection;

		//资金量
		double m_accountMargin;
		//手续费
		double m_commission;

		//第一天
		bool m_firstDay;
		//digital payoff amount K赔付
		double m_payoffAmount;
		//美式step
		int m_step;
	};
};