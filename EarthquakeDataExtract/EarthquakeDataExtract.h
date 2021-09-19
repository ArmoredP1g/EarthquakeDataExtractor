#pragma once

#include <QtWidgets/QWidget>
#include <QThread>
#include "ui_EarthquakeDataExtract.h"
#include "DataExtract.h"
#include "Setting.h"
using namespace std;

class EarthquakeDataExtract : public QWidget
{
    Q_OBJECT

public:
    EarthquakeDataExtract(QWidget *parent = Q_NULLPTR);


private:
    Ui::EarthquakeDataExtractClass ui;
	//各种路径
	string txtPath;
	string reportPath;
	string outputPath;

	//间隔日期
	string startDate;
	string endDate;
	limitingConditions limit;
	DataExtract extractor;

	//线程管理
	QThread *threadManage;
	//设置
	Setting * settings;
signals:
	void thread_Extract(const char *  startTime, const char *  endTime, limitingConditions * limitingConditions, const char *  reportPath, const char *  txtPath, const char *  outputPath);
private slots:
	void DataExtract();
};
