// BJSMODEL.h

namespace bjsmodel
{
	class bjsmodel
	{
    public: 
           double BSAmericanCallApprox2002(double S, double  X, double T, double r, double b, double v);
           double phi(double S, double T, double gamma, double h, double i, double r, double b, double v);
           double ksi(double S, double T2, double gamma, double h,double I2, double I1, double t1, double r, double b, double v);
           double BSAmericanApprox2002(char CallPutFlag, double S, double  X, double T, double r, double b, double v);
           double GBlackScholes(char CallPutFlag, double S, double  X, double T, double r, double b, double v);
           double CBND(double X, double y, double rho);
           double CND(double x); 
           double vega(char CallPutFlag, double S, double  X, double T, double r, double b, double v, int m);
           double ImpliedVol(char CallPutFlag, double S, double  X, double T, double r, double b, double cm, double epsilon, int m);
		   double Max(double a, double b);
		   double delta(char CallPutFlag, double S, double  X, double T, double r, double b, double v, int m); 
		   double gamma(char CallPutFlag, double S, double  X, double T, double r, double b, double v, int m); 
		   double rho(char CallPutFlag, double S, double  X, double T, double r, double b, double v, int m); 
		   double theta(char CallPutFlag, double S, double  X, double T, double todayToNextTradeDayNum, double r, double b, double v); 

		   double vega(char CallPutFlag, double S, double  X, double T, double r, double b, double v);
           double ImpliedVol(char CallPutFlag, double S, double  X, double T, double r, double b, double cm);
		   double delta(char CallPutFlag, double S, double  X, double T, double r, double b, double v); 
		   double gamma(char CallPutFlag, double S, double  X, double T, double r, double b, double v); 
		   double rho(char CallPutFlag, double S, double  X, double T, double r, double b, double v); 
		   double* bjsmodelresult(char CallPutFlag, double S, double  X, double T,double t, double r, double b, double ImpliedVol, int m, double epsilon);
	
     };
}