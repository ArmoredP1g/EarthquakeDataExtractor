#include "EarthquakeDataExtract.h"

EarthquakeDataExtract::EarthquakeDataExtract(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	threadManage = new QThread;
	extractor.moveToThread(threadManage);
	threadManage->start();

	ui.progressBar->reset();
	ui.tb_settings->setDisabled(true);
	ui.pb_setOutputPath->setDisabled(true);
	ui.pb_setReportPath->setDisabled(true);
	ui.pb_setTxtPath->setDisabled(true);
	ui.le_txtPath->setText(QString::fromLocal8Bit("E:\\EARTHQUAKE\\地震数据"));
	ui.le_startDate->setText(QString::fromLocal8Bit("0905"));
	ui.le_endDate->setText(QString::fromLocal8Bit("0906"));
	ui.le_outputPath->setText(QString::fromLocal8Bit("E:\\EARTHQUAKE\\output"));
	ui.le_reportPath->setText(QString::fromLocal8Bit("E:\\EARTHQUAKE\\finalReport\\finalReport"));
	
	connect(ui.tb_run, &QToolButton::clicked, this, &EarthquakeDataExtract::DataExtract);
	connect(this, &EarthquakeDataExtract::thread_Extract, &extractor,&DataExtract::Extract);
	connect(&extractor, &DataExtract::ProgressMove, ui.progressBar, &QProgressBar::setValue);
}


void EarthquakeDataExtract::DataExtract()
{
	//用于处理QString里的汉字
	txtPath = string(ui.le_txtPath->text().toLocal8Bit());
	reportPath = string(ui.le_reportPath->text().toLocal8Bit());
	outputPath = string(ui.le_outputPath->text().toLocal8Bit());
	startDate = string(ui.le_startDate->text().toLocal8Bit());
	endDate = string(ui.le_endDate->text().toLocal8Bit());
	emit thread_Extract(startDate.data(), endDate.data(), &limit, reportPath.data(), txtPath.data(), outputPath.data());
	qDebug() << "ui function ID:" << QThread::currentThreadId();
}

