#pragma once
#include <QObject>
#include <iostream>
#include <vector>
#include <string>//不包含这个没法cout string
#include <io.h>
#include <direct.h>
#include <time.h>
#include <fstream> 
#include <qthread>
#include <QDebug>

using namespace std;
//定义筛选条件
typedef struct {
	unsigned type = 0;
	double levelUpperLimit = -1;
	double levelLowerLimit = -1;
}limitingConditions;

class DataExtract :public QObject
{
	Q_OBJECT
public slots:
	void Extract(const char * startTime, const char *  endTime,limitingConditions * limitingConditions, const char *  reportPath, const char *  txtPath, const char *  outputPath);
private:
	vector<string> * getDirnames(string path);
	vector<string> * getFilenames(string path);
	vector<string> * getPropFileList(string path, string startTime, string endTime);//获取在区间内所有涉及的数据文件的路径列表
	//void cutFile(string path, int diff);//按行剪切
	double getDiff(string path,string reportPath, vector<string>* reportDirList);//按照文件名找对应的report文件里的参数，获取应该剪切掉的行数
	void CreateFile(string path, string outputPath, double diff);
	vector<string>split(const string& str, const string& delim);
	bool AllisNum(string str);
	double TimeTrans2Sec(string time);//把hh:mm:ss.ms格式时间转换成纯秒

signals:
	void ProgressMove(int percent);
	
};

