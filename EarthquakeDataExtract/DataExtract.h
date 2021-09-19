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

#define THREAD_BUSY 0	
#define THREAD_IDLE 1
#define THREAD_WAITING 2	//�̵߳ȴ�������

using namespace std;
//����ɸѡ����
typedef struct {
	unsigned type = 0;
	double levelUpperLimit = -1;
	double levelLowerLimit = -1;
}limitingConditions;

//�����̶߳��н��
typedef struct ThreadLoopNode
{
	int status = THREAD_IDLE;	//���б�ʶ
	thread t;		//�̶߳���
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
	ThreadLoopNode * loop;//�����̶߳���
	void ThreadRecycler();//ѭ�������߳�
	bool thread_Flag = false;
	std::thread th_ThreadRecycler;

	void MatchAndCut(string path, string outputPath, vector<string> * reportDirList, ThreadLoopNode *node);//ƥ�����˵���ļ��ҵ���Ӧdiff�������ݼ������
	vector<string> * getDirnames(string path);
	vector<string> * getFilenames(string path, bool subdir);
	vector<string> * getPropFileList(string path, string startTime, string endTime);//��ȡ�������������漰�������ļ���·���б�
	double getDiff(string path, vector<string>* reportDirList);//�����ļ����Ҷ�Ӧ��report�ļ���Ĳ�������ȡӦ�ü��е�������
	void CreateFile(string path, string outputPath, double diff);
	void RegroupFiles(string outputPath);//��������ļ������ڷֵ������ļ���
	bool AllisNum(string str);
	double TimeTrans2Sec(string time);//��hh:mm:ss.ms��ʽʱ��ת���ɴ���

	

signals:
	void ProgressMove(int percent);
	void BtnAvaliable(bool b);
};

