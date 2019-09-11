// LogConvert.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <direct.h>
#include <regex>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::regex_constants;

/*
	原始数据正则表达式格式：
	2019-08-01 19:06:33,883/1/2/3/4/5/6/1/2/0/0/0/114559
	替换后数据正则表达式格式：
	2019-08-01 19:06:33.883,1,2,3,4,5,6,1,2,0,0,0,114559
	*/
//const string strInputDataFmt = "(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2})(\\,)(\\d{3})(\\/)(-?\\d+)(\\/)(-?\\d+)(\\/)(-?\\d+)(\\/)(-?\\d+)(\\/)(-?\\d+)(\\/)(-?\\d+)\(\/)(-?\\d+)(\\/)(-?\\d+)(\\/)(-?\\d+)(\\/)(-?\\d+)(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)";
const string strInputDataFmt = "(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2})(\\,)(\\d{3})(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?(\\/)(-?\\d+)(\\.\\d+)?";
const string fmt = "$1.$3,$5$6,$8$9,$11$12,$14$15,$17$18,$20$21,$23$24,$26$27,$29$30,$32$33,$35$36,$38$39";
//const string fmt = "$1.$3,$5,$7,$9,$11,$13,$15,$17,$19,$21,$23,$25$26,$28";
const string title = "时间,电压V1,电压V2,电压V3,电流I1,电流I2,电流I3,有功功率,无功功率,视在功率,功率因素,容抗,频率";

time_t curtime;

void Log_Data_SubStr(ifstream& infLogFile, string strPathName);
int getVal(int defaultValue);

int main()
{
	string strLogDir;
	string strFilePrefix = "common.";
	string strFileNum;
	string strFileSuffix = "log";
	string strFileFullName = strFilePrefix + strFileNum + strFileSuffix;
	string strOutputFile = "OutputLog\\";
	string strFileSaveName;
	string strCsvSuffix = "csv";

	cout << "===========================================================================\n";
	cout << "                           Log文件自动转换程序\n";
	cout << "===========================================================================\n";
	cout << "    使用说明：输入保存Log文件的保存路径，按回车后自动进行转换；直接按回车建\n";
	cout << "键，确定默认保存路径：Log\\。Log文件的命名方式为：common.X.log，X为以1开始\n";
	cout << "的递增数字。\n";
	cout << "===========================================================================\n";
	cout << "请输入Log文件所在目录（默认路径：Log\\）：";
	getline(cin, strLogDir);
	if (strLogDir.empty()) {
		strLogDir = "Log\\";
	}
	else {
		strLogDir = strLogDir + "\\";
	}
	cout << "请输入Log文件起始编号（默认编号：1）：";
	int uiStartNo = 1;
	uiStartNo = getVal(1);

	//处理common.log
	ifstream in;
	time(&curtime);
	cout << string(ctime(&curtime), 24) << ": " << "正在打开" << strFileFullName << ": \n";
	in.open(strLogDir + strFileFullName, ifstream::in);
	if (in) {
		time(&curtime);
		cout << string(ctime(&curtime), 24) << ": " << strFileFullName << "打开成功！\n";
		_mkdir(strOutputFile.c_str());
		strFileSaveName = strFilePrefix + strFileNum + strCsvSuffix;
		strOutputFile = strOutputFile.substr(0, 11) + strFileSaveName;
		Log_Data_SubStr(in, strOutputFile);
		in.close();
	}
	else {
		time(&curtime);
		cout << string(ctime(&curtime), 24) << ": " << strFileFullName << "不存在！\n";
	}

	//处理common.X.log
	for (int i = uiStartNo; ; ++i) {
		strFileNum = to_string(i) + ".";
		strFileFullName = strFilePrefix + strFileNum + strFileSuffix;
		ifstream inf;
		time(&curtime);
		cout << string(ctime(&curtime), 24) << ": " << "正在打开" << strFileFullName << ": \n";
		inf.open(strLogDir + strFileFullName, ifstream::in);
		if (inf) {
			time(&curtime);
			cout << string(ctime(&curtime), 24) << ": " << strFileFullName << "打开成功！\n";
			strFileSaveName = strFilePrefix + strFileNum + strCsvSuffix;
			strOutputFile = strOutputFile.substr(0, 11) + strFileSaveName;
			Log_Data_SubStr(inf, strOutputFile);
			inf.close();
		}
		else {
			time(&curtime);
			cout << string(ctime(&curtime), 24) << ": " << strFileFullName << "不存在！\n";
			break;
		}
	}
	cout << string(ctime(&curtime), 24) << ": " << "已提取所有Log文件,";
	system("pause");
	return 0;
}

void Log_Data_SubStr(ifstream& infLogFile, string strPathName) {
	string strLogStr;
	ofstream ofsOutputFile;
	time(&curtime);
	cout << string(ctime(&curtime), 24) << ": " << "保存文件：" << strPathName << endl;
	ofsOutputFile.open(strPathName);
	ofsOutputFile << title << endl;
	time(&curtime);
	cout << string(ctime(&curtime), 24) << ": " << "数据提取中...";
	while (getline(infLogFile, strLogStr)) {
		if (strLogStr.compare(0, 6, "[INFO]") == 0) {
			try {
				regex rin(strInputDataFmt);
				smatch rout;
				string strOutputData;
				if (regex_search(strLogStr, rout, rin)) {
					strOutputData = regex_replace(strLogStr, rin, fmt, format_no_copy);
					//cout << string(ctime(&curtime), 24) << ": " << strOutputData << endl;
					ofsOutputFile << strOutputData << endl;
				}
				else {
					time(&curtime);
					cout << endl;
					cout << string(ctime(&curtime), 24) << ": " << "警告：没有匹配的数据!" << flush;
				}
			}
			catch (regex_error e) {
				cout << endl;
				cout << string(ctime(&curtime), 24) << ": " << e.what() << "\ncode: " << e.code() << flush;
			}
		}
		else {
			continue;
		}
		//cout << string(ctime(&curtime), 24) << ": " << strLogStr << endl;
	}
	ofsOutputFile.close();
	time(&curtime);
	cout << endl;
	cout << string(ctime(&curtime), 24) << ": " << "处理完毕。\n";
}


int getVal(int defaultValue)
{
	int val = 0;
	std::thread t1([&]() {
		std::cin >> val;
		if (val != 0)
		{
			//printf("get value:%d\n", val);
			return val;
		}
		});

	std::this_thread::sleep_for(std::chrono::seconds(5));
	t1.detach();

	//if (val == 0)
	//	val = defaultValue;
	return val;
}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
