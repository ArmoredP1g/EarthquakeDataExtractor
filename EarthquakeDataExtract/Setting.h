#pragma once
#include<qobject>
#include<iostream>
#include"DataExtract.h"
using namespace std;

class Setting : public QObject
{
	Q_OBJECT
public:
	Setting();
	void ModifySettings();
	unsigned int thread_num = 2;
	string reportPath;
	string txtPath;
	string outputPath;
};
