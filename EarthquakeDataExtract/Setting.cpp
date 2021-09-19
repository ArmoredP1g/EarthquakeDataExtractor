#include"Setting.h"

Setting::Setting()
{
	ifstream ifs(".\\settings.cfg");
	string buffer;
	while (getline(ifs, buffer))
	{
		vector<string> keyPair = split(buffer, ":");
		if (keyPair[0] == "Thread_Num")
		{
			thread_num = atoi(keyPair[1].c_str());
		}
		if (keyPair[0] == "ReportPath")
		{
			reportPath = keyPair[1].c_str();
		}
		if (keyPair[0] == "TxtPath")
		{
			txtPath = keyPair[1].c_str();
		}
		if (keyPair[0] == "OutputPath")
		{
			outputPath = keyPair[1].c_str();
		}
	}
	ifs.close();
}

void Setting::ModifySettings()
{
	//ÀÁµÃ×öÁË
}