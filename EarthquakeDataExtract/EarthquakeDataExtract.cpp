#include "EarthquakeDataExtract.h"

EarthquakeDataExtract::EarthquakeDataExtract(QWidget *parent)
    : QWidget(parent)
{
	settings = new Setting();
    ui.setupUi(this);
	threadManage = new QThread;
	extractor.moveToThread(threadManage);
	threadManage->start();
	ui.progressBar->reset();
	ui.tb_settings->setDisabled(true);
	ui.pb_setOutputPath->setDisabled(true);
	ui.pb_setReportPath->setDisabled(true);
	ui.pb_setTxtPath->setDisabled(true);
	ui.le_txtPath->setText(QString::fromLocal8Bit(settings->txtPath.c_str()));
	ui.le_startDate->setText(QString::fromLocal8Bit("0905"));
	ui.le_endDate->setText(QString::fromLocal8Bit("0905"));
	ui.le_outputPath->setText(QString::fromLocal8Bit(settings->outputPath.c_str()));
	ui.le_reportPath->setText(QString::fromLocal8Bit(settings->reportPath.c_str()));
	
	connect(ui.tb_run, &QToolButton::clicked, this, &EarthquakeDataExtract::DataExtract);
	connect(&extractor, &DataExtract::BtnAvaliable, ui.tb_run, &QToolButton::setEnabled);
	connect(this, &EarthquakeDataExtract::thread_Extract, &extractor,&DataExtract::Extract);
	connect(&extractor, &DataExtract::ProgressMove, ui.progressBar, &QProgressBar::setValue);
	connect(ui.tb_errorStatusButton, &QToolButton::clicked, &extractor, &DataExtract::Test);
}


void EarthquakeDataExtract::DataExtract()
{
	//用于处理QString里的汉字
	txtPath = string(ui.le_txtPath->text().toLocal8Bit());
	reportPath = string(ui.le_reportPath->text().toLocal8Bit());
	outputPath = string(ui.le_outputPath->text().toLocal8Bit());
	startDate = string(ui.le_startDate->text().toLocal8Bit());
	endDate = string(ui.le_endDate->text().toLocal8Bit());
	ui.tb_run->setEnabled(false);
	emit thread_Extract(startDate.data(), endDate.data(), &limit, reportPath.data(), txtPath.data(), outputPath.data());
}

