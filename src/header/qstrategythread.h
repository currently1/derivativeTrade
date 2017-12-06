#ifndef QSTRATEGYTHREAD_H
#define QSTRATEGYTHREAD_H

#include <QThread>
#include <QDateTime>
#include <traderspi.h>
#include "ctpcontrol.h"
#include "optionController.h"
#include "qOrderManager.h"
#include "asianPrice.h"
#include "bjsmodel.h"
struct instrumentDetail
{
	QString instrumentCode;
	QString lastUpdatetick;
	double tickValue;
	double ratio;
	optionHedgeHands hedgeLots;
	bool updateToBook;
	int hedgeThreshold;
	instrumentDetail(){
	//	memset(this, 0, sizeof(instrumentDetail));  
	};
	void reset(){
	//	memset(this, 0, sizeof(instrumentDetail));  
	}
};
class cashGreeks{
public:
    cashGreeks(){
        _deltaCash = 0;
        _gammaCash = 0;
        _vegaCash = 0;
        _rho = 0;
    }
    ~cashGreeks(){
    
    }
public:   
    // cash
    double _deltaCash;// delta * ��ļ۸�
    // gamma * ��ļ۸�^2 * 100
    double _gammaCash;
    // Theta ������/��
    double _thetaCash;
    // vega * ������ * 100
    double _vegaCash;
    // Rho 1%���޷������ʵı仯�Լ�ֵ������Ӱ��
    double _rho;

};
class QStrategyThread : public QThread
{
public:
	Q_OBJECT

public:
	//friend class QStrategy;
	QStrategyThread(QStandardItemModel*,QOrderManager*,QObject *parent = 0);
	~QStrategyThread();

	void loadConf(QString confName);
	void run();
	void StrategyOperation(int hedgeDifference,int position,CThostFtdcDepthMarketDataField *pDepthMarketData);
	inline void StrategyOptionCalcultate(CThostFtdcDepthMarketDataField *pDepthMarketData);
	void GetTimeTic(TThostFtdcDateType ActionDay,TThostFtdcTimeType UpdateTime,time_t&curTimeTic,time_t&everyDayStratTime);

    void setModelData(QString instrumentCode,optionGreeks CallOptionGreeks,optionGreeks PutOptionGreeks,optionHedgeHands CurHedgeHands,double ratio,double,QString,cashGreeks & greeksCash);
	void PrintLogging(CThostFtdcDepthMarketDataField *pDepthMarketData,optionGreeks callGreeks,optionGreeks putGreeks,optionHedgeHands hedgeHands,optionParam _optionParam);
	void setTDSpi(CtpTraderSpi *spi);
	bool optionChoose(double underlyingPrice ,time_t _curDateTime,optionGreeks &_CallgreeksValue,optionGreeks &_putGreeksValue);
	void carryTime();
	bool valideDate(time_t _settlementDate,time_t _todayDate,time_t _maturityDate);
	double getHands(int scaleUnit,double scale,double tickValue);
	bool timeCalculate(TThostFtdcTimeType,double);
	void StrategyOperationMutil();
	void testFunc();
	double H0(double tau,double lamuda,double vol,double riskA,double gamma,double rate,double underlying);
signals:
	void strategyMsg(QString);
	void subscribe_inst_data(QString pInstrumentId); 
	void dataChange(QModelIndex,QModelIndex);
	void updateToOrderBook(QString strategyName,QString instId,int totalHedgeNum,int hedgeThreshold,int maxOpenNum);

public:
	// ÿ�����Ե�
	QString productName;
	int optionType;
	int scaleUnit ;

	int m_canlendarNTotal;
	int m_tradeNTotal;
	int m_canlendarN;
	int m_tradeN;
	int m_tradeT1N;
	int m_calendarT1N;

	double callScale ;
	double putScale;
	QDateTime settlementDate;
	QDateTime maturityDate;
	QDateTime nextAveragePoint;
	QDateTime firstAveragePoint;
	double m_initialPrice ;
	double averagePrice;
	double m_averagePriceAdjust;

	//QString execPricePercent ;
	double m_putExecPricePercent ;
	double m_putExerPrice;
	double m_callExecPricePercent ;
	double m_callExerPrice;

	double interest;
	double dividend;
	double barrier ;
	double tickValue;
	double tickValue2;
	double tickValue3;
	double tickValue4;
	int callHedgeDirection ;
	int putHedgeDirection ;
	double priceVolability;
	QString margin;
	QString openCommission ;
	QTime strategyStartTime;
	QTime settleTime ;
	int m_openNums;
	QString hedgeLevel ;
	double hedgeVolability ;
	QTime carryNoonTime;
	QTime carryNightTime ;

	int carryNoonDistance ;
	int carryNightDistance;
	int totalAveragePoint;
	int averagedPoint;
	int m_averagedPointAdjust;
	QString instrumentCode ;
	QString instrumentCode2;
	QString instrumentCode3;
	QString instrumentCode4;
	QList<instrumentDetail> m_instrumentList;
	bool m_instrumentStatus;
	double instrumentRatio;
	double instrumentRatio2;
	double instrumentRatio3;
	double instrumentRatio4;

	double m_tau;
	double m_t1;

	double m_nextAverageTime;
	double m_firstAverageTime;
	int hedgeThreshold;

	// ״̬ true ���� false δ����
	bool m_status;
	// ����ʱ����
	int m_spanTime;
	// ���״̬
	bool m_checkStatus;
	// ctp control;
	// option manager
	optionController m_oOptionCalculateEngine;
	// carry ʱ��Tic
	time_t m_nightTime;
	time_t m_noonTime;
	time_t m_noonCloseTime;

	CtpTraderSpi* TDSpi_stgy;//TDָ��
	//����Model
	QStandardItemModel *m_strategyGreekModel;

	//�ǽ���ʱ��
	int m_iNoTradeHour1;
	int m_iNoTradeHour2;
	//��һ��tick����ʱ�� ������һ��tick���µ�tick����ʱ��仯��������
	// �����һ��tick�Ա��µ�����ʱ���� �ޱ仯 ������κν���
	QString m_oldTickUpdate;
	//�����������
	int m_tryInsertOrderTimes;
	//���Դ���
	int m_testTimes;
	//���Թ�����
	QOrderManager *m_pOrderManager;
	//����mode
	optionModel::optionEngine m_optionEngine;
	// deltay
	bool m_isCarry;
	//work Day
	map<string,int> m_workDay;
	map<string,int> m_calendarDay;

	double m_tradeCostPercent;
	double m_riskAversion;

    cashGreeks strategyCash;
};

#endif // QSTRATEGYTHREAD_H
