#include "DataExtract.h"
/****************************************
	by Andrew Mao 210715
	Ψһ���ⲿ���������������
	�����ļ������Ŀ���ļ���
*****************************************/
void DataExtract::Extract(const char *  startTime, const char *  endTime, limitingConditions * limitingConditions, const char *  reportPath, const char *  txtPath, const char *  outputPath)
{
	//qDebug() << "file reader functon ID:" << QThread::currentThreadId();
	//���ҵ���Ҫ�������ļ�·��
	vector<string>* filelist = getPropFileList(txtPath,startTime,endTime);
	vector<string>* reportDirList = getDirnames(reportPath);
	//�ҵ���Ӧ��report�ļ��е�ʱ����Ϣ������ȡ�ļ�
	for (int iter = 0;iter < filelist->size(); iter++)
	{	

		//cout << iter << endl;
		emit ProgressMove((int(double(iter+1 * 100)) / double(filelist->size())));
		//�ҵ�ƫ�Ƶ�����
		double diff = getDiff((*filelist)[iter], reportPath, reportDirList);

		if (diff != -1)
		{
			CreateFile((*filelist)[iter],outputPath,diff);
		}
		else
		{
			continue;
		}
	}
	qDebug() << QString::fromLocal8Bit("*********************************************")<< endl;
	qDebug() << QString::fromLocal8Bit("*********************������******************") << endl;
	qDebug() << QString::fromLocal8Bit("*********************************************") << endl;
	delete filelist;
	delete reportDirList;
}



/****************************************
	by Andrew Mao 210715
	��ȡ·�������е��ļ���
*****************************************/
vector<string> * DataExtract::getDirnames(string path)
{
	//�ļ���� win10�±�����LONG LONG
	long long hFile = 0;
	struct _finddata_t fileInfo;//�����ļ�������Ϣ
	string pathName, exdName;
	vector<string> * result = new vector<string>();//���ؽ��

	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
	{
		return NULL;
	}

	do
	{

		//ɾ��ǰ����û�õ� "." ".."���ļ� Ҳ��֪��Ϊɶ����������
		if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
		{
			//�ж����ļ��л����ļ�
			if (fileInfo.attrib&_A_SUBDIR)
			{
				//���ļ��еĻ�ֱ������ȥ
				result->push_back(path+"\\"+fileInfo.name);
			}
			else
			{
				//���ļ��Ļ�ֱ������������ֻҪ�ļ���
				continue;
			}
		}		
	} while (_findnext(hFile, &fileInfo) == 0);

	_findclose(hFile);
	return result;
}

/****************************************
	by Andrew Mao 210715
	��ȡ·�������е��ļ�
	(������·���µ�)
*****************************************/
vector<string> * DataExtract::getFilenames(string path)
{
	//�ļ���� win10�±�����LONG LONG
	long long hFile = 0;
	struct _finddata_t fileInfo;//�����ļ�������Ϣ
	string pathName, exdName;
	vector<string> * result = new vector<string>();//���ؽ��

	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
	{
		return NULL;
	}

	do
	{
		//ɾ��ǰ����û�õ� "." ".."���ļ� Ҳ��֪��Ϊɶ����������
		if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
		{
			//�ж����ļ��л����ļ�
			if (fileInfo.attrib&_A_SUBDIR)
			{
				//���ļ��еĻ��ݹ����
				vector<string> * temp = getFilenames(path + "\\" + fileInfo.name);
				for (string iter : (*temp))
				{
					result->push_back(iter);
				}
				delete temp;
			}
			else
			{
				//���ļ��Ļ�ֱ������ȥ
				result->push_back(path+"\\"+fileInfo.name);
			}
		}
	
	} while (_findnext(hFile, &fileInfo) == 0);

	_findclose(hFile);
	return result;
}


/****************************************
	by Andrew Mao 210715
	�������з�������Ҫ����ļ�
	·���б�
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

		//����ļ�����1011-1012����
		if (filename.size() == 2)
		{
			//��ʼʱ������ʱ�����ļ����������������� ���������������ڿ�ʼ�����м�
			
			if ((atoi(startTime.c_str()) >= atoi(filename[0].c_str()) && atoi(startTime.c_str()) <= atoi(filename[1].c_str())) ||
				(atoi(endTime.c_str()) >= atoi(filename[0].c_str()) && atoi(endTime.c_str()) <= atoi(filename[1].c_str())) ||
				(atoi(startTime.c_str()) <= atoi(filename[0].c_str()) && atoi(endTime.c_str()) >= atoi(filename[1].c_str())))
			{
				subdir.push_back(iter);
			}
		}
		//����ļ�����1011����
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
		//����ѡĿ¼�������ļ����ص��ڴ�
		vector<string> * temp;
		temp = getFilenames(iter);

		vector<string>::iterator i;

		for (i = temp->begin(); i < temp->end(); i++)
		{
			//(-err-)���ļ�����txt��ʽ�ģ����������14λ��ȫ�����ֵģ�ֱ��ɾ��
			if (strcmp(i->substr(i->size() - 4, 4).data(), ".txt") != 0 ||
				!AllisNum(i->substr(i->size() - 18, 14)))
			{
				continue;
			}
			//�س�4(6)λ����(��)
			string date = i->substr(i->size() - 16, 4);
			//����Ҳ����Ҫ��
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
	���ظü��е�������
	-!�����ڴ�й©!-
*****************************************/
double DataExtract::getDiff(string path, string reportPath, vector<string>* reportDirList)
{
	vector<string> params = split(path,".");
	vector<string> tempPathList;//��ŷ���Ҫ����ļ���·���б� ��09-11-1�������Կ��ܲ�ֹһ��

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

		//����ļ������ڷ���
		if (atoi(date.c_str()) == atoi((dirDate[0] + dirDate[1]).c_str()))
		{
			tempPathList.push_back(iter);
		}
	}

	//��ûƥ����ļ���
	if (tempPathList.size() == 0)
		return 0;

	//��������ƥ����ļ����µ������ļ����ҵ���Ӧ��ʱ��
	for (string iter : tempPathList)
	{
		vector<string> * files = getFilenames(iter);
		for (string file : *(files))
		{
			//�ڴ�й©������
			ifstream fs(file);//�ļ���
			string line;//���һ���ı�����
			if (!fs.is_open())
			{
				qDebug() << QString::fromLocal8Bit("�ļ��򲻿�:")<< QString(file.data()) << endl;				
				fs.close();
				continue;
			}

			//�˲��һ�����������ԣ���ȡʱ�������Ա�
			if (getline(fs, line))
			{
				vector<string> params = split(line,"\t");
				//���report�ļ�������
				if (params[0] != "DBO")
				{
					qDebug() << QString::fromLocal8Bit("�ļ���һ��DBO�����쳣:") << QString(file.data()) << endl;
					fs.close();
					continue;
				}
				if (params[2].length() != 10 || split(params[2], "-").size() != 3)
				{
					qDebug() << QString::fromLocal8Bit("�ļ���һ�����ڲ����쳣:") << QString(file.data()) << endl;
					fs.close();
					continue;
				}
				if (params[3].length() != 11 || split(params[3],":").size() != 3)
				{
					qDebug() << QString::fromLocal8Bit("�ļ���һ��ʱ��������쳣:") << QString(file.data()) << endl;
					fs.close();
					continue;
				}
				//������û�������Ϊ�ļ�û������

				//�˶�ʱ��
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
						qDebug() << QString::fromLocal8Bit("P��ʱ��ΪNoTime:") << QString(file.data()) << endl;
						continue;
					}
					PgSec = TimeTrans2Sec(params[7]);
					PgDate = params[6];
				}
				if (params[1] == province && params[2] == nozonozoparam3 && params[4] == "Sg")
				{
					if (params[6] == "NoTime")
					{
						qDebug() << QString::fromLocal8Bit("S��ʱ��ΪNoTime:") << QString(file.data()) << endl;
						continue;
					}
					SgSec = TimeTrans2Sec(params[7]);
					SgDate = params[6];
				}
				//����ʱ�䶼��ȡ����
				if (PgDate != "" && SgDate != "")
				{
					break;
				}
			}

			fs.close();

			//�ж�ʱ���û����
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
	���в��������ļ�
*****************************************/
void DataExtract::CreateFile(string path, string outputPath, double diff)
{
	string tt = path;//����ô��һ��getline������ȥ��ɶԭ��
	ifstream infile(tt);
	ofstream outfile;
	string line = "";
	vector<string> filenames = split(path,".");
	
	//����ƫ������
	for (int l = 0; l < diff * 100; l++)
	{
		istream &a = getline(infile, line);
	}

	//û�ļ��д���һ��
	if (_access((outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4)).c_str(), 0) == -1)
	{
		_mkdir((outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4)).c_str());
	}

	filenames = split(path,"\\");
	string finalpath = (outputPath + "\\" + filenames[filenames.size() - 2].substr(2, 4) + "\\(cutted)" + filenames.back());

	//�Ѿ������ļ��˾�������
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


vector<string> DataExtract::split(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����
	char * strs = new char[str.length() + 1]; //��Ҫ����
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //�ָ�õ����ַ���ת��Ϊstring����
		res.push_back(s); //����������
		p = strtok(NULL, d);
	}

	delete strs;
	delete d;
	delete p;
	return res;
}

/****************************************
	by Andrew Mao 210715
	��һ���ַ������ǲ���ȫ����
	
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
	����ʱ�����Ӧ������
*****************************************/
double DataExtract::TimeTrans2Sec(string time)
{
	vector<string> hhmmss = split(time,":");
	return atof(hhmmss[0].c_str()) * 3600 + atof(hhmmss[1].c_str()) * 60 + atof(hhmmss[2].c_str());
}

