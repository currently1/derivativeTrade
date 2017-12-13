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
		//Ĭ��ʹ�ÿ��Ʊ������� Ĭ�ϲ���Ϊ1000
		double americanBinomial (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanBTree (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanBTreeDelta (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanBTreeGamma(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanDigitalImmediate(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double americanDigitalAtExpiry(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		//Binary Options ע��Բ��������� Ӧ��֤ remainTime ������Χ��
		double cashOrNothing(OptionType _o,Greeks _v,const double _strike,const double __riskFreeRate,const double _payAmountK,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double assetOrNothing(OptionType _o,Greeks _v,const double _strike,const double _rishFreeRate,const double _dividendYield,const double _underlyingPrice ,const double _vol, const double _elapsedTime,const double _remainTime);
		//Barier Option
		double BarrierStaticReplication(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		double BarrierPrice(OptionType _optionType,Greeks _valueType,const double _strike,const double __riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime);
		//Asian Option
		void setParameterAsian(double _SA,int n,int m,double _t1){this->m_dAveragePrice = _SA;this->_n = n;this->_m = m; this->_t1 = _t1;}
		// ��ѧ�㷨
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
		// ����
		underlyingType m_underlyingType;
		//�ڳ��۸�
		double m_dInitialPrice;
		// ��ģ
		int m_iCallScale;
		// ����
		int m_iPutScale;
		//�޷�������
		double m_dInterest;
		//������ ��Ϣ
		double m_dDividend;
		//�Գ岨����
		double m_dVolability;
		//���۲�����
		double m_priceVolability;
		//ִ�м۸�ٷֱ�
		double m_dExecPricePercent;
		//ִ�м۸�
		double m_dExecPrice;
		// ==========��ʽ==========
		//��ʽ����
		double m_dAveragePrice;
		int _n;
		int _m;
		int _t1;
		// ==========�ϰ�==========
		//�ϰ�ֵ
		double m_dBarrierValue;
		double m_dBarrierRebate;//�ϰ��⸶
		PayoutStyle m_rebateStyle;
		bool m_bTouchBarrierTag;
		//===========��ʽ����==============
		bool m_bOneTouchAtExpiryTag;
		bool m_bOneTouchImmediateTag;
		
		// ÿ���ֵ
		int m_iTickValue;
		//bump
		double m_dBump;
		double m_gBump;
		double m_vBump;
		double m_tBump;
		double m_rBump;
		//��ģ��λ

		int m_iScaleUnit;

		//��С���۵�λ
		double  m_priceTick;
		//�껪ʣ��ʱ��
		double m_reaminTime;
		//�껪����ʱ��
		double m_elaspedTme;
		//�Գ巽��
		int m_iHedgeDirection;

		//�ʽ���
		double m_accountMargin;
		//������
		double m_commission;

		//��һ��
		bool m_firstDay;
		//digital payoff amount K�⸶
		double m_payoffAmount;
		//��ʽstep
		int m_step;
	};
};