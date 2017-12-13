#pragma once
#include "option.h"
using namespace optionModel;
class JuZhong99
{
	//S：标的资产现价  
    //X：执行价  
    //r：无风险利率  
    //q：连续分红率，Cost of Carry = r-q  
    //sigma：波动率  
    //t：距离到期时间  
    //PutCall：Call/Put 

public:
	JuZhong99(void);
	JuZhong99(underlyingType);
	~JuZhong99(void);
	double getOptionValue(double S, double X, double q, double r,  
		double sigma, double t, optionModel::OptionType PutCall);
	double getDelta(double S, double X, double q, double r,  
        double sigma, double t, OptionType PutCall);
	double getGamma(double S, double X, double q, double r,  
        double sigma, double t, OptionType PutCall);
	double getTheta(double S, double X, double q, double r,  
        double sigma, double t, OptionType PutCall);
	double getVega(double S, double X, double q, double r,  
        double sigma, double t, OptionType PutCall);
	double getRho(double S, double X, double q, double r,  
        double sigma, double t, OptionType PutCall);
	double getImpliedVol(double S, double X, double q, double r,double optionPrice,  
        double t, OptionType PutCall, double accuracy, int maxIterations);
private:
	double* GetChi(double S, double Sstar, double X, double q, double r,  
		double sigma, double t, double hAh, double phi)  ;
	double GetVh(double S, double Sstar, double X, double q, double r,   
		double sigma, double t, double phi)  ;
	double GetSstar(double phi,double q, double t, double sigma, double r, double X,   double accuracy, int maxIterations, double Sseed);
	double GetLamda(double t, double r, double q, double sigma, double phi);
	double GetBeta(double r, double q, double sigma);
	double GetAlpha(double r, double sigma) ;
	double GetH(double r, double t) ;
	OptionType putCall;
	underlyingType m_underlyingType;
	optionEngine m_optionEngine;
};

