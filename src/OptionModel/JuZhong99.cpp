#include "stdafx.h"
#include "JuZhong99.h"


JuZhong99::JuZhong99(void)
{
	
}
JuZhong99::JuZhong99(underlyingType ut){
	this->m_underlyingType = ut;
	this->m_optionEngine.setUnderlyingType(ut);
}

JuZhong99::~JuZhong99(void)
{
}
double JuZhong99::getOptionValue(double S, double X, double q, double r,  
								 double sigma, double t, OptionType PutCall)  
{  
            if(r==0)  
                r = r+1e-10;  
            if(q==0)  
                q = q+1e-10;  
			optionEngine myBS; 
            double phi;  
              
			if(PutCall==OptionType::CALL)  
            {  
                phi = 1.0;  
            }  
            else  
            {  
                phi = -1.0;  
            }  
                      
            double Sseed = X;  
            double Sstar = GetSstar(phi, q, t, sigma, r, X, 1e-6, 1000, Sseed);  
  
            if(phi*(Sstar-S)>0)  
            {                 
				double BSprice =this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::PRICE,X,r,q,S,sigma,0,t);  
                double hAh = phi*(Sstar-X) - this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::PRICE,X,r,q,Sstar,sigma,0,t);  ;  
                double* chi = this->GetChi(S, Sstar, X, q, r, sigma, t, hAh, phi);  
                double lamda = GetLamda(t, r, q, sigma, phi);  
  
				return BSprice+hAh*std::pow(S/Sstar,lamda)/(1-chi[0]);  
                  
            }  
            else  
            {  
                return phi*(S-X);  
            }  
}

double JuZhong99::getDelta(double S, double X, double q, double r,  
				double sigma, double t, OptionType PutCall)  
{  
            if(r==0)  
                r = r+1e-10;  
            double phi;  
			if(PutCall == OptionType::CALL)  
            {  
                phi = 1.0;  
          
            }  
            else  
            {  
                phi = -1.0;  
            }  
                      
            double Sseed = X;  
            double Sstar = GetSstar(phi, q, t, sigma, r, X, 1e-6, 1000, Sseed);  
  
			double deltaBS = this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::DELTA,X,r,q,S,sigma,0,t);  
			double BSprice = this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::PRICE,X,r,q,Sstar,sigma,0,t);  
            double hAh = phi*(Sstar-X)-BSprice;  
            double* chi = this->GetChi(S, Sstar, X, q, r, sigma, t, hAh, phi);  
            double lamda = GetLamda(t, r, q, sigma, phi);  
              
            return deltaBS+(lamda/S/(1.0-chi[0])+chi[1]/std::pow(1.0-chi[0],2))  
                *hAh*std::pow(S/Sstar,lamda);  
}  

double JuZhong99::getGamma(double S, double X, double q, double r,  
						   double sigma, double t, OptionType PutCall)  
    {  
        if(r==0)  
            r = r+1e-10;   
        double phi;  
		if(PutCall==OptionType::CALL)  
        {  
            phi = 1.0;  
        }  
        else  
        {  
            phi = -1.0;  
       
        }  
                      
        double Sseed = X;  
        double Sstar = GetSstar(phi, q, t, sigma, r, X, 1e-6, 1000, Sseed);  
              
		double gammaBS = this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::GAMMA,X,r,q,S,sigma,0,t);        
		double BSprice = this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::PRICE,X,r,q,Sstar,sigma,0,t); 
        double hAh = phi*(Sstar-X)-BSprice;  
        double* chi = GetChi(S, Sstar, X, q, r, sigma, t, hAh, phi);  
        double lamda = GetLamda(t, r, q, sigma, phi);  
                          
        return gammaBS+(2.0*lamda*chi[1]/S/std::pow(1.0-chi[0],2)  
                        +2.0*std::pow(chi[1],2)/std::pow(1.0-chi[0],3)  
                        +chi[2]/std::pow(1.0-chi[0],2)  
                        +(lamda*lamda-lamda)/S/S/(1.0-chi[0]))   
            *(phi*(Sstar-X)-BSprice)*std::pow(S/Sstar,lamda);  
    }  

double JuZhong99::getTheta(double S, double X, double q, double r,  
	 double sigma, double t, OptionType PutCall)  
{  
	double gamma = this->getGamma(S, X, q, r, sigma, t, PutCall);  
	double delta = this->getDelta(S, X, q, r, sigma, t, PutCall);  
	double optionValue = this->getOptionValue(S, X, q, r, sigma, t, PutCall);  
              
    return r*optionValue-0.5*std::pow(sigma*S,2)*gamma-(r-q)*S*delta;  
}  


double JuZhong99::getVega(double S, double X, double q, double r,  
						  double sigma, double t, OptionType PutCall)  
{  
	double optionValue0 = this->getOptionValue(S, X, q, r, sigma+0.001, t, PutCall);  
	double optionValue1 = this->getOptionValue(S, X, q, r, sigma-0.001, t, PutCall);  
              
    return (optionValue0-optionValue1)/0.002;  
}
double JuZhong99::getRho(double S, double X, double q, double r,  
						 double sigma, double t, OptionType PutCall)  
{  
	double optionValue0 = this->getOptionValue(S, X, q, r+0.001, sigma, t, PutCall);  
	double optionValue1 = this->getOptionValue(S, X, q, r-0.001, sigma, t, PutCall);  
              
    return (optionValue0-optionValue1)/0.002;  
}  

 double JuZhong99::getImpliedVol(double S, double X, double q, double r,double optionPrice,  
	 double t, OptionType PutCall, double accuracy, int maxIterations)  
{  
	double t_sqrt = std::sqrt(t);  
    double sigma = optionPrice/S/0.398/t_sqrt;  
    for(int i=0; i<maxIterations; i++)  
    {  
		double price = this->getOptionValue(S, X, q, r, sigma, t, PutCall);  
        double diff = optionPrice-price;  
		if(std::abs(diff)<accuracy)  
            return sigma;  
		double vega = this->getVega(S, X, q, r, sigma, t, PutCall);  
        sigma = sigma+diff/vega;  
    }  
    return sigma;  
}  

 double* JuZhong99::GetChi(double S, double Sstar, double X, double q, double r,  
                              double sigma, double t, double hAh, double phi)  
{  
    double beta = GetBeta(r,q,sigma);  
    double alpha = GetAlpha(r, sigma);  
    double h = GetH(r, t);  
              
    double factor0 = std::sqrt(std::pow((beta-1.0),2)+4.0*alpha/h);  
    double lamda1 = -phi*alpha/h/h/factor0;  
    double lamda = 0.5*(1.0-beta+phi*factor0);  
    double factor1 = (1.0-h)*alpha/(2.0*lamda+beta-1.0);  
    double Vh = GetVh(S, Sstar, X, q, r, sigma, t, phi);  
  
    double b = factor1*lamda1*0.5;  
    double c = -factor1*(1.0/hAh*Vh +1.0/h+lamda1/(2.0*lamda+beta-1.0));                  
              
    double* chi = new double[3];  
    chi[0] = b*std::pow(std::log(S/Sstar),2)+c*std::log(S/Sstar);  
    chi[1] = (2.0*b*std::log(S/Sstar)+c)/S;  
    chi[2] = 2.0*b/S/S-chi[1]/S;  
    return chi;  
}  
 double JuZhong99::GetVh(double S, double Sstar, double X, double q, double r,   
                              double sigma, double t, double phi)  
{  
    double t_sqrt = std::sqrt(t);  
    double sigma2 = sigma*sigma;  
    double d1 = (std::log(Sstar/X)+(r-q+sigma2*0.5)*t)/(t_sqrt*sigma);  
    double d2 = d1-t_sqrt*sigma;  
              
            
	double Nd1 = this->m_optionEngine.normdist(phi*d1,0,1);  
    double Nd2 = this->m_optionEngine.normdist(phi*d2,0,1);  
	double nd1 = this->m_optionEngine.normal_pdf(d1,0,1);  
	double Qt = std::exp((r-q)*t);  
              
    return Sstar*nd1*sigma*Qt*0.5/r/t_sqrt-phi*q*Sstar*Nd1*Qt/r+phi*X*Nd2;  
}  

 double JuZhong99::GetSstar(double phi,double q, double t, double sigma, double r, double X,  
                                double accuracy, int maxIterations, double Sseed)  
        {  
            double Sstar = Sseed;  
			OptionType PutCall;
			PutCall = phi>0?OptionType::CALL:OptionType::PUT;  
  
            double g = 1.0;  
            double g1 = 1.0;  
            int noInterations = 0;  
                  
            while((std::abs(g)>accuracy) && (std::abs(g1)>accuracy)   
                  && (noInterations++<maxIterations) && (Sstar>0.0))  
            {  
				double BSprice =   this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::PRICE,X,r,q,Sstar,sigma,0,t);  
				double delta   =   this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::DELTA,X,r,q,Sstar,sigma,0,t);
				double gamma   =   this->m_optionEngine.EuropeanCalculate(PutCall,Greeks::GAMMA,X,r,q,Sstar,sigma,0,t);         
                double lamda = GetLamda(t, r, q, sigma, phi);  
                g = Sstar*(phi-delta)-lamda*(phi*(Sstar-X)-BSprice);              
                g1 = phi-delta-Sstar*gamma-lamda*(phi-delta);  
                  
                Sstar = Sstar-g/g1;  
            }  
              
            return Sstar;  
        }  
          
double JuZhong99::GetLamda(double t, double r, double q, double sigma, double phi)  
        {  
            double beta = GetBeta(r,q,sigma);  
            double alpha = GetAlpha(r, sigma);  
            double h = GetH(r, t);  
  
            return 0.5*(1.0-beta+phi*std::sqrt(std::pow((beta-1.0),2)+4.0*alpha/h));  
        }  
          
double JuZhong99::GetBeta(double r, double q, double sigma)  
        {  
            return 2.0*(r-q)/sigma/sigma;  
        }  
          
  double JuZhong99::GetAlpha(double r, double sigma)  
        {  
            return 2.0*r/sigma/sigma;  
        }  
          
  double JuZhong99::GetH(double r, double t)  
        {  
            return 1.0-std::exp(-r*t);  
        }  
    