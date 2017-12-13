#pragma once

#include "option.h"

namespace AsianPrice{

	//enum CallPutFlag{CALL,PUT};
	class asianPrice
	{
		public:
		asianPrice();
		// n : totalPoint m : AveragedPoint t1:timeToFisrtAveragePoint
		double AsianCurranApprox(optionModel::OptionType t , double S, double SA, double X, double t1, double T, double n, double m, double r, double b, double v);
		double delta(optionModel::OptionType, double S, double SA, double X, double t1, double T, double n, double m, double r, double b, double v);
		double vega(optionModel::OptionType, double S, double SA, double X, double t1, double T, double n, double m, double r, double b, double v);
		double gamma(optionModel::OptionType, double S, double SA, double X, double t1, double T, double n, double m, double r, double b, double v);
		double rho(optionModel::OptionType, double S, double SA, double X, double t1, double T, double n, double m, double r, double b, double v);
		double theta(optionModel::OptionType, double S, double SA, double X, double t1, double T, double n, double m, double r, double b, double v);
		
		~asianPrice();
		double GBlackScholes(optionModel::OptionType, double S, double X, double T, double r, double b, double v);
		double CND(double);
		double CND2(double);
	};



}
