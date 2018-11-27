#include "stdafx.h"
#include<iostream>
#include <string>
#include<fstream>
using namespace std;

ifstream fin;
ofstream fout;
//测试文件 test  要求文件 pragram
char *inFileName = "code.txt";
char *outFileName = "result.txt";

#define TABLE_COLUMN 12  //列
#define TABLE_ROW 13	 //行
int posLine = 1;//记录行号
int errors = 1;//记录错误数
string str_temp = " ";

//keywords
string keyWord[8] = {
	"if", "else", "for", "while", "do", "int", "write", "read"
};

//定义各种状态
typedef enum {
	S0,  //开始状态
	S1,S2,S3,S4,S5,S6,//中间状态
	S7,S8,S9,S10,S11,
	ERR// 错误状态
} STATE;

//状态转换表
int transform_table[][12] = {
	//		字母	数字	0		分隔符  +-       /		 *		 !		 =		><		other（包含空格） 
	{ -1,	'a',	1,		0,		'(',	'+',	'/',	'*',	'!',	'=',	'>',	'o' },
	{ S0,	S1,		S2,		S3,		S4,		S6,		S8,		S6,	    S5,		S7,		S7 ,	ERR },
	{ S1,	S1,		S1,		S1,		ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//终
	{ S2,	ERR,	S2,		S2,		ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//终
	{ S3,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//终
	{ S4,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//终
	{ S5,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	S6,		ERR,	ERR },
	{ S6,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//终
	{ S7,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	S6,		ERR,	ERR },//终
	{ S8,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	S9,		ERR,	ERR,	ERR,	ERR },//终
	{ S9,	S9,		S9,		S9,		S9,		S9,		S9,		S10,	S9,		S9,		S9 ,	S9 },
	{ S10,	S9,		S9,		S9,		S9,		S9,		S11,	S10,	S9,		S9,		S9 ,	S9 },
	{ S11,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//终
};

//状态注释
char *state_class[100];
void init_state_class() {
	state_class[S1] = "ID";
	state_class[S2] = "NUM";
	state_class[S3] = "NUM";
	state_class[S4] = "Delimiter";
	//state_class[S5] = "!";
	state_class[S6] = "Operator";
	state_class[S7] = "Operator";
	state_class[S8] = "Operator";
	//state_class[S9] = "注释";
	//state_class[S10] = "*";
	state_class[S11] = "注释";
}

//定义结束状态
STATE end_state[] = {
	S1,S2,S3,S4,S6,S7,S8,S11,
};
int end_state_len = 8;

//将输入的数字转化为 table的 第几列
int get_column(char ch) {
	if (ch >= '1' && ch <= '9') {
		return 2;//字母		
	}
	else if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
		return 1;//数字
	}
	else if (ch == '0') {
		return 3;//0
	}
	else if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ';' || ch == ',') {
		return 4;//分隔符
	}
	else if (ch == '+' || ch == '-') {
		return 5;//+-
	}
	else if (ch == '/') {
		return 6;// /	
	}
	else if (ch == '*') {
		return 7;//*	
	}
	else if (ch == '!') {
		return 8;//!
	}
	else if (ch == '=') {
		return 9;//=	
	}
	else if (ch == '<' || ch == '>') {
		return 10;//><
	}
	else
	{
		return 11;//other
	}
}

//是否在终态中
int in_end_state(STATE now) {
	for (int i = 0; i < end_state_len; i++) {
		if (end_state[i] == now) {
			return 1; // 在终态集合中
		}
	}
	return 0; // 不在终态集合中
}

//状态转换  输入当前状态 和下一个字符  输出到达的状态
STATE jump(STATE now, char c) {
	STATE new_state;
	// 求出当前状态行号
	int row_idx = 0;
	for (int i = 1; i < TABLE_ROW; i++) {
		if (now == transform_table[i][0]) {
			row_idx = i;
			break;
		}
	}
	if (row_idx == 0)
	{
		return ERR;
	}
	//求出字符属于第几列
	int col_idx = get_column(c);
	new_state = (STATE)transform_table[row_idx][col_idx];
	return new_state;
}

//输出第star到end的字符
string changeToStr(char *str, int start, int end) {
	string s ="";
	for (int i = start; i <= end; i++) {
		s+= str[i];		
		
	}
	//cout << s;
	return s;
}

//判断是否为关键词
int isKeywords(string str)
{
	for (int i=0;i<8;i++)
	{
		if (str == keyWord[i])
		{
			return i;
		}
	}
	return -1;
}

void analyseWord(char *line) {
	STATE now = S0;//初始状态
	int index = 0; //记录当前字符的位置  便于输出
	int len = strlen(line);
	for (int i = 0; i < len; i++) {
		char c = line[i];
		//空格跳过
		if (c == ' ' || c == '\n' || c == '\t') {
			if (index == i) {
				index++;
			}
			continue; //读取下一个字符
		}
		now = jump(now, line[i]);
							 
		//对于一个新状态的处理  存放正确字符  或者  报错  备注
	/*	if (jump(now, line[i + 1]) == ERR && get_column(line[i + 1])==11&& line[i + 1]!=' '&&line[i + 1] != '\0')
		{
			cout << "error:"<<"|"<< line[i + 1]<<"|"<< endl;

		}*/

		if (in_end_state(now) && jump(now, line[i+1]) == ERR) {//当前字符在终态（accept）  输入下一个字符状态为ERR
			string str = changeToStr(line, index, i);
			
			//判断是不是keywords
			if (isKeywords(str)>=0)
			{	
				fout << keyWord[isKeywords(str)] << str_temp;
				//cout << "\t  S" << now << "\tkeyWord  " << keyWord[isKeywords(str)] << endl;
				fout << str << str_temp  << endl;//存放到文件中 << posLine
			}
			else if(now != S11)
			{
				if (now != S1&& now != S2&& now != S3)
				{
					fout << str << str_temp << str << str_temp << endl;//存放到文件中  << posLine
				}
				else
				{
					fout << state_class[now] << str_temp;
					//cout << "\t  S" << now << "\t" << state_class[now] << endl;
					fout << str << str_temp  << endl;//存放到文件中 << posLine
				}

			}
			
			//回到初始态
			now = S0;
			index = i + 1;
			
		}
		//不可以接受 当前在中间状态 且下一个状态为err 表示无路可走,当前字符不合法
		else if (!in_end_state(now)&&jump(now, line[i + 1]) == ERR)
		{
			cout <<"errors:"<<errors++<<"  line:"<< posLine<< "\t未知符号或不合法字符： " << line[i] << endl;
			now = S0;
			index = i + 1;
		}
		//解决注释不全问题   最后一个字符的下一个字符是字符结尾 \0  当前字符的状态不是终态 
		else if (line[i+1] =='\0' && !in_end_state(now))
		{
			cout << "errors:" << errors++ << "  line:" << posLine << "\t注释不全: " << changeToStr(line, index, i) << endl;
		}

	}

}


int main() {	
	//打开文件
	fin.open(inFileName,ios::in);
	fout.open(outFileName, ios::out);
	cout << "要编译的原文件为："<<inFileName<<endl;
	cout << "编译完成的文件为：" << outFileName << endl << endl;
	char buffer[256];
	init_state_class();
	while (!fin.eof()) {
		fin.getline(buffer, 256, '\n');//读取一行
		analyseWord(buffer);
		posLine++;
	}
	cout << "编译结束：发现" << errors-1<< "处错误" << endl << endl;

	//关闭文件
	fin.close();
	fout.close();
	system("pause");
	return 0;
}

void lexical_analysis()
{
	fin.open(inFileName, ios::in);
	fout.open(outFileName, ios::out);
	cout << "要编译的原文件为：" << inFileName << endl;
	cout << "编译完成的文件为：" << outFileName << endl << endl;
	char buffer[256];
	init_state_class();
	while (!fin.eof()) {
		fin.getline(buffer, 256, '\n');//读取一行
		analyseWord(buffer);
		posLine++;
	}
	cout << "编译结束：发现" << errors - 1 << "处错误" << endl << endl;

	//关闭文件
	fin.close();
	fout.close();
}
