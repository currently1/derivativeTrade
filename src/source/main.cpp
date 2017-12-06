
#define ELPP_THREAD_SAFE
#include "easylogging++.h"

#include "traderqtapp.h"
#include <QtWidgets/QApplication>

#include "logindialog.h"
#include <qtextcodec>
#include <windows.h>
#include "ctpcontrol.h"
#include <qstring.h>
#include <qmap.h>
#include <qreadwritelock.h>
#include <qmutex.h>
#include <atomic>
#include <qatomic.h>
#include <qsemaphore.h>
#include <qmutex.h>


INITIALIZE_EASYLOGGINGPP

HANDLE g_hEvent;

//#if defined(_MSC_VER) && (_MSC_VER >= 1600)
//#pragma execution_character_set("utf8") 
//#endif

QReadWriteLock g_lock;

QMap<QString,CThostFtdcDepthMarketDataField > instrumentMarketData;

QMap<QString,QAtomicInt > gOrderUnfinished;

QMutex g_mutex;

int ConfigureLogger() 
{
	el::Configurations defaultConf;
	defaultConf.setToDefault();
	defaultConf.setGlobally(
		el::ConfigurationType::Format, "%datetime{%Y/%M/%d %H:%m:%s %g} %level%msg from %func at %loc thread=%thread");
	el::Loggers::reconfigureLogger("default", defaultConf);

	return 0;
}



int requestId=0;//请求编号

int main(int argc, char *argv[])
{
	ConfigureLogger();
	int result;
	LOG(INFO) << "Exe start!";
	
	QApplication a(argc, argv);

	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	
	//这个label
	//QLabel *label = new QLabel();
	//label->setText(QString::fromLocal8Bit("你好！"));
	//label->show();

	TraderQtApp w;

	ctpControl ctpThreadControl(&w);

	loginDialog login(&w);

	w.setQuoteModel(ctpThreadControl.createQuoteModel());
	w.setCtpControl(&ctpThreadControl);
	login.setCtpControl(&ctpThreadControl);

	QObject::connect(&ctpThreadControl,SIGNAL(connectCtpQuest(CThostFtdcRspInfoField*)),&login,SLOT(logginRespond(CThostFtdcRspInfoField*)));
	QObject::connect(&ctpThreadControl,SIGNAL(ctpcontrolMsg(CTPSTATUS)),&w,SLOT(onCtpControlMsg(CTPSTATUS)));
	QObject::connect(&ctpThreadControl,SIGNAL(ctpMessage(QString)),&w,SLOT(onStrategyMsg(QString)));
	if(login.exec() == QDialog::Accepted){
	
		w.show();
		w.initConf();
		result=  a.exec();

	}else{
		result =  0;
	}
	
	ctpThreadControl.quit();
	ctpThreadControl.wait();
	while(true){}
	//CloseHandle(g_hEvent);

	return result;

}
