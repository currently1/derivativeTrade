#include "optionController.h"
#include <sstream>
#include <fstream>

#include <random>

#include <qsettings.h>
#include <qdatetime.h>
#define ELPP_THREAD_SAFE
#include "easylogging++.h"
#include "bjsmodel.h"


//! Used to simplify the throwing of formatted exceptions
#define XLW__HERE__ __FILE__ "(" _MAKESTRING(__LINE__) "): "
#define _MAKESTRING(a) __MAKESTRING(a)
#define __MAKESTRING(a) #a

#define ERROR__OUTPUT(ERROR_MSG_PARTS) \
    do { \
        std::ostringstream ostr; \
        ostr << ERROR_MSG_PARTS; \
        std::cerr << XLW__HERE__ << ostr.str() << std::endl; \
    } while(0)


#define  ACCURACY 1.0e-6



optionController::optionController(void)
{

	this->m_priceTick = 1; // default

	this->m_firstDay =false;
}


optionController::~optionController(void)
{
	//delete m_instTradeList;
	   //book->save("modify.xlsx");//保存文件

}


// 依据_valueType返回所需值
bool optionController::optionCalculate(double underlyingPrice ,time_t _curDateTime,optionGreeks &_CallgreeksValue,optionGreeks &_putGreeksValue,optionHedgeHands &_hedgeHands,optionParam &_optionParam){
	optionModel::OptionType optionType;
	bool resultCalculate = false;;
	double _curUnderlying = underlyingPrice;
	double initialPrice = this->m_dInitialPrice;
	if(this->m_dInitialPrice <= 0){
		initialPrice = _curUnderlying;
		this->m_dInitialPrice = initialPrice;
		// 开始执行时 定价未确定 调整当期时刻为3小时后  采用carry Theta修正
		//_curDateTime += DAY_SECOND;
	}
	//double (optionController::*optionCalculate)(char _optionType,char _valueType, double _strike,double _riskFreeRate,double _dividendYield,double _underlyingPrice,double _vol,,const double _elapsedTime,const double _remainTime);

	double callExecPrice = initialPrice * this->m_dCallExecPricePercent;
	double putExecPrice = initialPrice * this->m_dPutExecPricePercent;
	if(!valideDate( this->m_tSettlementDate, _curDateTime, this->m_tMaturityDate)){
		ERROR__OUTPUT("ERROR TIME INPUT");
		return resultCalculate;
	}

	getStandAnnualTime(_curDateTime,_optionParam.elapsedTime,_optionParam.remainTime);
	
	this->m_dCallExecPrice = callExecPrice;
	this->m_dPutExecPrice = putExecPrice;

	//选择期权种类

	if(this->m_optionEngine.m_pOptionCalculate == NULL){ERROR__OUTPUT("OptionKind Error");return 0;}
	// 修改delta为对冲带
	switch (this->m_iOptionKind)
	{
	case 0:
		this->m_optionEngine.setEurpeanParameters(optionModel::underlyingType::Future);
				if(m_iCallScale >0){
					optionType = optionModel::OptionType::CALL;
					_CallgreeksValue.price =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::PRICE,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.delta =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::DELTA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.gamma =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::GAMMA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.vega =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::VEGA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.theta = this->m_optionEngine.EuropeanCalculate(optionType,Greeks::THETA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.rho =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::RHO,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					}
				if(m_iPutScale >0){
					optionType = optionModel::OptionType::PUT;
						_putGreeksValue.price = this->m_optionEngine.EuropeanCalculate(optionType,Greeks::PRICE,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

						_putGreeksValue.delta =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::DELTA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

						_putGreeksValue.gamma =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::GAMMA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

						_putGreeksValue.vega =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::VEGA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

						_putGreeksValue.theta =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::THETA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

						_putGreeksValue.rho =  this->m_optionEngine.EuropeanCalculate(optionType,Greeks::RHO,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

				}
		break;
	case 1: // 亚式计算
		//m_pOptionCalculate =  &optionEngine::AsianCalculate;
		break;
	case 2:
			if(m_iCallScale >0){
					optionType = optionModel::OptionType::CALL;
					_CallgreeksValue.price =  this->m_optionEngine.americanBTree(optionType,Greeks::PRICE,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.delta =  this->m_optionEngine.americanBTree(optionType,Greeks::DELTA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.gamma =  this->m_optionEngine.americanBTree(optionType,Greeks::GAMMA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.vega =  this->m_optionEngine.americanBTree(optionType,Greeks::VEGA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.theta = this->m_optionEngine.americanBTree(optionType,Greeks::THETA,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_CallgreeksValue.rho =  this->m_optionEngine.americanBTree(optionType,Greeks::RHO,m_dCallExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					}
				if(m_iPutScale >0){
					optionType = optionModel::OptionType::PUT;
					_putGreeksValue.price = this->m_optionEngine.americanBTree(optionType,Greeks::PRICE,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_putGreeksValue.delta =  this->m_optionEngine.americanBTree(optionType,Greeks::DELTA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_putGreeksValue.gamma =  this->m_optionEngine.americanBTree(optionType,Greeks::GAMMA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_putGreeksValue.vega =  this->m_optionEngine.americanBTree(optionType,Greeks::VEGA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_putGreeksValue.theta =  this->m_optionEngine.americanBTree(optionType,Greeks::THETA,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);

					_putGreeksValue.rho =  this->m_optionEngine.americanBTree(optionType,Greeks::RHO,m_dPutExecPrice,this->m_dInterest,this->m_dDividend,_curUnderlying,this->m_dVolability,this->m_elaspedTme,this->m_reaminTime);


				}
		break;
	case 3:
		//m_pOptionCalculate =  &optionEngine::BarrierCalculate;
		break;
	case 4:
		//m_pOptionCalculate =  &optionEngine::LookBacCalculate;
		break;
	default:
		
		break;
	}

	if( this->m_iCallHedgeDirection == 0){
			_hedgeHands.callhedgeHands = getHands(this->m_iScaleUnit,this->m_iCallScale) * _CallgreeksValue.delta ;
	}else{
			_hedgeHands.callhedgeHands = getHands(this->m_iScaleUnit,this->m_iCallScale) * _CallgreeksValue.delta * -1;
		}
				
	if(this->m_iPutHedgeDirection == 0){
		_hedgeHands.puthedgeHands = getHands(this->m_iScaleUnit,this->m_iPutScale) * _putGreeksValue.delta ;
	}
	else{
		_hedgeHands.puthedgeHands = getHands(this->m_iScaleUnit,this->m_iPutScale) * _putGreeksValue.delta * -1;
	}

	_hedgeHands.hedgeHands = (_hedgeHands.callhedgeHands + _hedgeHands.puthedgeHands);

	_hedgeHands.DateTime = _curDateTime;

	_optionParam.callScale = this->m_iCallScale;

	_optionParam.putScale = this->m_iPutScale;

	_optionParam.settlementDate = this->m_sSettlementDate;

	_optionParam.maturityDate = this->m_sMaturityDate;

	_optionParam._optionKind = this->m_iOptionKind;

	_optionParam.volability = this->m_dVolability;

	_optionParam.initialPirce = this->m_dInitialPrice;

	_optionParam.callExecPrice = this->m_dCallExecPrice;

	_optionParam.putExecPrice = this->m_dPutExecPrice;
	resultCalculate = true;
	return resultCalculate;
}

//返回年化后的剩余时间 消耗时间
bool optionController::valideDate(time_t _settlementDate,time_t _todayDate,time_t _maturityDate){
	bool remark = true;
	if((_settlementDate )> _todayDate || _todayDate > (_maturityDate) || (_settlementDate ) > _maturityDate)
	{
		remark = false;
	}
	return remark;
}

void optionController::getStandAnnualTime(time_t _todayDate,double &elapsedTime,double &remainTime){

	elapsedTime = ( _todayDate - this->m_tSettlementDate) / YEAR_SECOND;

	remainTime = (this->m_tMaturityDate - _todayDate) / YEAR_SECOND;

	// 时间修正
	//if(elapsedTime < 0){
	//	elapsedTime = 0; remainTime = (this->m_tMaturityDate - this->m_tSettlementDate) / YEAR_SECOND;
	//}
	//if(m_reaminTime < 0){
	//	remainTime = 0; elapsedTime = (this->m_tMaturityDate - this->m_tSettlementDate) / YEAR_SECOND;
	//}

	// 重置时间
	this->m_elaspedTme = elapsedTime;
	this->m_reaminTime = remainTime;
}



double optionController::getHands(int scaleUnit,double scale){
	double Hands = 0;
	if(this->m_iScaleUnit == 0){//元				
		Hands	 = (scale *  (this->m_tMaturityDate-this->m_tSettlementDate)/ YEAR_SECOND  /this->m_dInitialPrice / this->m_iTickValue );	
	}
	else if(this->m_iScaleUnit == 1)//吨
	{
		Hands = (scale / this->m_iTickValue);
	}
	return Hands;
}
//
void optionController::set_instMessage_map_stgy(map<string, CThostFtdcInstrumentField*>& instMessage_map_stgy)
{
	m_instMessage_map_stgy = instMessage_map_stgy;
	cerr<<"收到合约个数:"<<m_instMessage_map_stgy.size()<<endl;

}
//设置交易的合约代码
void optionController::setInstId(string instId)
{
	strcpy_s(m_instId, instId.c_str());
}

void optionController::setUserId(string userId){
	this->m_userId = userId;
}

// remainTime CarryRemainTime为 剩余时间，carry剩余时间
void optionController::everDaySettle(CThostFtdcDepthMarketDataField *_closePrice,time_t _curTime,time_t _curTimeCarry,optionHedgeHands lastHedgeHands){
//	this->m_book = xlCreateXMLBook();
//	if(this->m_book)
//    {
//        m_book->setKey("Halil Kural", "windows-2723210a07c4e90162b26966a8jcdboe");//key 解除试用限制
//	}
//	else{
//		cerr<<"初始化excel book实例失败"<<endl;
//		system("pause");
//	}
	
//	string tradedate = _closePrice->TradingDay;
//	string yesterdayDate = this->m_yesterdayDate;
//	//setOptionCalculate();//指针赋值
//	if(abs(this->m_tSettlementDate - _curTime) < DAY_SECOND * 0.75){
//		m_firstDay = true;
//	}
//	if(!this->m_book) {cerr<<"excel book Error"<<endl;system("pause");}
//	if(m_firstDay){


//		//Write Title
//		string book_template = "input/template.xlsx";// 使用模版创建
//			if(this->m_book->load(book_template.c_str()))//载入文件
//			{
//				// Write MarketData
//				libxl::Sheet* sheet = this->m_book->getSheet(1);//获取第二个表格
//				if(sheet)
//				{
//					//title
//					sheet->writeStr(0,0,"TradeDate");
//					sheet->writeStr(0,1,"Code");
//					sheet->writeStr(0,2,"Name");
//					sheet->writeStr(0,3,"ExchangeId");
//					sheet->writeStr(0,4,"OptionKind");
//					sheet->writeStr(0,5,"Call/put");
//					sheet->writeStr(0,6,"ClosePrice");
//					sheet->writeStr(0,7,"UpperLimitPrice");
//					sheet->writeStr(0,8,"LowerLimitPrice");
//					sheet->writeStr(0,9,"MarketVol");
//					sheet->writeStr(0,10,"YesterDayVol");
//					sheet->writeStr(0,11,"Strike");
//					sheet->writeStr(0,12,"Interest");
//					sheet->writeStr(0,13,"VolumeMultiple");
//					sheet->writeStr(0,14,"SettlementDate");
//					sheet->writeStr(0,15,"MaturirtDate");
//					sheet->writeStr(0,16,"DelivDate");
//				}
//				sheet = this->m_book->getSheet(2);//position
//				if(sheet){
//					sheet->writeStr(0,0,"TradeDate");
//					sheet->writeStr(0,1,"contractName");
//					sheet->writeStr(0,2,"Quantity");
//					sheet->writeStr(0,3,"PV");
//					sheet->writeStr(0,4,"TradeAmount");
//				}
//				sheet = this->m_book->getSheet(3);//Risk
//				if(sheet){
//					//title
//					sheet->writeStr(0,0,"TradeDate");
//					sheet->writeStr(0,1,"contractName");
//					sheet->writeStr(0,2,"Delta(T+1)");
//					sheet->writeStr(0,3,"Delta(T+1)Lots");
//					sheet->writeStr(0,4,"Gamma(T+1)");
//					sheet->writeStr(0,5,"Vega(T+1)");
//					sheet->writeStr(0,6,"Theta(T+1)");
//					sheet->writeStr(0,7,"Rho(T+1)");
//					sheet->writeStr(0,8,"$Delta(T+1)");
//					sheet->writeStr(0,9,"$Gamma(T+1)");
//					sheet->writeStr(0,10,"$Vega(T+1)");
				
//				}
//				sheet = this->m_book->getSheet(4);//Pnl
//				if(sheet){
//					//title
//					sheet->writeStr(0,0,"TradeDate");
//					sheet->writeStr(0,1,"contractName");
//					sheet->writeStr(0,2,"Each Pnl");
//					sheet->writeStr(0,3,"Total Pnl");
//					sheet->writeStr(0,4,"IntradePnl");
//					sheet->writeStr(0,5,"HoldPnl");
//					sheet->writeStr(0,6,"NewPnl");
//					sheet->writeStr(0,7,"CostsOfFund");
//					sheet->writeStr(0,8,"Commission");
//					sheet->writeStr(0,9,"DeltaPnl");
//					sheet->writeStr(0,10,"GammaPnl");
//					sheet->writeStr(0,11,"VegaPnl");
//					sheet->writeStr(0,12,"ThetaPnl");
//					sheet->writeStr(0,13,"RhoPnl");
//					sheet->writeStr(0,14,"UnexplainPnl");
				
//				}
//				sheet = this->m_book->getSheet(5);
//				if(sheet){
//					sheet->writeStr(0,0,"TradeDate");
//					sheet->writeStr(0,1,"TradeTime");
//					sheet->writeStr(0,2,"合约代码");
//					sheet->writeStr(0,3,"方向");
//					sheet->writeStr(0,4,"开/平");
//					sheet->writeStr(0,5,"Volume");
//					sheet->writeStr(0,6,"TradePrice");
				
//				}

//				string resultDir = "output/" + this->m_sOptionCode + "_" + tradedate + "_settleFile.xlsx";
//				this->m_book->save(resultDir.c_str());
//			}
//			else
//			{
//				std::cout << m_book->errorMessage() << std::endl;
				
//			}
//	}


//	//打开上一日excel
//	string sourceDir = "output/" + this->m_sOptionCode + "_" + this->m_yesterdayDate + "_settleFile.xlsx";
	
//	if(this->m_book->load(sourceDir.c_str())){
//		//将交易流水和交易过程保存到Trade和detail中
//		// -----------------------------------------------Trade-------------------------------------------------------
//		this->loadHistoryTrade(tradedate,this->m_userId); // 首先载入当日历史成交记录
//		libxl::Sheet *sheet = this->m_book->getSheet(5);
//		if(sheet){
//			int i = sheet->lastRow();

//			for(auto iter=this->m_instTradeList.begin();iter<m_instTradeList.end();i++,iter++){

//				sheet->writeStr(i,0,iter->TradeDate);

//				sheet->writeStr(i,1,iter->TradeTime);

//				sheet->writeStr(i,2,iter->InstrumentID);

//				if(iter->Direction == '0')
//					sheet->writeStr(i,3,"买　");
//				else
//					sheet->writeStr(i,3,"　卖");

//				switch (iter->OffsetFlag)
//				{
//					case '0':
//						sheet->writeStr(i,4,"开仓");break;
//					case '1':
//						sheet->writeStr(i,4,"平仓");break;
//					case '2':
//						sheet->writeStr(i,4,"强平");break;
//					case '3':
//						sheet->writeStr(i,4,"平今");break;
//					case '4':
//						sheet->writeStr(i,4,"平昨");break;
//					case '5':
//						sheet->writeStr(i,4,"强减");break;
//					case '6':
//						sheet->writeStr(i,4,"本地强平");break;
//					default:
//						break;
//				}
//				sheet->writeNum(i,5,iter->Volume);
//				sheet->writeNum(i,6,iter->Price);
			
//			}





//		}
//		// -----------------------------------------------Detail-------------------------------------------------------
//		sheet = this->m_book->getSheet(6);
//		if(sheet){
		





//		}

//		//对于组合类期权 下列结算代码不支持 直接返回
//		if(this->m_iCallScale>0 && this->m_iPutScale>0){return;}

//		double scale = this->m_iCallScale>0 ?m_iCallScale:m_iPutScale;
//		int hedgeDirection = this->m_iCallScale>0 ?this->m_iCallHedgeDirection:this->m_iPutHedgeDirection;
//		// ---------------------------------option MarketData ---------------------------------------
//		sheet = this->m_book->getSheet(1);// marketData
//		string optionDirection = this->m_iCallScale>0 ? "看涨" : "看跌";
//		OptionType optionType = this->m_iCallScale>0 ? OptionType::CALL : OptionType::PUT;
//		// optionType = this->m_iCallScale>0 ? 'c' : 'p';
//		double yesterdayVol = 0;
//		double yesterdayClosePrice = _closePrice->ClosePrice;
//		double yesterdayPV = 0;
//		double VolabilitySell = this->m_priceVolability;
//		double execPrice = 0;
//		if(this->m_dInitialPrice<=0){
//			execPrice = this->m_iCallScale>0 ? _closePrice->ClosePrice * this->m_dCallExecPricePercent : _closePrice->ClosePrice * this->m_dPutExecPricePercent ;
//		}else{
//			execPrice = this->m_iCallScale>0 ?  this->m_dInitialPrice* this->m_dCallExecPricePercent : this->m_dInitialPrice * this->m_dPutExecPricePercent ;

//		}
//		if(sheet){
//			int lastRow = sheet->lastRow();
//			if(!m_firstDay){
//				VolabilitySell = sheet->readNum(lastRow-1,9);
//			}
//			//        option																future
//			sheet->writeStr(lastRow,0,_closePrice->TradingDay);								sheet->writeStr(lastRow+1,0,_closePrice->TradingDay);
//			sheet->writeStr(lastRow,1,this->m_sOptionCode.c_str());							sheet->writeStr(lastRow+1,1,this->m_instId);
//			sheet->writeStr(lastRow,2,this->m_sOptionName.c_str());							sheet->writeStr(lastRow+1,2,this->m_instMessage_map_stgy[this->m_instId]->InstrumentName);
//			sheet->writeStr(lastRow,3,"OTC");												sheet->writeStr(lastRow+1,3, this->m_instMessage_map_stgy[this->m_instId]->ExchangeID);
//			sheet->writeStr(lastRow,4,optionKindToStr(this->m_iOptionKind).c_str());		sheet->writeStr(lastRow+1,4,"");
//			sheet->writeStr(lastRow,5,optionDirection.c_str());								sheet->writeStr(lastRow+1,5,"");
//			sheet->writeStr(lastRow,6,"");													sheet->writeNum(lastRow+1,6,_closePrice->LastPrice);	//收盘价
//			sheet->writeStr(lastRow,7,"");													sheet->writeNum(lastRow+1,7,_closePrice->UpperLimitPrice);//最高价
//			sheet->writeStr(lastRow,8,"");													sheet->writeNum(lastRow+1,8, _closePrice->LowerLimitPrice);//最低价
//			sheet->writeStr(lastRow,9,"");													sheet->writeNum(lastRow+1,9,this->m_dVolability);//对冲波动率
//			sheet->writeStr(lastRow,10,"");													sheet->writeNum(lastRow+1,10,VolabilitySell);
//			sheet->writeNum(lastRow,11,execPrice);											sheet->writeStr(lastRow+1,11,"");
//			sheet->writeNum(lastRow,12,this->m_dInterest);									sheet->writeStr(lastRow+1,12,"");
//			sheet->writeStr(lastRow,13,"");													sheet->writeNum(lastRow+1,13,this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple);
//			sheet->writeStr(lastRow,14,this->m_sSettlementDate.substr(0,10).c_str());		sheet->writeStr(lastRow+1,14,this->m_instMessage_map_stgy[this->m_instId]->OpenDate);
//			sheet->writeStr(lastRow,15,this->m_sMaturityDate.substr(0,10).c_str());			sheet->writeStr(lastRow+1,15,this->m_instMessage_map_stgy[this->m_instId]->ExpireDate);
//			sheet->writeStr(lastRow,16,this->m_sMaturityDate.substr(0,10).c_str());			sheet->writeStr(lastRow+1,16,this->m_instMessage_map_stgy[this->m_instId]->EndDelivDate);
//			if(!m_firstDay){
//				yesterdayClosePrice = sheet->readNum(lastRow-1,6);//上一日收盘价
//				yesterdayVol = sheet->readNum(lastRow-1,9);//上一日波动率
//			}
		
//		}

//		// ---------------------------------Position Data ---------------------------------------------------------
//		sheet = this->m_book->getSheet(2);//postion;

//		double remainTime = 0,elapsedTime = 0;
//		double callHands = this->getHands(this->m_iScaleUnit,this->m_iCallScale);
//		double putHands = this->getHands(this->m_iScaleUnit,this->m_iPutScale);
//		double optionQuantity = callHands>0?callHands:putHands;// 期权数量
//		double futureQuantity = lastHedgeHands.netPos; // 期货头寸
//		this->getStandAnnualTime(_curTime,elapsedTime,remainTime);
//		double optionBuyPrice =		(this->m_optionEngine.EuropeanCalculate)(optionType,Greeks::PRICE,execPrice,this->m_dInterest,this->m_dDividend,_closePrice->ClosePrice,this->m_dVolability,	elapsedTime,remainTime);
//		double optionSellPrice =	(this->m_optionEngine.EuropeanCalculate)(optionType,Greeks::PRICE,execPrice,this->m_dInterest,this->m_dDividend,_closePrice->ClosePrice,VolabilitySell,elapsedTime,remainTime);
//		double optionPV = optionBuyPrice * optionQuantity * this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple;
//		double optionTradeAmount = optionSellPrice * optionQuantity * this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple; // 获取上一日PV值

//		double futurePV = futureQuantity*_closePrice->ClosePrice*this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple;

//		if(sheet){
//			int lastRow = sheet->lastRow();
//			if(!m_firstDay){
//				optionTradeAmount = sheet->readNum(lastRow-2,3); // 非首日用上一日PV值覆盖
//			}
//			//        option																future
//			sheet->writeStr(lastRow,0,_closePrice->TradingDay);								sheet->writeStr(lastRow+1,0,_closePrice->TradingDay);
//			sheet->writeStr(lastRow,1,this->m_sOptionName.c_str());							sheet->writeStr(lastRow+1,1,this->m_instMessage_map_stgy[this->m_instId]->InstrumentName);
//			sheet->writeNum(lastRow,2,optionQuantity);										sheet->writeNum(lastRow+1,2, futureQuantity);
//			sheet->writeNum(lastRow,3,optionPV);											sheet->writeNum(lastRow+1,3, futurePV);
//			sheet->writeNum(lastRow,4,optionTradeAmount);									sheet->writeNum(lastRow+1,4, 0);
//		}

//		// -----------------------------------------------RISK------------------------------------------------------
//		sheet = this->m_book->getSheet(3);//
//		double InsturmentVolumeMultiple = this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple;
//		double InstrumentClosePrice = _closePrice->ClosePrice;
//		optionGreeks CallOptionGreeks,PutOptionGreeks, optionGreeks_now;
//		optionGreeks CallOptionGreeksCarry,PutOptionGreeksCarry,optionGreeksCarry;
//		optionHedgeHands CurHedgeHands,CurHedgeHandsCarry;
//		optionParam _optionParam,_optionParamCarry;
//		bool optionCur = this->optionCalculate(_closePrice->LastPrice,_curTime,CallOptionGreeks,PutOptionGreeks,CurHedgeHands,_optionParam); // current T greeks
//		bool optionCarry = this->optionCalculate(_closePrice->LastPrice,_curTimeCarry,CallOptionGreeksCarry,PutOptionGreeksCarry,CurHedgeHandsCarry,_optionParamCarry);// carry current T
//		if(this->m_iCallScale>0){
//			optionGreeks_now = CallOptionGreeks;
//			optionGreeksCarry = CallOptionGreeksCarry;
//		}
//		else{
//			optionGreeks_now = PutOptionGreeks;
//			optionGreeksCarry = PutOptionGreeksCarry;
//		}

//		// for option
//		double DeltaTpassOne = optionGreeksCarry.delta * optionQuantity * InsturmentVolumeMultiple  * hedgeDirection;
//		double DeltaHandTpassOne = DeltaTpassOne / InsturmentVolumeMultiple;
//		double GammaTPassOne = optionGreeksCarry.gamma * optionQuantity * InsturmentVolumeMultiple  * hedgeDirection;
//		double VegaTPassOne = optionGreeksCarry.vega * optionQuantity * InsturmentVolumeMultiple  * hedgeDirection;
//		double ThetaTPassOne = optionGreeksCarry.theta * optionQuantity * InsturmentVolumeMultiple * hedgeDirection / 365.0L;//换算为每日
//		double RhoTPassOne = optionGreeksCarry.rho * optionQuantity * InsturmentVolumeMultiple  * hedgeDirection;
//		//dollarisation
//		double deltaDollarise = DeltaTpassOne * InstrumentClosePrice;
//		double gammaaDollarise = GammaTPassOne * InstrumentClosePrice * InstrumentClosePrice / 100.0L;
//		double vegaDollarise = VegaTPassOne * this->m_dVolability * 100 ;
//		// for futures
//		double DeltaFuture = lastHedgeHands.netPos * InsturmentVolumeMultiple;
//		double DeltaFutureHand = lastHedgeHands.netPos ;
//		double DeltaFutureDollarise = lastHedgeHands.netPos * InsturmentVolumeMultiple * InstrumentClosePrice;
//		// For calculate Pnl
//		double lastDeltaTp1 = 0,lastGammaTp1 = 0,lastVegaTp1 = 0,lastThetaTp1 = 0,lastRhoTp1 = 0;
	
//		if(sheet){
//			int lastRow = sheet->lastRow();
//			if(!m_firstDay){
//				lastDeltaTp1 = sheet->readNum(lastRow-2,8);
//				lastGammaTp1 = sheet->readNum(lastRow-2,9);
//				lastVegaTp1 = sheet->readNum(lastRow-2,5);
//				lastThetaTp1 = sheet->readNum(lastRow-2,6);
//				lastRhoTp1 = sheet->readNum(lastRow-2,7);
//			}
//			//        option																future
//			sheet->writeStr(lastRow,0,_closePrice->TradingDay);								sheet->writeStr(lastRow+1,0,_closePrice->TradingDay);
//			sheet->writeStr(lastRow,1,this->m_sOptionName.c_str());							sheet->writeStr(lastRow+1,1,this->m_instMessage_map_stgy[this->m_instId]->InstrumentName);
//			sheet->writeNum(lastRow,2,DeltaTpassOne);										sheet->writeNum(lastRow+1,2, DeltaFuture);
//			sheet->writeNum(lastRow,3,DeltaHandTpassOne);									sheet->writeNum(lastRow+1,3,DeltaFutureHand);
//			sheet->writeNum(lastRow,4,GammaTPassOne);
//			sheet->writeNum(lastRow,5,VegaTPassOne);
//			sheet->writeNum(lastRow,6,ThetaTPassOne);
//			sheet->writeNum(lastRow,7,RhoTPassOne);
//			sheet->writeNum(lastRow,8,deltaDollarise);										sheet->writeNum(lastRow+1,8,DeltaFutureDollarise);
//			sheet->writeNum(lastRow,9,gammaaDollarise);
//			sheet->writeNum(lastRow,10,vegaDollarise);
		
//		}

//		// -----------------------------------------------Pnl-------------------------------------------------------
//		sheet = this->m_book->getSheet(4);
		
//		double priceReturn = (_closePrice->ClosePrice / yesterdayClosePrice - 1);
//		// for option
//		double optionPnl = optionTradeAmount - optionPV;
//		double deltaPnl = lastDeltaTp1 * priceReturn;
//		double gammaPnl = lastGammaTp1 * priceReturn * priceReturn * 0.5L * 100.0L;
//		double vegaPnl = (optionSellPrice - optionBuyPrice) * optionQuantity * InsturmentVolumeMultiple;
//		double thetaPnl = lastThetaTp1 * 1;
//		double rhoPnl = 0;
//		double otherPnl = optionPnl - deltaPnl - gammaPnl -vegaPnl - thetaPnl;
	
//		// for futures
//		double intradePnl = 0, holdPostionPnl = 0, newPositionPnl = 0;
	
//		//依次计算
		
//		this->calculateFuturePnl(_closePrice->TradingDay,yesterdayClosePrice,InstrumentClosePrice,lastHedgeHands,intradePnl,holdPostionPnl,newPositionPnl);
//		double futurePnl = intradePnl + holdPostionPnl + newPositionPnl;
//		double TotalPnl = futurePnl + optionPnl;
//		double CostOfFunds = 0;
//		double Commission = 0;
//		if(sheet){
//			int lastRow = sheet->lastRow();
			
//			//        option																future
//			sheet->writeStr(lastRow,0,_closePrice->TradingDay);								sheet->writeStr(lastRow+1,0,_closePrice->TradingDay);
//			sheet->writeStr(lastRow,1,this->m_sOptionName.c_str());							sheet->writeStr(lastRow+1,1,this->m_instMessage_map_stgy[this->m_instId]->InstrumentName);
//			sheet->writeNum(lastRow,2,optionPnl);											sheet->writeNum(lastRow+1,2, futurePnl);
//			sheet->writeNum(lastRow,3,TotalPnl);
//																							sheet->writeNum(lastRow+1,4,intradePnl);
//																							sheet->writeNum(lastRow+1,5,holdPostionPnl);
//																							sheet->writeNum(lastRow+1,6,newPositionPnl);
//			sheet->writeNum(lastRow,7,CostOfFunds);
//			sheet->writeNum(lastRow,8,Commission);
//			sheet->writeNum(lastRow,9,deltaPnl);
//			sheet->writeNum(lastRow,10,gammaPnl);
//			sheet->writeNum(lastRow,11,vegaPnl);
//			sheet->writeNum(lastRow,12,thetaPnl);
//			sheet->writeNum(lastRow,13,rhoPnl);
//			sheet->writeNum(lastRow,14,otherPnl);
//		}
//		string settleFile = "output/" + this->m_sOptionCode + "_" + tradedate + "_settleFile.xlsx";
//		if(this->m_book->save(settleFile.c_str())){
		
//		}else{
//			cout << this->m_book->errorMessage() << endl;
//		}
		
//	}
//	else{
		
//		std::cout << m_book->errorMessage() << std::endl;
	
//	}
//	this->m_book->release();
//	// 完结 撒花。。。。
}
//依据日期和交易日 载入成交列表
bool optionController::loadHistoryTrade(string date,string userId){
	ifstream in;
	char buffer[1024];
	CThostFtdcTradeField trade_message ;
	in.open("output/tradeList/" + userId + "_"+ this->m_instId + "_" + date + ".csv",ios::in) ;
	if(in){
		int lineTag = 0;
		while(in.getline(buffer,1024,'\n')){
			if(lineTag == 0){
				lineTag++;
				continue;
			}	
			int len=strlen(buffer);
			char tmp[30];
			for(int i=0,j=0,count=0;i<=len;i++){
				if(buffer[i]!= ',' && buffer[i]!='\0'){
					tmp[j++]=buffer[i];
				}
				else{
					tmp[j]='\0';
					count++;
					j=0;
					switch(count){
					case 1:
						memcpy(trade_message.BrokerID,tmp,sizeof(trade_message.BrokerID));
						break;
					case 2:
						memcpy(trade_message.InvestorID,tmp,sizeof(trade_message.InvestorID));
						break;
					case 3:
						memcpy(trade_message.InstrumentID,tmp,sizeof(trade_message.InstrumentID));
						break;
					case 4:
						memcpy(trade_message.OrderRef,tmp,sizeof(trade_message.OrderRef));
						break;
					case 5:
						memcpy(trade_message.UserID,tmp,sizeof(trade_message.UserID));
						break;
					case 6:
						memcpy(trade_message.ExchangeID,tmp,sizeof(trade_message.ExchangeID));
						break;
					case 7:
						memcpy(trade_message.TradeID,tmp,sizeof(trade_message.TradeID));
						break;
					case 8:
						trade_message.Direction = tmp[0];
						break;
					case 9:
						memcpy(trade_message.OrderSysID,tmp,sizeof(trade_message.OrderSysID));
						break;
					case 10:
						memcpy(trade_message.ParticipantID,tmp,sizeof(trade_message.ParticipantID));
						break;
					case 11:
						memcpy(trade_message.ClientID,tmp,sizeof(trade_message.ClientID));
						break;
					case 12:
						trade_message.TradingRole = tmp[0];
						break;
					case 13:
						memcpy(trade_message.ExchangeInstID,tmp,sizeof(trade_message.ExchangeInstID));
						break;
					case 14:
						trade_message.OffsetFlag=tmp[0];
						break;
					case 15:
						trade_message.HedgeFlag=tmp[0];
						break;
					case 16:
						trade_message.Price= atof(tmp);
						break;
					case 17:
						trade_message.Volume= atof(tmp);
						break;
					case 18:
						memcpy(trade_message.TradeDate,tmp,sizeof(trade_message.TradeDate));
						break;
					case 19:
						memcpy(trade_message.TradeTime,tmp,sizeof(trade_message.TradeTime));
						break;
					case 20:
						trade_message.TradeType=tmp[0];
						break;
					case 21:
						trade_message.PriceSource=tmp[0];
						break;
					case 22:
						memcpy(trade_message.TraderID,tmp,sizeof(trade_message.TraderID));
						break;
					case 23:
						memcpy(trade_message.OrderLocalID,tmp,sizeof(trade_message.OrderLocalID));
						break;
					case 24:
						memcpy(trade_message.ClearingPartID,tmp,sizeof(trade_message.ClearingPartID));
						break;
					case 25:
						memcpy(trade_message.BusinessUnit,tmp,sizeof(trade_message.BusinessUnit));
						break;
					case 26:
						trade_message.SequenceNo =atoi(tmp);
						break;
					case 27:
						memcpy(trade_message.TradingDay,tmp,sizeof(trade_message.TradingDay));
						break;
					case 28:
						trade_message.SettlementID =atoi(tmp);
						break;
					case 29:
						trade_message.BrokerOrderSeq =atoi(tmp);
						break;
					case 30:
						trade_message.TradeSource=tmp[0];
						break;
					}
				}
			}
			m_instTradeList.push_back(trade_message);
		}
	
	}

	return true;
}

bool optionController::calculateFuturePnl(string date,double yesterdayClose,double closePrice,optionHedgeHands lastHedgeHands,double& intradeDay,double& holdPosition,double& newPosition){
	intradeDay = 0;
	holdPosition = 0;
	newPosition = 0;

	int dayLong = 0, dayShort = 0;
	double sellAverage = 0, buyAverage = 0;
	
	for(auto iter = this->m_instTradeList.begin();iter != this->m_instTradeList.end();iter++){
		//依次计算 期货盈亏
		if(strcmp(iter->InstrumentID, m_instId) == 0 && iter->InvestorID == this->m_userId){
			if(iter->Direction == '0'){
				dayLong += iter->Volume;
				buyAverage += iter->Price;
			}
			else if(iter->Direction == '1'){
				dayShort += iter->Volume;
				sellAverage += iter->Price;
			}
		}
	}
	if(dayLong>0)
	buyAverage /= double(dayLong);
	if(dayShort>0)
	sellAverage /= double(dayShort);
	
	intradeDay = min(dayLong,dayShort) * (sellAverage - buyAverage) * this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple;
	holdPosition = (lastHedgeHands.netPos - (dayLong - dayShort)) * (closePrice - yesterdayClose)* this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple;
	if((dayLong - dayShort) > 0)
		newPosition = (dayLong-dayShort)*(closePrice-buyAverage) * this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple;
	else
		newPosition = (dayShort-dayLong)*(sellAverage-closePrice) * this->m_instMessage_map_stgy[this->m_instId]->VolumeMultiple;

	return true;


}
void optionController::loadConf(QString loadDir){
	QString saveproductName = loadDir ;
	QSettings *configSetting = new QSettings(saveproductName, QSettings::IniFormat); 
	configSetting->setIniCodec("GB2312");
	//期权种类
	int iOptionKind = configSetting->value("/OptionParam/optionKind").toInt();
	this->m_iOptionKind = optionKind(iOptionKind);
	QString settlementDate = configSetting->value("/OptionParam/settlementDate").toString();
	this->m_tSettlementDate = strTime2unix(settlementDate.toStdString());
	this->m_sSettlementDate = settlementDate.toStdString();
	QString  maturityDate = configSetting->value("/OptionParam/maturityDate").toString();
	this->m_tMaturityDate = strTime2unix(maturityDate.toStdString());
	this->m_sMaturityDate = maturityDate.toStdString();
	this-> m_iCallScale = configSetting->value("/OptionParam/callScale").toInt(); 
	this->  m_iPutScale = configSetting->value("/OptionParam/putScale").toInt(); 
	//利率
	this->  m_dInterest = configSetting->value("/OptionParam/interest").toDouble();
	//红利率
	this->  m_dDividend = configSetting->value("/OptionParam/dividend").toDouble();
	this->  m_dVolability = configSetting->value("/OptionParam/hedgeVolability").toDouble();
	this->  m_priceVolability = configSetting->value("/OptionParam/priceVolability").toDouble();
	//亚式均价
	this->  m_dAveragePrice = configSetting->value("/OptionParam/averagePrice").toDouble();
	//障碍值
	this->  m_dBarrierValue = configSetting->value("/OptionParam/barrier").toDouble();
	this->  m_dCallExecPricePercent = configSetting->value("/OptionParam/callExecPricePercent").toDouble();
	this->  m_dPutExecPricePercent = configSetting->value("/OptionParam/putExecPricePercent").toDouble();
	this->  m_dInitialPrice = configSetting->value("/OptionParam/initialPrice").toDouble();
	// meid
	this->  m_iTickValue = configSetting->value("/OptionParam/tickValue").toDouble();
	this->m_iScaleUnit = configSetting->value("/OptionParam/scaleUnit").toInt(); 
	this-> m_iCallHedgeDirection = configSetting->value("/OptionParam/callHedgeDirection").toInt();
	this-> m_iPutHedgeDirection = configSetting->value("/OptionParam/putHedgeDirection").toInt();
	this-> m_sOptionName = configSetting->value("/OptionParam/productName").toString().toStdString();
	delete configSetting;
}
