#ifndef QSTRATEGY_H
#define QSTRATEGY_H

#include <QWidget>
#include <qdialog.h>
#include "ui_qstrategy.h"
#include <qstandarditemmodel.h>
#include "qstrategythread.h"
#include <qlist.h>
#include <qvector.h>
#include <qmap.h>
#include <qthread.h>
#include "ctpcontrol.h"
#include "qOrderManager.h"
class QStrategy : public QDialog
{
	Q_OBJECT

public:
	QMap<QString,QStrategyThread*> m_pStrategyList;
	QStrategy(QWidget *parent,QStandardItemModel *m_OrderManageModel);
	~QStrategy();
	void loadTemplate(int row);
	void checkProductList();
	void freshModel();
	void freshStrategy();
	QStandardItemModel * createModel();
	void setCtpControl(ctpControl*);
	QStandardItemModel * createStrategyGreekModel();
	QOrderManager * getOrderManager();
	void onRunClicked(bool allowTrade,int spanTime);
	void onStopClicked();
	void onDelClicked();
	void getBookInstrumentList(QString bookName);
	protected slots:
		void on_save_clicked();
		void on_cancle_clicked();
		void on_searchBook_clicked();
		// 通过tradepp 绑定 下面函数接受信号

		void subscribe_inst_data(QString pInstrumentId);
		void dataChange(QModelIndex lefttop,QModelIndex bottomRight);
private:

	Ui::QStrategy ui;
	ctpControl *m_pCtpControl;
	QStandardItemModel * m_strategy_model;
	//edit 标志位
	bool edit;
	//策略Model
	QStandardItemModel *m_strategyGreekModel;
	QThread m_strategyManagerThread;
	QOrderManager *m_pOrderManager;
	vector<string> m_instumentList;

	QString m_productName ;


	int m_optionType; // combox是从0开始计算的
	int m_scaleUnit ; 
	QString m_callScale; 
	QString m_putScale ; 
	QDateTime m_settlementDate;
	QDateTime m_maturityDate;
	QDateTime m_nextAveragePoint;
	QDateTime m_firstAveragePoint;
	QString m_initialPrice;
	QString m_averagePrice;
	QString m_callExecPricePercent;
	QString m_putExecPricePercent;
	QString m_interest;
	QString m_dividend;
	QString m_barrier;

	int m_callHedgeDirection;
	int m_putHedgeDirection ;
	
	QTime m_strategyStartTime;

	QString m_openNums;
	QString m_hedgeLevel ;
	QString m_hedgeVolability ;
	QTime m_carryNoonTime ;
	QTime m_carryNightTime;
	int m_carryNoonDistance; 
	int m_carryNightDistance;

	int m_totalAveragePoint ; 
	int m_averagedPoint ; 

	QString m_instrumentCode ; 
	QString m_instrumentCode2 ; 
	QString m_instrumentCode3 ; 
	QString m_instrumentCode4 ; 

	double m_instrumentRatio ;; 
	double m_instrumentRatio2; 
	double m_instrumentRatio3; 
	double m_instrumentRatio4; 

	int m_hedgeThreshold; 

	double m_tickValue;
	double m_tickValue2;;
	double m_tickValue3;
	double m_tickValue4;

};

#endif // QSTRATEGY_H
