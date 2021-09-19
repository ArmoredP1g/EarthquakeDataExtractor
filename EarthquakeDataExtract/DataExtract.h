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

#define THREAD_BUSY 0	
#define THREAD_IDLE 1
#define THREAD_WAITING 2	//线程等待被回收

using namespace std;
//定义筛选条件
typedef struct {
	unsigned type = 0;
	double levelUpperLimit = -1;
	double levelLowerLimit = -1;
}limitingConditions;

//环形线程队列结点
typedef struct ThreadLoopNode
{
	int status = THREAD_IDLE;	//空闲标识
	thread t;		//线程对象
	ThreadLoopNode * next = NULL;
}ThreadLoopNode;

vector<string>split(const string& str, const string& delim);

class DataExtract :public QObject
{
	Q_OBJECT
public:
	DataExtract();
	void initThreadLoop(int workers);
	//vector<string>split(const string& str, const string& delim);
public slots:
	void Extract(const char * startTime, const char *  endTime,limitingConditions * limitingConditions, const char *  reportPath, const char *  txtPath, const char *  outputPath);
	void Test();
	void Test2();
	void Test3();
	void Test4();
private:
	ThreadLoopNode * loop;//环形线程队列
	void ThreadRecycler();//循环回收线程
	bool thread_Flag = false;
	std::thread th_ThreadRecycler;

	void MatchAndCut(string path, string outputPath, vector<string> * reportDirList, ThreadLoopNode *node);//匹配参数说明文件找到对应diff并对数据剪切输出
	vector<string> * getDirnames(string path);
	vector<string> * getFilenames(string path, bool subdir);
	vector<string> * getPropFileList(string path, string startTime, string endTime);//获取在区间内所有涉及的数据文件的路径列表
	double getDiff(string path, vector<string>* reportDirList);//按照文件名找对应的report文件里的参数，获取应该剪切掉的行数
	void CreateFile(string path, string outputPath, double diff);
	void RegroupFiles(string outputPath);//生成完的文件按日期分到各个文件夹
	bool AllisNum(string str);
	double TimeTrans2Sec(string time);//把hh:mm:ss.ms格式时间转换成纯秒

	

signals:
	void ProgressMove(int percent);
	void BtnAvaliable(bool b);
};

