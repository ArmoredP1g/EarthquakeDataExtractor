#include "DataExtract.h"

DataExtract::DataExtract()
{
	initThreadLoop(16);
	//th_ThreadRecycler.detach();
}

/****************************************
	by Andrew Mao 210715
	唯一对外部函数，将剪切完的
	数据文件输出至目标文件夹
*****************************************/
void DataExtract::Extract(const char *  startTime, const char *  endTime, limitingConditions * limitingConditions, const char *  reportPath, const char *  txtPath, const char *  outputPath)
{
	//激活线程回收器
	thread_Flag = true;
	th_ThreadRecycler = std::thread(&DataExtract::ThreadRecycler, this);
	//先找到需要包含的文件路径
	vector<string>* filelist = getPropFileList(txtPath,startTime,endTime);
	vector<string>* reportDirList = getDirnames(reportPath);

	int fileCounts = filelist->size();
	int reportCounts = reportDirList->size();
	if (fileCounts != 0)
	{
		cout << "数据文件总数:	" << fileCounts << endl;
	}
	else
	{
		cout << "未找到数据文件"<< endl;
		emit BtnAvaliable(true);
		thread_Flag = false;
		th_ThreadRecycler.detach();
		return;
 	}

	if (reportCounts != 0)
	{
		cout << "report文件总数:	" << fileCounts << endl;
	}
	else
	{
		cout << "未找到report文件" << endl;
		emit BtnAvaliable(true);
		thread_Flag = false;
		th_ThreadRecycler.detach();
		return;
	}

	cout << "----------------------RUNNING----------------------" << endl;

	//找到对应的report文件中的时间信息，并截取文件
	for (int iter = 0;iter < filelist->size(); iter++)
	{		
		emit ProgressMove((iter * 100) / fileCounts);

		//寻找空闲线程槽位
		while (loop->status != THREAD_IDLE)
		{
			loop = loop->next;
		}
		loop->status = THREAD_BUSY;
		ThreadLoopNode * temp = loop;
		loop->t = std::thread(&DataExtract::MatchAndCut,this,(*filelist)[iter],outputPath,reportDirList, temp);
	}

	//！！在这需要检测线程是否全部回归空闲(没整)
	//改变主意了，睡眠三秒，啥破电脑也该完事了
	_sleep(3000);
	emit ProgressMove(100);
	emit BtnAvaliable(true);
	thread_Flag = false;
	th_ThreadRecycler.detach();
	delete filelist;
	delete reportDirList;
}

void DataExtract::MatchAndCut(string path, string outputPath, vector<string> * reportDirList, ThreadLoopNode *node)
{
	//找到偏移的行数
	double diff = getDiff(path, reportDirList);

	if (diff != -1)
	{
		CreateFile(path, outputPath, diff);
	}
	node->status = THREAD_WAITING;
}

/****************************************
	by Andrew Mao 210715
	获取路径下所有的文件夹
*****************************************/
vector<string> * DataExtract::getDirnames(string path)
{
	//文件句柄 win10下必须是LONG LONG
	long long hFile = 0;
	struct _finddata_t fileInfo;//包含文件基本信息
	string pathName, exdName;
	vector<string> * result = new vector<string>();//返回结果

	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
	{
		return result;
	}

	do
	{

		//删除前两个没用的 "." ".."的文件 也不知到为啥会有这玩意
		if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
		{
			//判断是文件夹还是文件
			if (fileInfo.attrib&_A_SUBDIR)
			{
				//是文件夹的话直接塞进去
				result->push_back(path+"\\"+fileInfo.name);
			}
			else
			{
				//是文件的话直接跳过，这里只要文件夹
				continue;
			}
		}		
	} while (_findnext(hFile, &fileInfo) == 0);

	_findclose(hFile);
	return result;
}

/****************************************
	by Andrew Mao 210715
	获取路径下所有的文件
	(包括子路径下的)
*****************************************/
vector<string> * DataExtract::getFilenames(string path, bool subdir)
{
	//文件句柄 win10下必须是LONG LONG
	long long hFile = 0;
	struct _finddata_t fileInfo;//包含文件基本信息
	string pathName, exdName;
	vector<string> * result = new vector<string>();//返回结果

	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
	{
		return NULL;
	}

	do
	{
		//删除前两个没用的 "." ".."的文件 也不知到为啥会有这玩意
		if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
		{
			//判断是文件夹还是文件
			if (fileInfo.attrib&_A_SUBDIR && subdir)
			{
				//是文件夹的话递归调用
				vector<string> * temp = getFilenames(path + "\\" + fileInfo.name, true);
				for (string iter : (*temp))
				{
					result->push_back(iter);
				}
				delete temp;
			}
			else if(!(fileInfo.attrib&_A_SUBDIR))
			{
				//是文件的话直接塞进去
				result->push_back(path+"\\"+fileInfo.name);
			}
		}
	
	} while (_findnext(hFile, &fileInfo) == 0);

	_findclose(hFile);
	return result;
}


/****************************************
	by Andrew Mao 210715
	返回所有符合日期要求的文件
	路径列表
*****************************************/
vector<string> * DataExtract::getPropFileList(string path, string startTime, string endTime)
{
	vector<string> * result = new vector<string>();
	vector<string> subdir;
	vector<string> tempFilelist;
	vector<string> * rootdir = getDirnames(path);

	for (string iter: (*rootdir))
	{
		vector<string> filename = split(split(iter,"\\").back(),"-");

		//如果文件夹是1011-1012这种
		if (filename.size() == 2)
		{
			//开始时间或结束时间在文件夹名所包含区间内 或者区间整个都在开始结束中间
			
			if ((atoi(startTime.c_str()) >= atoi(filename[0].c_str()) && atoi(startTime.c_str()) <= atoi(filename[1].c_str())) ||
				(atoi(endTime.c_str()) >= atoi(filename[0].c_str()) && atoi(endTime.c_str()) <= atoi(filename[1].c_str())) ||
				(atoi(startTime.c_str()) <= atoi(filename[0].c_str()) && atoi(endTime.c_str()) >= atoi(filename[1].c_str())))
			{
				subdir.push_back(iter);
			}
		}
		//如果文件夹是1011这种
		else if (filename.size() == 1)
		{
			if (atoi(startTime.c_str()) <= atoi(filename[0].c_str()) && atoi(endTime.c_str()) >= atoi(filename[0].c_str()))
			{
				subdir.push_back(iter);
			}
		}
	}

	for (string iter : subdir)
	{
		//将初选目录下所有文件加载到内存
		vector<string> * temp;
		temp = getFilenames(iter,true);

		vector<string>::iterator i;

		for (i = temp->begin(); i < temp->end(); i++)
		{
			//(-err-)若文件不是txt格式的，或名字最后14位不全是数字的，直接删除
			if (strcmp(i->substr(i->size() - 4, 4).data(), ".txt") != 0 ||
				!AllisNum(i->substr(i->size() - 18, 14)))
			{
				continue;
			}
			//截出4(6)位年月(日)
			string date = i->substr(i->size() - 16, 4);
			//日期也符合要求
			if (atoi(startTime.c_str()) <= atoi(date.c_str()) && atoi(endTime.c_str()) >= atoi(date.c_str()))
			{
				result->push_back(*i);
			}	
		}
		delete temp;
	}

	delete rootdir;
	return result;
}

/****************************************
	by Andrew Mao 210717
	返回该剪切掉的行数
	-!严重内存泄漏!-
*****************************************/
double DataExtract::getDiff(string path, vector<string>* reportDirList)
{
	vector<string> params = split(path,".");
	vector<string> tempPathList;//存放符合要求的文件名路径列表 有09-11-1这种所以可能不止一个

	string date = params[params.size() - 2].substr(2, 4);
	string datetime = params[params.size() - 2];
	string nozonozoparam3 = params[params.size() - 5];
	string province = split(params[params.size() - 6],"\\").back();

	double PgSec = 0;
	string PgDate = "";
	double SgSec = 0;
	string SgDate = "";

	for (string iter:*reportDirList)
	{
		vector<string> dirName = split(iter, "\\");
		vector<string> dirDate = split(dirName[dirName.size()-1],"-");

		//如果文件名日期符合
		if (atoi(date.c_str()) == atoi((dirDate[0] + dirDate[1]).c_str()))
		{
			tempPathList.push_back(iter);
		}
	}

	//若没匹配的文件夹
	if (tempPathList.size() == 0)
		return 0;

	//遍历所有匹配的文件夹下的所有文件，找到对应的时间
	for (string iter : tempPathList)
	{
		vector<string> * files = getFilenames(iter,true);
		for (string file : *(files))
		{
			ifstream fs(file);//文件流
			string line;//存放一行文本内容
			if (!fs.is_open())
			{
				cout << "文件打不开:"<< file.data() << endl;				
				fs.close();
				continue;
			}

			//核查第一行数据完整性，获取时间用来对比
			if (getline(fs, line))
			{
				vector<string> params = split(line,"\t");
				//检查report文件完整性
				if (params[0] != "DBO")
				{
					cout << "文件第一行DBO参数异常:" << file.data() << endl;
					fs.close();
					continue;
				}
				if (params[2].length() != 10 || split(params[2], "-").size() != 3)
				{
					cout << "文件第一行日期参数异常:" << file.data() << endl;
					fs.close();
					continue;
				}
				if (params[3].length() != 11 || split(params[3],":").size() != 3)
				{
					cout << "文件第一行时分秒参数异常:" << file.data() << endl;
					fs.close();
					continue;
				}
				//以上仨没问题就认为文件没错误了

				//核对时间
				vector<string> yymmdd = split(params[2], "-");
				vector<string> hhmmss = split(params[3].substr(0, 8), ":");
				if (datetime != (yymmdd[0] + yymmdd[1] + yymmdd[2] + hhmmss[0] + hhmmss[1] + hhmmss[2]))
				{
					fs.close();
					continue;
				}
				
			}


			while (getline(fs, line))
			{
				vector<string> params = split(line, "\t");

				
				if (params[1] == province && params[2] == nozonozoparam3 && params[4] == "Pg")
				{
					if (params[6] == "NoTime")
					{
						cout << "P波时间为NoTime:" << file.data() << endl;
						continue;
					}
					PgSec = TimeTrans2Sec(params[7]);
					PgDate = params[6];
				}
				if (params[1] == province && params[2] == nozonozoparam3 && params[4] == "Sg")
				{
					if (params[6] == "NoTime")
					{
						cout << "S波时间为NoTime:" <<file.data() << endl;
						continue;
					}
					SgSec = TimeTrans2Sec(params[7]);
					SgDate = params[6];
				}
				//两个时间都获取到了
				if (PgDate != "" && SgDate != "")
				{
					break;
				}
			}

			fs.close();
			//若时间没获取到，直接返回-1
			if (PgDate == "" || SgDate == "")
			{
				cout << "report file: " << file << " PG或者SG其中一个没有" << endl;
				delete files;
				return -1;
			}

			//判断时间跨没跨天
			if (PgDate == SgDate)
			{
				delete files;
				return SgSec - PgSec;
			}
			else
			{
				delete files;
				return SgSec + (86400 - PgSec);
			}

		}
		delete files;
	}

	return -1;
}

/****************************************
	by Andrew Mao 210717
	剪切并生成新文件
*****************************************/
void DataExtract::CreateFile(string path, string outputPath, double diff)
{
	string tt = path;//不这么整一下getline读不进去，啥原理
	ifstream infile(tt);
	ofstream outfile;
	string line = "";
	vector<string> filenames = split(path,".");
	
	//跳过偏移行数
	for (int l = 0; l < diff * 100; l++)
	{
		if (!getline(infile, line))
		{
			cout << "文件不够长:" << path << endl;
			return;
		}
	}

	//没文件夹创建一个
	//0905
	if (_access((outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4)).c_str(), 0) == -1)
	{
		_mkdir((outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4)).c_str());
	}
	//"20090501000404"
	if (_access((outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4)+"\\"+ filenames[filenames.size()-2]).c_str(), 0) == -1)
	{
		_mkdir((outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4) + "\\" + filenames[filenames.size() - 2]).c_str());
	}

	filenames = split(path,"\\");
	string finalpath = (outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4) + "\\" + filenames[filenames.size() - 2] + "\\" + filenames.back());

	//已经有这文件了就跳过了
	if (_access(finalpath.data(),0) != -1)
	{
		infile.close();
		outfile.close();
	}

	outfile = ofstream(finalpath, ios::app);
	while (getline(infile, line))
	{
		outfile << line << endl;
	}

	infile.close();
	outfile.close();
}

void DataExtract::RegroupFiles(string outputPath)
{
	vector<string>* filelist = getFilenames(outputPath, false);
	for (string iter : *filelist)
	{

		rename(iter.data(),iter.data());
	}

}

vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型
	char * strs = new char[str.length() + 1]; //不要忘了
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型
		res.push_back(s); //存入结果数组
		p = strtok(NULL, d);
	}

	delete strs;
	delete d;
	delete p;
	return res;
}

/****************************************
	by Andrew Mao 210715
	看一个字符串里是不是全是数
	
*****************************************/
bool DataExtract::AllisNum(string str)
{
	for (int i = 0; i < str.size(); i++)
	{
		int tmp = (int)str[i];
		if (tmp >= 48 && tmp <= 57)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

/****************************************
	by Andrew Mao 210717
	返回时间戳对应的秒数
*****************************************/
double DataExtract::TimeTrans2Sec(string time)
{
	vector<string> hhmmss = split(time,":");
	return atof(hhmmss[0].c_str()) * 3600 + atof(hhmmss[1].c_str()) * 60 + atof(hhmmss[2].c_str());
}


/****************************************
	by Andrew Mao 210919
	负责循环回收线程
*****************************************/
void DataExtract::ThreadRecycler()
{
	ThreadLoopNode * node = loop;
	while (thread_Flag)
	{
		if (THREAD_WAITING == node->status)
		{
			node->t.join();
			node->status = THREAD_IDLE;
		}
		node = node->next;
	}
	cout << "回收线程退出" << endl;
}

/****************************************
	by Andrew Mao 210919
	初始化线程池
*****************************************/
void DataExtract::initThreadLoop(int workers)
{
	//至少有一个节点
	loop = new ThreadLoopNode;
	ThreadLoopNode * head = loop;
	for (int i = 0;i<workers - 1;i++)
	{
		loop->next = new ThreadLoopNode;
		loop = loop->next;
	}
	//最后把屁股插脑袋上，整成个环儿
	loop->next = head;
}

void DataExtract::Test()
{
	
}

void DataExtract::Test2()
{
	cout << "test2" << endl;
	_sleep(5000);
}

void DataExtract::Test3()
{
	cout << "test3" << endl;
	_sleep(5000);
}

void DataExtract::Test4()
{
	cout << "test4" << endl;
	_sleep(5000);
}