
//#include "stdafx.h"

#include "option.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include <iostream>
#define  ACCURACY 1.0e-6
//#define DLL_API   

using namespace optionModel;
using std::vector;
using std::max;
optionEngine::optionEngine(void)
{
	m_pOptionCalculate = NULL;
	this->m_priceTick = 1; // default

	this->m_firstDay =false;
	this->m_payoffAmount = 0;
	this->m_underlyingType = optionModel::underlyingType::Future;
	this->m_dBarrierValue = 0;
	this->m_rebateStyle = optionModel::PayoutStyle::AtExpiry;
	m_bTouchBarrierTag = false;
	m_bOneTouchImmediateTag = false;
	m_bOneTouchAtExpiryTag = false;
	this->m_dBump = 0.005L;
	this->m_gBump = 0.0005L;
	this->m_tBump = 0.0005L;
	this->m_vBump = 0.001L;
	this->m_rBump = 0.0001L;
	this->m_step = 700;
}


optionEngine::~optionEngine(void)
{
	//delete m_instTradeList;
	   //book->save("modify.xlsx");//保存文件

}




double optionEngine::EuropeanCalculate(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remainTime){
	double elapsedTime = 0.0L, remainTime = 0.0L;
	double resultValue = 0.0L ,greeks = 0.0L;
	double d1 = 0.0L,d2 = 0.0L;
	elapsedTime = _elapsedTime;
	remainTime = _remainTime;
	if(this->m_underlyingType == underlyingType::Future){
		d1 = (std::log(_underlyingPrice/  _strike) + (_vol* _vol *0.5) * remainTime) / (_vol * std::sqrt(remainTime));
		d2 = d1 - _vol * std::sqrt(remainTime);
	}else{
		d1 = (std::log(_underlyingPrice/  _strike) + (_riskFreeRate - _dividendYield + _vol* _vol *0.5) * remainTime) / (_vol * std::sqrt(remainTime));
		d2 = d1 - _vol * std::sqrt(remainTime);
	}
	
	if(_valueType != Greeks::PRICE){
		this->m_pOptionCalculate = &optionEngine::EuropeanCalculate;
		greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		return greeks;
	}


	//求期权值=BlackScholesCalculate("call","d",6064,H2,I2,A2,J2,K2)
	if(_optionType == OptionType::CALL){
		switch (m_underlyingType)
		{
		case optionModel::Stock:
				resultValue = _underlyingPrice *  std::exp(-_dividendYield * remainTime) *normdist(d1,0,1) - _strike * std::exp(-_riskFreeRate * remainTime) * normdist(d2,0,1);
			break;
		case optionModel::Future:
				resultValue = _underlyingPrice *  std::exp(-_riskFreeRate * remainTime) *normdist(d1,0,1) - _strike * std::exp(-_riskFreeRate * remainTime) * normdist(d2,0,1);
			break;
		default:
			break;
		}
		
	}
	if(_optionType == OptionType::PUT){
		switch (m_underlyingType)
		{
		case optionModel::Stock:
				resultValue = _strike * std::exp(-_riskFreeRate * remainTime) * normdist(-d2,0,1) -  _underlyingPrice*  std::exp(-_dividendYield * remainTime) *normdist(-d1,0,1) ;
			break;
		case optionModel::Future:
				resultValue = _strike * std::exp(-_riskFreeRate * remainTime) * normdist(-d2,0,1) -  _underlyingPrice*  std::exp(-_riskFreeRate * remainTime) *normdist(-d1,0,1) ;
			break;
		default:
			break;
		}
		
	}

	return resultValue;

}
double optionEngine::AmericanCalculate(OptionType _optionType,Greeks _valueType, double _strike,double _riskFreeRate,double _dividendYield,double _underlyingPrice,double _vol,const double _elapsedTime,const double _remainTime){

		double elapsedTime = _elapsedTime, remainTime= _remainTime;
		double resultValue = 0,greeks = 0;
		double S_seed = 0, b = 0, sigma_sqr = _vol*_vol,time_sqrt = 0,nn = 0; 
		double m =0, K =0, q1 = 0,q2 = 0, a1 = 0,a2 = 0,q_inf1= 0,q_inf2=0;
		double q_inf = 0,S_star_inf = 0;// seed value from paper
		double h1 = 0,h2=0; // iterate on S to find S_star, using Newton steps
		double Si=0, g=1, gprime=1.0,d1 = 0,option_bsm;
		double numerator = 0,denominator = 0; // 分子 分母
		double rhs,d_rhs,lhs;
		double hs,vs,d_hs;
		double S_star = 0,A = 0;

		int no_iterations = 0;
		if(this->m_underlyingType == optionModel::underlyingType::Future){
			b = 1e-10;// cost of  carry for future is zero
		}
		if(this->m_underlyingType == optionModel::underlyingType::Stock	){
			b = _riskFreeRate - _dividendYield;// for stock b = r - q
		}

		if(_valueType != Greeks::PRICE){
			this->m_pOptionCalculate = &optionEngine::AmericanCalculate;
			greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
			return greeks;
		}

		if(b>=_riskFreeRate){
			return  this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
		}
		nn = 2.0 * b /sigma_sqr;
		m = 2.0 * _riskFreeRate/sigma_sqr;
		K =1.0-std::exp(-_riskFreeRate*remainTime);
		time_sqrt = std::sqrt(remainTime);
		if(_optionType == OptionType::CALL){
			q2 = (-(nn-1)+ sqrt(std::pow((nn-1),2.0)+(4*m/K)))*0.5;
			q_inf2 = 0.5 * ( -(nn-1.0) +  std::sqrt(std::pow((nn-1),2.0)+4.0*m));

			S_star_inf = _strike / (1.0 - 1.0/q_inf2);
			h2 = -1*( b*remainTime+2.0*_vol*time_sqrt)*(_strike/(S_star_inf-_strike));
			S_seed = _strike + (S_star_inf-_strike)*(1.0-std::exp(h2));
			// newTon 
			while ((fabs(g) > ACCURACY) 

				&& ( no_iterations++<500)

				&& (Si>0.0)) 
				{

					option_bsm  = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,Si,_vol,_elapsedTime,_remainTime);

					d1 = (log(Si/_strike)+(b+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

					lhs = Si - _strike;

					rhs = option_bsm + ( 1 - std::exp((b-_riskFreeRate)*_remainTime) * this->normdist(d1,0,1)) * Si  / q2;

					d_rhs = std::exp((b-_riskFreeRate) *remainTime) * this->normdist(d1,0,1) * (1 - 1/q2) +
						( 1 - std::exp((b-_riskFreeRate) * _remainTime) *this->normal_pdf(d1,0,1) / ( _vol * time_sqrt)) / q2;

					numerator  = _strike + rhs  - d_rhs*Si;

					denominator = 1 - d_rhs;

					Si = numerator / denominator;

					g = std::abs(lhs - rhs) / _strike; 
				};
				//while ((fabs(g) > ACCURACY) 

				//   && (fabs(gprime)>ACCURACY) // to avoid exploding Newton's  

				//   && ( no_iterations++<500)

				//   && (Si>0.0)) {

				//	option_bsm  = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,Si,_vol,_elapsedTime,_remainTime);

				//	d1 = (log(Si/_strike)+(b+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

				//	g =   (Si - _strike) - option_bsm -   ( 1 - std::exp((b -_riskFreeRate ) *remainTime)*  normdist(d1,0,1)) * Si / q2;

				//	//g=(1.0-1.0/q)*Si-_strike-option_bsm+(1.0/q)*Si*exp((_dividendYield-_riskFreeRate)*remainTime)* normal_cdf(omega*d1,0.001);

				//	gprime=( 1.0-1.0/q2)*(1.0-exp((b-_riskFreeRate)*remainTime)*normdist( d1,0,1)) +(1.0/q2)*exp((b-_riskFreeRate)*remainTime)*normdist( d1,0.0f,1.0f)*(1.0/(_vol*time_sqrt));

				//	Si=Si-(g/gprime); 
				//};

				if (fabs(g)>ACCURACY) { S_star = S_seed; } // did not converge
				else { S_star = Si; };

				d1 = (log(S_star/_strike)+(b+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

				A =   (1.0-exp((b-_riskFreeRate)*remainTime)*normdist( d1,0,1))* (S_star/q2); 

		}
		if(_optionType == OptionType::PUT){
			q1 = (-(nn-1)- sqrt(std::pow((nn-1),2.0)+(4*m/K)))*0.5;
			q_inf1 = 0.5 * ( -(nn-1.0) -  std::sqrt(std::pow((nn-1),2.0)+4.0*m));

			S_star_inf = _strike / (1.0 - 1.0/q_inf1);
			h1 = ( b*remainTime-2.0*_vol*time_sqrt)*(_strike/(_strike - S_star_inf));
			S_seed =  S_star_inf + (_strike - S_star_inf)*(std::exp(h1));

			// newTon 
			while ((fabs(g) > ACCURACY) 

				&& ( no_iterations++<500)

				&& (Si>0.0)) 
				{

					option_bsm  = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,Si,_vol,_elapsedTime,_remainTime);

					d1 = (log(Si/_strike)+(b+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

					hs = option_bsm - ( 1 - std::exp((b-_riskFreeRate)*_remainTime) * this->normdist(-d1,0,1)) * Si  / q1;

					vs = _strike - Si;

					d_hs = -1 * std::exp((b-_riskFreeRate) *remainTime) * this->normdist(-d1,0,1) * (1 - 1/q1) -
						( 1 + std::exp((b-_riskFreeRate) * _remainTime) *this->normal_pdf(-d1,0,1) / ( _vol * time_sqrt)) / q1;

					numerator  = _strike - hs  + d_hs*Si;

					denominator = 1 + d_rhs;

					Si = numerator / denominator;

					g = std::abs(hs - vs) / _strike; 
				};		


				//while ((fabs(g) > ACCURACY) 

				//   && (fabs(gprime)>ACCURACY) // to avoid exploding Newton's  

				//   && ( no_iterations++<500)

				//   && (Si>0.0)) {

				//	option_bsm  = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,Si,_vol,_elapsedTime,_remainTime);

				//	d1 = (log(Si/_strike)+(b+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

				//	g = -1 * (Si - _strike) - option_bsm  +  ( 1 - std::exp((b -_riskFreeRate ) *remainTime)*  normdist(-d1,0,1)) * Si / q1;

				//	//g=(1.0-1.0/q)*Si-_strike-option_bsm+(1.0/q)*Si*exp((_dividendYield-_riskFreeRate)*remainTime)* normal_cdf(omega*d1,0.001);

				//	gprime=-1 *( 1.0-1.0/q1)*(1.0-exp((b-_riskFreeRate)*remainTime)*normdist(-1 * d1,0,1)) +(1.0/q1)*exp((b-_riskFreeRate)*remainTime)*normdist(-1 * d1,0.0f,1.0f)*(1.0/(_vol*time_sqrt));

				//	Si=Si-(g/gprime); 
				//};

				if (fabs(g)>ACCURACY) { S_star = S_seed; } // did not converge
				else { S_star = Si; };

				d1 = (log(S_star/_strike)+(b+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

				A =   -1 * (1.0-exp((b-_riskFreeRate)*remainTime)*normdist(-d1,0,1))* (S_star/q1); 

		}


		double e  = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
		if(_optionType == OptionType::CALL){
			if (_underlyingPrice>=S_star) {resultValue=_underlyingPrice-_strike;} 
			else {
				resultValue=e+A*pow((_underlyingPrice/S_star),q2);
			};
		}else{
			if (_underlyingPrice<=S_star) {resultValue=_strike-_underlyingPrice;} 
			else {
				resultValue=e+A*pow((_underlyingPrice/S_star),q1);
			};
			
		}
		return max(resultValue,e); // know value will never be less than BS value
}


double optionEngine::cashOrNothing(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	double value = 0;
	double d = 0,greeks = 0;;
	if(this->m_underlyingType == optionModel::underlyingType::Future){
		d = (std::log(_underlyingPrice/_strike) + ( - _vol * _vol * 0.5) * _remainTime) / (_vol * std::sqrt(_remainTime));
	}
	if(this->m_underlyingType == optionModel::underlyingType::Stock){
		d = (std::log(_underlyingPrice/_strike) + (_dividendYield - _vol * _vol * 0.5) * _remainTime) / (_vol * std::sqrt(_remainTime));
	}
	
	if(_valueType != Greeks::PRICE){
		this->m_pOptionCalculate = &optionEngine::cashOrNothing;
		greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		return greeks;

	}
	if(_optionType == OptionType::CALL){
		value = this->m_payoffAmount * std::exp(-_riskFreeRate * _remainTime ) * this->normdist(d,0,1);
	}
	else if(_optionType == OptionType::PUT){
		value = this->m_payoffAmount * std::exp(-_riskFreeRate * _remainTime ) * this->normdist(-d,0,1);
	}
	
	return value;
}
double optionEngine::assetOrNothing(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice ,const double _vol, const double _elapsedTime,const double _remainTime){
	double value = 0;
	double d = 0,greeks = 0;;
	d = (std::log(_underlyingPrice/_strike) + (_dividendYield + _vol * _vol * 0.5) * _remainTime) / (_vol * std::sqrt(_remainTime));
	if(_valueType != Greeks::PRICE){
		this->m_pOptionCalculate = &optionEngine::assetOrNothing;
		greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		return greeks;

	}
	if(_optionType == OptionType::CALL){
		value = _underlyingPrice * std::exp(_dividendYield -_riskFreeRate * _remainTime ) * this->normdist(d,0,1);
	}
	else if(_optionType == OptionType::PUT){
		value = _underlyingPrice * std::exp(_dividendYield -_riskFreeRate * _remainTime ) * this->normdist(-d,0,1);
	}
	
	return value;
}

double optionEngine::americanDigitalImmediate(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	double greeks = 0, lamudaPositive = 0 , lamudaNegative = 0,k = 0,k_ = 0,alpha = 0 , beta = 0;
	double dPositive,dNegative;
	double value;

	if(_valueType != Greeks::PRICE){
		this->m_pOptionCalculate = &optionEngine::americanDigitalImmediate;
		greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		return greeks;

	}
	
	k = _riskFreeRate / (0.5 * _vol * _vol);
	
	if(this->m_underlyingType == optionModel::underlyingType::Stock){
		k_ = (_riskFreeRate - _dividendYield) / (0.5 * _vol * _vol);
	}else{
		k_ = 0;
	}
	 
	alpha = 0.5 * (1- k_);
	beta = -(0.25 * (1 - k_) *  (1 - k_) + k);

	if(_dividendYield != 0){
		lamudaPositive = alpha + std::sqrt(-beta);
		lamudaNegative = alpha - std::sqrt(-beta);
	} else{
		lamudaPositive = 1;
		lamudaNegative = -k;
	}


	dPositive = (std::log(_underlyingPrice / _strike) + _vol * _vol  * std::sqrt(-beta) * _remainTime ) / (_vol * std::sqrt(_remainTime));
	dNegative = (std::log(_underlyingPrice / _strike) - _vol * _vol  * std::sqrt(-beta) * _remainTime ) / (_vol * std::sqrt(_remainTime));
	
	if(_optionType == OptionType::CALL){
		// 或者触发
		if(_underlyingPrice >= _strike || this->m_bOneTouchImmediateTag){
			return 1;
		}else if( _underlyingPrice > 0){
			if( _remainTime == 0){
				return 0;//At expiry
			}
			value = std::pow(_underlyingPrice / _strike,lamudaPositive) * this->normdist(dPositive,0,1) + 
				std::pow(_underlyingPrice / _strike, lamudaNegative) *  this->normdist(dNegative,0,1);
	
		}
	}
	else if(_optionType == OptionType::PUT){
		if(_underlyingPrice <= _strike || this->m_bOneTouchImmediateTag){
			return 1;
		}else{
			if(_remainTime == 0){
				return 0;
			}
				value = std::pow(_underlyingPrice / _strike,lamudaPositive) * this->normdist(-dPositive,0,1) + 
			std::pow(_underlyingPrice / _strike, lamudaNegative) *  this->normdist(-dNegative,0,1);
		}

	}

	return value;
}


double optionEngine::americanDigitalAtExpiry(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	double greeks = 0, lamudaPositive = 0 , lamudaNegative = 0,k = 0,k_ = 0,alpha = 0 , beta = 0;
	double value;

	if(_valueType != Greeks::PRICE){
		this->m_pOptionCalculate = &optionEngine::americanDigitalAtExpiry;
		greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		return greeks;

	}
	k = _riskFreeRate / (0.5 * _vol * _vol);
	
	// for future
	if(this->m_underlyingType == optionModel::underlyingType::Future){
		k_ = 0;
	}
	// for stock
	else{
		k_ = (_riskFreeRate - _dividendYield) / (0.5 * _vol * _vol);
	}


	this->setCashOrNothingPayoff(1.0); 
	alpha = 0.5 * (1- k_);
	beta = -(0.25 * (1 - k_) *  (1 - k_) + k);
	if(_optionType == OptionType::CALL){
		if(_underlyingPrice >= _strike || this->m_bOneTouchAtExpiryTag){
			value = std::exp(-_riskFreeRate * _remainTime);
		}
		else if (_underlyingPrice > 0){
		
			value = this->cashOrNothing(OptionType::CALL,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) +
				std::pow(_underlyingPrice / _strike,2*alpha) * 
				this->cashOrNothing(OptionType::PUT,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_strike * _strike / _underlyingPrice,_vol,_elapsedTime,_remainTime);
		}
	
	}
	else if(_optionType == OptionType::PUT){
		if(_underlyingPrice <= _strike || this->m_bOneTouchAtExpiryTag){
			value = std::exp(-_riskFreeRate * _remainTime);
		}
		else {
			this->setCashOrNothingPayoff(1.0); 
			value = this->cashOrNothing(OptionType::PUT,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) + 
				std::pow(_underlyingPrice / _strike,2*alpha) * 
				this->cashOrNothing(OptionType::CALL,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_strike * _strike / _underlyingPrice,_vol,_elapsedTime,_remainTime);
		}
	

	}
	return value;

}
// 首先设置rebateStyle(AtExpiry Immediate) 然后再设置下 underlyingStyle（stock future）
double optionEngine::BarrierStaticReplication(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){

	
	
	double greeks = 0, lamudaPositive = 0 , lamudaNegative = 0,k = 0,k_ = 0,alpha = 0 , beta = 0;
	double value;
	double barrieStrike =  this->m_dBarrierValue * this->m_dBarrierValue / _strike ; // B^2 / K 
	double barrieMutiple = _strike / this->m_dBarrierValue;
	double rebateValue = 0;


	switch (_optionType)
	{
		case optionModel::CALL:
			break;
		case optionModel::PUT:
			break;
		case optionModel::CallUpIn:
			if(this->m_dBarrierValue > _underlyingPrice && this->m_dBarrierValue > _strike && !this->m_bTouchBarrierTag){

				value = barrieMutiple * this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  +
				(this->m_dBarrierValue - _strike) * this->americanDigitalAtExpiry(OptionType::CALL,Greeks::PRICE,this->m_dBarrierValue,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) ;

				
			}else{
				value = this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
			}
			break;
		case optionModel::CallUpOut:
			if(this->m_dBarrierValue > _underlyingPrice && !this->m_bTouchBarrierTag){
				value = this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  -
				barrieMutiple * this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  -
				(this->m_dBarrierValue - _strike) * this->americanDigitalAtExpiry(OptionType::CALL,Greeks::PRICE,this->m_dBarrierValue,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) ;
			}else{
				value = 0;
			}
			break;
		case optionModel::CallDownIn:
			if(_underlyingPrice>this->m_dBarrierValue && !this->m_bTouchBarrierTag){
				//value = barrieMutiple * this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  +
				//rebateValue;
				value = BarrierStaticReplication( OptionType::PutDownIn,Greeks::PRICE,  _strike,  _riskFreeRate,  _dividendYield,  _underlyingPrice,   _vol,  _elapsedTime,  _remainTime) -
					(_strike - this->m_dBarrierValue) * this->americanDigitalAtExpiry(OptionType::PUT,Greeks::PRICE,this->m_dBarrierValue,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) ;

			}else{
				value = this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) ;
			}

			break;
		case optionModel::CallDownOut:
			if(_underlyingPrice>this->m_dBarrierValue && !this->m_bTouchBarrierTag){
				//value = this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  - 
				//barrieMutiple * this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  +
				//rebateValue;
				value = this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  - 
				 BarrierStaticReplication( OptionType::CallDownIn,Greeks::PRICE,  _strike,  _riskFreeRate,  _dividendYield,  _underlyingPrice,   _vol,  _elapsedTime,  _remainTime);
			}else{
				value = 0;
			}
			break;
		case optionModel::PutUpIn:
			if(this->m_dBarrierValue > _underlyingPrice && !this->m_bTouchBarrierTag){
				//value = barrieMutiple * this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  +
				//rebateValue;
				value = BarrierStaticReplication( OptionType::CallUpIn,Greeks::PRICE,  _strike,  _riskFreeRate,  _dividendYield,  _underlyingPrice,   _vol,  _elapsedTime,  _remainTime)
					-( this->m_dBarrierValue - _strike) * this->americanDigitalAtExpiry(OptionType::CALL,Greeks::PRICE,this->m_dBarrierValue,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
			}else{
				value = this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  ;
			
			}

			break;
		case optionModel::PutUpOut:
			if(this->m_dBarrierValue > _underlyingPrice && !this->m_bTouchBarrierTag){
				//value =  this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  -
				//barrieMutiple * this->EuropeanCalculate(OptionType::CALL,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  +
				//rebateValue;
				value = this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)-
					BarrierStaticReplication( OptionType::PutUpIn,Greeks::PRICE,  _strike,  _riskFreeRate,  _dividendYield,  _underlyingPrice,   _vol,  _elapsedTime,  _remainTime) ;
			}else{
				value = 0;
			}

			break;
		case optionModel::PutDownIn:
			if(_underlyingPrice > this->m_dBarrierValue && this->m_dBarrierValue < _strike && !this->m_bTouchBarrierTag){
				value = barrieMutiple * this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  +
				(_strike - this->m_dBarrierValue) * this->americanDigitalAtExpiry(OptionType::PUT,Greeks::PRICE,this->m_dBarrierValue,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) ;
			}else{
				value = this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) ;
			}

			break;
		case optionModel::PutDownOut:
			if(_underlyingPrice > this->m_dBarrierValue && !this->m_bTouchBarrierTag ){
				value = this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  -
					barrieMutiple * this->EuropeanCalculate(OptionType::PUT,Greeks::PRICE,barrieStrike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime)  -
					(_strike -this->m_dBarrierValue ) * this->americanDigitalAtExpiry(OptionType::PUT,Greeks::PRICE,this->m_dBarrierValue,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime) ;
			}
			else{
				value = 0;
			}
			break;
		default:
			break;
	}
	if(value < 0){
		value = 0;
	}
	return value;
}
double optionEngine::BarrierPrice(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	double rebateValue = 0;
	double greeks = 0,value = 0;	
	OptionType OTType;
		// 保证在barrier处 greeks计算准确性
	if(_underlyingPrice == this->m_dBarrierValue){
		this->m_bTouchBarrierTag = true;
		this->m_bOneTouchAtExpiryTag = true;
		this->m_bOneTouchImmediateTag = true;
	}
	if(_optionType == OptionType::CallDownIn || _optionType == OptionType::CallDownOut || _optionType == OptionType::PutDownOut || _optionType == OptionType::PutDownIn){
		OTType = OptionType::PUT;//putupout putdownin calldownin callupout
	}else{// callupin putupin putdownout calldownout
		OTType = OptionType ::CALL;
	}
	if(_optionType == OptionType::CallDownIn || _optionType == OptionType::CallUpIn || _optionType == OptionType::PutDownIn || _optionType == OptionType::PutUpIn){
		this->m_rebateStyle = PayoutStyle::AtExpiry; // In style only at Expiry 
	}
	if(this->m_rebateStyle == PayoutStyle::AtExpiry){
		if(_optionType == OptionType::CallDownIn || _optionType == OptionType::CallUpIn || _optionType == OptionType::PutUpIn || _optionType == OptionType::PutDownIn){
			rebateValue = this->m_dBarrierRebate * std::exp(-_riskFreeRate * _remainTime)- this->m_dBarrierRebate * this->americanDigitalAtExpiry( OTType, Greeks::PRICE,  this->m_dBarrierValue, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		}
		else{
			rebateValue = this->m_dBarrierRebate * this->americanDigitalAtExpiry( OTType, Greeks::PRICE,  this->m_dBarrierValue, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		}
	}
	if(this->m_rebateStyle == PayoutStyle::Immediate){
		if(_optionType == OptionType::CallDownIn || _optionType == OptionType::CallUpIn || _optionType == OptionType::PutUpIn || _optionType == OptionType::PutDownIn){
			rebateValue = this->m_dBarrierRebate - this->m_dBarrierRebate * this->americanDigitalImmediate( OTType, Greeks::PRICE,  this->m_dBarrierValue, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		}else{
			rebateValue = this->m_dBarrierRebate * this->americanDigitalImmediate( OTType, Greeks::PRICE,  this->m_dBarrierValue, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		}
	}

	if(_valueType != Greeks::PRICE){
		this->m_pOptionCalculate = &optionEngine::BarrierPrice;
		greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);
		return greeks;
	}
	value = BarrierStaticReplication( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime) + rebateValue ;

	return value;
}


double optionEngine::normal_pdf(double var,double mean,double sigma){
	double ret = std::exp(-(var-mean)*(var-mean)/(2.0L*sigma*sigma));
	ret /= sigma*std::sqrt(2.0L*std::_Pi);
	return ret;
}
double optionEngine::normal_cdf(const double x,const double step){
	double ret = 0.0;
	for(double i = -10l;i<x;i+=step){
		ret += step *  1 / sqrt(2 * std::_Pi) * std::exp( -i * i / 2);;
	}
	return ret;
}

double optionEngine::normdist(double x, double mean, double standard_dev)
{
    double res;
    x=(x - mean) / standard_dev;
    if (x == 0)
    {
        res=0.5;
    }
    else
    {
        double oor2pi = 1/(sqrt(double(2) * 3.14159265358979323846));
        double t = 1 / (double(1) + 0.2316419 * fabs(x));
        t *= oor2pi * exp(-0.5 * x * x) 
             * (0.31938153   + t 
             * (-0.356563782 + t
             * (1.781477937  + t 
             * (-1.821255978 + t * 1.330274429))));
        if (x >= 0)
        {
            res = double(1) - t;
        }
        else
        {
            res = t;
        }
    }
    return res;
}

double optionEngine::get_greeks(OptionType _optionType,Greeks _valueType, const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice,const double _vol,const double _elapsedTime,const double _remainTime){
	double greeks= 0;
	double upOption = 0; 
	double midOption = 0;
	double downOption = 0; 

	double upPrice =  _underlyingPrice + _underlyingPrice *this->m_dBump;
	double midPrice = _underlyingPrice;
	double downPrice =_underlyingPrice - _underlyingPrice *this->m_dBump;

	if( _valueType == Greeks::DELTA){
		upOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield, upPrice , _vol,_elapsedTime, _remainTime) ;
		downOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield, downPrice , _vol,_elapsedTime, _remainTime) ;
		greeks = (upOption - downOption)  / (_underlyingPrice * 2.0L * this->m_dBump);
	}
	else if( _valueType  == Greeks::GAMMA){
		upOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield,  _underlyingPrice + _underlyingPrice * this->m_gBump , _vol,_elapsedTime, _remainTime) ;
		midOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield,  _underlyingPrice  , _vol,_elapsedTime, _remainTime) ;
		downOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield,  _underlyingPrice - _underlyingPrice * this->m_gBump , _vol,_elapsedTime, _remainTime) ;
		greeks =  (upOption - 2.0F*midOption + downOption) / (_underlyingPrice* _underlyingPrice * this->m_gBump *this->m_gBump );
		//upOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::DELTA,  _strike, _riskFreeRate, _dividendYield, upPrice , _vol,_elapsedTime, _remainTime) ;
		//downOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::DELTA,  _strike, _riskFreeRate, _dividendYield, downPrice , _vol,_elapsedTime, _remainTime) ;
		//greeks = (upOption - downOption)  / (this->m_priceTick * 2.0L * this->m_dBump);

	}

	else if( _valueType  == Greeks::VEGA){
		upOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice , _vol + this->m_vBump,_elapsedTime, _remainTime) ;
		downOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice , _vol - this->m_vBump,_elapsedTime, _remainTime) ;
		greeks = (upOption - downOption)  / (2.0L * this->m_vBump);
	}
	else if( _valueType == Greeks::THETA){
		upOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice , _vol,_elapsedTime, _remainTime - 1.0F /365.0F) ;
		downOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice , _vol,_elapsedTime, _remainTime ) ;
		greeks = (upOption - downOption)  / (1.0F/365.0F) / 365.0F;// for year
	}
	else if(_valueType == Greeks::RHO){
		upOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate + this->m_rBump, _dividendYield, _underlyingPrice , _vol,_elapsedTime, _remainTime ) ;
		downOption	= (this->*m_pOptionCalculate)( _optionType, Greeks::PRICE,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice , _vol,_elapsedTime, _remainTime ) ;
		greeks = (upOption - downOption)  / (this->m_rBump) / 100.0F;
	}
	else {
		printf("error value Type!!!!!!!!!!");
		system("pause");
	}
	return greeks;
}

 void optionEngine::testHelloWord(){
	 cout << "call Hello World at " <<  __LINE__  << endl;
 }
 double optionEngine::americanBTreeDelta (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	double dt = _remainTime / this->m_step;
	double u=exp(_vol*sqrt(dt));
	double d=exp(-_vol*sqrt(dt));
	double p_up = (1-d)/(u-d);
    double p_down = 1.0 - p_up;
	double price1_u=americanBTree(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice*u,_vol,_elapsedTime, _remainTime*(this->m_step-1)/this->m_step);
	double price1_d=americanBTree(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice*d,_vol,_elapsedTime, _remainTime*(this->m_step-1)/this->m_step);
	double price0=americanBTree(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime, _remainTime);
	double delta_up=(price1_u-price0)/(_underlyingPrice*u-_underlyingPrice);
	double delta_down=(price1_d-price0)/(_underlyingPrice*d-_underlyingPrice);
	double delta=p_up*delta_up+p_down*delta_down;
	return delta;
 }
 double optionEngine::americanBTreeGamma(OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	double dt = _remainTime / this->m_step;
	double u=exp(_vol*sqrt(dt));
	double d=exp(-_vol*sqrt(dt));
	double p_up = (1-d)/(u-d);
    double p_down = 1.0 - p_up;
	double delta1_u=americanBTree(_optionType,Greeks::DELTA,_strike,_riskFreeRate,_dividendYield,_underlyingPrice*u,_vol,_elapsedTime, _remainTime*(this->m_step-1)/this->m_step);
	double delta1_d=americanBTree(_optionType,Greeks::DELTA,_strike,_riskFreeRate,_dividendYield,_underlyingPrice*d,_vol,_elapsedTime, _remainTime*(this->m_step-1)/this->m_step);
	double delta0=americanBTree(_optionType,Greeks::DELTA,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime, _remainTime);
	double gamma_u=(delta1_u-delta0)/(_underlyingPrice*u-_underlyingPrice);
	double gamma_d=(delta1_d-delta0)/(_underlyingPrice*d-_underlyingPrice);
	double gamma=p_up*gamma_u+p_down*gamma_d;
	return gamma;
 }

 double optionEngine::americanBTree (OptionType _optionType,Greeks _valueType,const double _strike,const double _riskFreeRate,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	 double greeks;
	 int step = this->m_step;
	 double dt = _remainTime / double(step);
	 // check dt
	 double dtCondition = _vol*_vol / ((_riskFreeRate - _dividendYield)*(_riskFreeRate - _dividendYield));
	 while(dt > dtCondition){
		 step += 100;
		dt =  _remainTime / double(step);
	}
 	if(_valueType != Greeks::PRICE){
		switch (_valueType)
		{
		case optionModel::DELTA:
			greeks = americanBTreeDelta ( _optionType, _valueType,  _strike,  _riskFreeRate,  _dividendYield,  _underlyingPrice,   _vol,  _elapsedTime,  _remainTime);
			break;
		case optionModel::GAMMA:
			greeks = americanBTreeGamma ( _optionType, _valueType,  _strike,  _riskFreeRate,  _dividendYield,  _underlyingPrice,   _vol,  _elapsedTime,  _remainTime);
			break;
		default:
			this->m_pOptionCalculate = &optionEngine::americanBTree;
			greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);	
			break;
		}
		return greeks;
	 }
	 double u = 0, p = 0,d = 0, rf = 0;
	 double direction = -1.0F;
	 int j = 0,i = 0;
	 double optionValue,neihanValue;
	 double p0, p1;
	 double EuropeanBinomial = 0,EuropeanAnalytic,AmericanBinomial,AmericanAnalytic;
	 double test;
	 double  discount = std::exp(-_riskFreeRate*dt);
	 vector<double> AOx(step+1,-1);
	 vector<double> EOx(step+1,-1);
	 if(this->m_underlyingType == underlyingType::Future){
		 rf = 0;

	 }else{
		 rf = _riskFreeRate - _dividendYield;
	 }
	 u = std::exp(_vol * std::sqrt(dt));
	 d = 1.0 / u;
	 p = (std::exp(rf * dt) - d) / (u - d);
	 p0 = discount * p;
	 p1 = discount  - p0;


	 if(_optionType == OptionType::CALL){
		direction = 1.0F;
	 }

	for( i = 0; i <= step; i++){
		AOx[i] = max<double>((_underlyingPrice * std::pow(u,double(2*i - step)) - _strike) * direction,0);
		EOx[i] = AOx[i];
	}
	for( j = step-1;j >= 0 ; j--){
		for( i = 0 ; i <= j; i ++){
			optionValue = (p1 * AOx[i] + p0* AOx[i+1]);
			neihanValue =  (_underlyingPrice * std::pow(u,double(2*i - j)) - _strike)*direction;
			AOx[i] = max(neihanValue,optionValue);
			EOx[i] =  (p1 * EOx[i] +  p0 * EOx[i+1]);
		}
	}
	EuropeanBinomial = EOx[0];
	EuropeanAnalytic = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
	AmericanBinomial = AOx[0];
	AmericanAnalytic = EuropeanAnalytic + (AmericanBinomial - EuropeanBinomial); //control variate methods;
	return AmericanAnalytic;
 
 }

 double  optionEngine::americanBinomial (OptionType _optionType,Greeks _valueType,const double _strike, double _riskFreeRate1111,const double _dividendYield,const double _underlyingPrice, const double _vol,const double _elapsedTime,const double _remainTime){
	 
	 int step = 200;
	 double dt = _remainTime / double(step); // 时间步长
	 // 二叉树使用参数
	 double u = std::exp(_vol*std::sqrt(dt));
	 double d = 1.0 / u;
	 double p = 0;
	 double rf = 0;
	 double m1 = 0, m2 =0;
	 int  N = step + 1;
	 double greeks = 0;
	 int j = 0,jj = 0,i = 0;
	 double optionValue,neihanValue;
	 double p0, p1;
	// vector<vector<double>> Sx(N,vector<double>(N,-1));
	 vector<double> AOx(N,-1);
	 vector<double> EOx(N,-1);

	 if(this->m_underlyingType == underlyingType::Future){
		 rf = _riskFreeRate1111;
		//_dividendYield = 0;//ignore _dividendYield for future
	 }else{
		 rf = _riskFreeRate1111 - _dividendYield;
	 }

	 //m1 = std::exp(rf * _remainTime);
	 //m2 = std::exp(2.0F * rf * _remainTime + _vol * _vol * _remainTime);
	// u = ((m2+1 + std::sqrt((m2+1) * (m2 + 1) - 4.0F* m1 *  m1))) / 2.0F * m1;
	 //d = 1.0/ u;
	// p = (m1 - d) / (u - d);
	 u = std::exp(_vol * std::sqrt(dt));
	 d = 1.0 / u;
	 p = (std::exp(rf * dt) - d) / (u - d);
	 p0 = std::exp(-_riskFreeRate1111*dt) * p;
	 p1 = std::exp(-_riskFreeRate1111*dt)  - p0;

	 if(_valueType != Greeks::PRICE){
		this->m_pOptionCalculate = &optionEngine::americanBinomial;
		greeks = this->get_greeks( _optionType, _valueType,  _strike, _riskFreeRate1111, _dividendYield, _underlyingPrice, _vol, _elapsedTime,_remainTime);	
		return greeks;
	}
	 

	 if(_optionType == OptionType::CALL){
			// 构造二叉树矩阵 i 表示行 j表示列 Sx为股价举证 fx为期权内在价值 

			// vector<vector<double>> Afx(N,vector<double>(N,-1));
			// vector<vector<double>> fx(N,vector<double>(N,-1));
			//for( j  = 0; j < N; j ++){
			//	for( i = 0; i < j+1; i ++){
			//		Sx[i][j] = _underlyingPrice*(std::pow(u,double(j-i)) * std::pow(d,double(i)));
			//	}
			//}
			//计算美式期权价格矩阵Afx
			for( i = 0; i < N; i++){
			//Afx[i][N-1] = fx[i][N-1];
				AOx[i] = max<double>(_underlyingPrice * std::pow(u,2*i - step) - _strike,0);;
				EOx[i] = AOx[i];
				//AOx[i] =  max((Sx[i][N-1] - _strike) ,0);
				//EOx[i] =  max((Sx[i][N-1] - _strike),0);
			}

			//倒推前期各类价格
			for( j = step-1;j >= 0 ; j--){
				for( i = 0 ; i <= j; i ++){
					optionValue = (p0 * AOx[i+1] + p1* AOx[i]);
					//neihanValue =  max(Sx[i][j-1] - _strike,0);
					neihanValue =  _underlyingPrice * std::pow(u,2*i - j) - _strike;;
					AOx[i] = max(optionValue,neihanValue);
					EOx[i] =  (p0 * EOx[i+1] +  p1 * EOx[i]);
			
				}
			}
			double EuropeanBinomial = EOx[0];
			double EuropeanAnalytic = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate1111,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
			double AmericanBinomial = AOx[0];
			double AmericanAnalytic = EuropeanAnalytic + (AmericanBinomial - EuropeanBinomial); //control variate methods;

			return EuropeanBinomial;
	 }

	 if(_optionType == OptionType::PUT){
			// 构造二叉树矩阵 i 表示行 j表示列 Sx为股价举证 fx为期权内在价值 

			// vector<vector<double>> Afx(N,vector<double>(N,-1));
			// vector<vector<double>> fx(N,vector<double>(N,-1));
		/*	for( j  = 0; j < N; j ++){
				for( i = 0; i < j+1; i ++){
					Sx[i][j] = _underlyingPrice*(std::pow(u,double(j-i)) * std::pow(d,double(i)));
				}
			}*/
			//计算美式期权价格矩阵Afx
			for( i = 0; i < N; i++){
			//Afx[i][N-1] = fx[i][N-1];
				AOx[i] = max<double>(_strike - _underlyingPrice * std::pow(u,2*i - step) ,0);;
				EOx[i] = AOx[i];
			//	AOx[i] =  max( _strike - Sx[i][N-1] ,0);
			//	EOx[i] =  max( _strike - Sx[i][N-1] ,0);
			}

			//倒推前期各类价格
			for( j = step - 1;j >= 0 ; j --){
				for( i = 0 ; i <= j; i ++){
					optionValue = (p0 * AOx[i+1] + p1* AOx[i]);
					//neihanValue =  max(_strike - Sx[i][j-1],0);
					neihanValue =  _strike - _underlyingPrice * std::pow(u,2*i - j);;
					AOx[i] = max(optionValue,neihanValue);
					EOx[i] = (p0 * EOx[i+1] +  p1 * EOx[i]);
				}
			}
			double EuropeanBinomial = EOx[0];
			double EuropeanAnalytic = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate1111,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
			double AmericanBinomial = AOx[0];
			double AmericanAnalytic = EuropeanAnalytic + (AmericanBinomial - EuropeanBinomial); //control variate methods;

			return AmericanAnalytic;
	 }
	 return 0;
	// // 构造二叉树矩阵 i 表示行 j表示列 Sx为股价举证 fx为期权内在价值 
	// vector<vector<double>> Sx(N,vector<double>(N,-1));
	// vector<double> AOx(N,-1);
	// vector<double> EOx(N,-1);
	//// vector<vector<double>> Afx(N,vector<double>(N,-1));
	//// vector<vector<double>> fx(N,vector<double>(N,-1));
	// for(int j  = 0; j < N; j ++){
	//	 for(int i = 0; i < j+1; i ++){
	//		 Sx[i][j] = _underlyingPrice*(std::pow(u,(j-i)) * std::pow(d,(i)));
	//	 }
	// }
	// //计算美式期权价格矩阵Afx
	// for(int i = 0; i < N; i++){
	//	//Afx[i][N-1] = fx[i][N-1];
	//	 AOx[i] =  max((Sx[i][N-1] - _strike) * direction,0);
	//	 EOx[i] =  max((Sx[i][N-1] - _strike) * direction,0);
	// }
	// int j = 0;
	// double optionValue,neihanValue;
	// //倒推前期各类价格
	// for(int jj = 0;jj < step ; jj ++){
	//	j = N - jj - 1;
	//	for(int i = 0 ; i < j; i ++){
	//		optionValue = std::exp(-_riskFreeRate*dt) * (p * AOx[i] + (1 - p) * AOx[i+1]);
	//		neihanValue =  max((Sx[i][j-1] - _strike) * direction,0);
	//		AOx[i] = max(optionValue,neihanValue);
	//		EOx[i] =  std::exp(-_riskFreeRate*dt) * (p * EOx[i] + (1 - p) * EOx[i+1]);
	//		
	//	}
	// }
	// double EuropeanBinomial = EOx[0];
	// double EuropeanAnalytic = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
	// double AmericanBinomial = AOx[0];
	// double AmericanAnalytic = EuropeanAnalytic + (AmericanBinomial - EuropeanBinomial); //control variate methods;

	// return AmericanAnalytic;

 }

 

 //void optionEngine::convertTime(cDate settlementDate,cDate nowDate,cDate maturityDate,double &elapsedTime,double &remainTime){
	// elapsedTime = ( nowDate.JulianDate() - settlementDate.JulianDate()) / 365.0F;
	// remainTime  = ( maturityDate.JulianDate() - nowDate.JulianDate()) / 365.0F;
 //}

 /*
 		double sigma_sqr = _vol*_vol;

		double time_sqrt = std::sqrt(remainTime);

		double nn = 2.0 * b /sigma_sqr; 

		double m = 2.0 * _riskFreeRate/sigma_sqr;  

		double K = 1.0-std::exp(-_riskFreeRate*remainTime); 

		double q = (-(nn-1)+omega *sqrt(std::pow((nn-1),2.0)+(4*m/K)))*0.5;

		// seed value from paper
		double q_inf = 0.5 * ( (-nn-1.0) + omega *std::sqrt(std::pow((nn-1),2.0)+4.0*m));

		double S_star_inf = _strike / (1.0 - 1.0/q_inf);

		double h = -1*omega*( b*remainTime+omega*2.0*_vol*time_sqrt)*(omega*_strike/(S_star_inf-_strike));
		if(omega == 1){
			S_seed = _strike + (S_star_inf-_strike)*(1.0-std::exp(h));
		}else{
			S_seed = S_star_inf + (_strike - S_star_inf) * std::exp(h);
		}
		int no_iterations=0; // iterate on S to find S_star, using Newton steps

		double Si=S_seed;         

		double g=1;

		double gprime=1.0;

		while ((fabs(g) > ACCURACY) 

		   && (fabs(gprime)>ACCURACY) // to avoid exploding Newton's  

		   && ( no_iterations++<500)

		   && (Si>0.0)) {

			   double option_bsm  = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,Si,_vol,_elapsedTime,_remainTime);

			double d1 = (log(Si/_strike)+(b+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

			g = omega * (Si - _strike) - option_bsm - omega * ( 1 - std::exp((b -_riskFreeRate ) *remainTime)*  normdist(omega*d1,0,1)) * Si / q;

			//g=(1.0-1.0/q)*Si-_strike-option_bsm+(1.0/q)*Si*exp((_dividendYield-_riskFreeRate)*remainTime)* normal_cdf(omega*d1,0.001);

			gprime=omega *( 1.0-1.0/q)*(1.0-exp((b-_riskFreeRate)*remainTime)*normdist(omega * d1,0,1)) +(1.0/q)*exp((b-_riskFreeRate)*remainTime)*normdist(omega * d1,0.0f,1.0f)*(1.0/(_vol*time_sqrt));

			Si=Si-(g/gprime); 
		};

		double S_star = 0;

		if (fabs(g)>ACCURACY) { S_star = S_seed; } // did not converge
		else { S_star = Si; };


		double d1 = (log(S_star/_strike)+(_riskFreeRate+0.5*sigma_sqr)*remainTime)/(_vol*time_sqrt);

		double A =  omega * (1.0-exp((b-_riskFreeRate)*remainTime)*normdist(omega * d1,0,1))* (S_star/q); 

		double e  = this->EuropeanCalculate(_optionType,Greeks::PRICE,_strike,_riskFreeRate,_dividendYield,_underlyingPrice,_vol,_elapsedTime,_remainTime);
		if(_optionType == OptionType::CALL){
			if (_underlyingPrice>=S_star) {resultValue=_underlyingPrice-_strike;} 
			else {
				resultValue=e+A*pow((_underlyingPrice/S_star),q);
			};
		}else{
			if (_underlyingPrice<=S_star) {resultValue=_strike-_underlyingPrice;} 
			else {
				resultValue=e+A*pow((_underlyingPrice/S_star),q);
			};
			
		}
 
 */