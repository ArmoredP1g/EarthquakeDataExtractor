#pragma once
#include <QObject>
#include <iostream>
#include <vector>
#include <string>//���������û��cout string
#include <io.h>
#include <direct.h>
#include <time.h>
#include <fstream> 
#include <qthread>
#include <QDebug>

using namespace std;
//����ɸѡ����
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
	vector<string> * getPropFileList(string path, string startTime, string endTime);//��ȡ�������������漰�������ļ���·���б�
	//void cutFile(string path, int diff);//���м���
	double getDiff(string path,string reportPath, vector<string>* reportDirList);//�����ļ����Ҷ�Ӧ��report�ļ���Ĳ�������ȡӦ�ü��е�������
	void CreateFile(string path, string outputPath, double diff);
	vector<string>split(const string& str, const string& delim);
	bool AllisNum(string str);
	double TimeTrans2Sec(string time);//��hh:mm:ss.ms��ʽʱ��ת���ɴ���

signals:
	void ProgressMove(int percent);
	
};

