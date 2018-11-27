// experiment2LL1.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include<iostream>
#include <string>
#include <string.h>
#include<fstream>
#include<map>
#include<vector>
#include<stack>
#include <iomanip>
using namespace std;


//********************************************************** 词法分析 *************************************************************
ifstream fin;
ofstream fout;
//测试文件 test  要求文件 pragram
char *inFileName = "pragram.txt";
char *outFileName = "pragram1_result.txt";

#define TABLE_COLUMN 12  //列
#define TABLE_ROW 13	 //行
int posLine = 1;//记录行号
int errors = 1;//记录错误数

			   //keywords
string keyWord[8] = {
	"if", "else", "for", "while", "do", "int", "write", "read"
};

//定义各种状态
typedef enum {
	S0,  //开始状态
	S1, S2, S3, S4, S5, S6,//中间状态
	S7, S8, S9, S10, S11,
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
	string s = "";
	for (int i = start; i <= end; i++) {
		s += str[i];

	}
	//cout << s;
	return s;
}

//判断是否为关键词
int isKeywords(string str)
{
	for (int i = 0; i<8; i++)
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

		if (in_end_state(now) && jump(now, line[i + 1]) == ERR) {//当前字符在终态（accept）  输入下一个字符状态为ERR
			string str = changeToStr(line, index, i);

			//判断是不是keywords
			if (isKeywords(str) >= 0)
			{
				fout << keyWord[isKeywords(str)] << '\t';
				//cout << "\t  S" << now << "\tkeyWord  " << keyWord[isKeywords(str)] << endl;
				fout << str << '\t' << posLine << endl;//存放到文件中
			}
			else if (now != S11)
			{
				if (now != S1&& now != S2&& now != S3)
				{
					fout << str << '\t' << str << '\t' << posLine << endl;//存放到文件中
				}
				else
				{
					fout << state_class[now] << '\t';
					//cout << "\t  S" << now << "\t" << state_class[now] << endl;
					fout << str << '\t' << posLine << endl;//存放到文件中
				}

			}

			//回到初始态
			now = S0;
			index = i + 1;

		}
		//不可以接受 当前在中间状态 且下一个状态为err 表示无路可走,当前字符不合法
		else if (!in_end_state(now) && jump(now, line[i + 1]) == ERR)
		{
			cout << "errors:" << errors++ << "  line:" << posLine << "\t未知符号或不合法字符： " << line[i] << endl;
			now = S0;
			index = i + 1;
		}
		//解决注释不全问题   最后一个字符的下一个字符是字符结尾 \0  当前字符的状态不是终态 
		else if (line[i + 1] == '\0' && !in_end_state(now))
		{
			cout << "errors:" << errors++ << "  line:" << posLine << "\t注释不全: " << changeToStr(line, index, i) << endl;
		}

	}

}

//*************************	词法分析总控程序	****************************
void lexical_analysis()
{
	cout << "开始词法分析！！！" << endl << endl;
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
	//最后插入#号  便于语法分析
	fout <<"#\t#\t"<< posLine;
	cout << "词法分析结束：发现" << errors - 1 << "处错误" << endl << endl;

	//关闭文件
	fin.close();
	fout.close();
}



//**********************************************************语法分析*************************************************************

class Tab {//预测分析表中单个产生式
public:
	Tab(string n, string e, string p) :noend(n), end(e), prod(p) {};
	string noend;
	string end;
	string prod;
};

//测试文件 test  要求文件 pragram
ifstream fin_result1;
//pragram1_result   测试 pragram1_result_test
char *inFileName_result1 = "pragram1_result.txt";//实验一处理的结果
vector<Tab>  pTab;       //预测分析表
stack<string>pStack;     //分析栈
vector<string>pStr;      ////读入符号串 缓冲区
int          index = 0;  //输入串的下标
int          num = 0;    //记录步骤数
string         X;          //从栈顶取出的符号
string         a;          //字符串目前分析到的位置
#define BEGIN "P"
//终结符号集合
vector<string> VT{ "ID","NUM","if","else","while","for","write","read","int","{","}"
,"+","-","*","/",">","<",">=","<=","==","!=",";","(",")","=" }; 
//非终结符号集合
vector<string> VN{ "P","D_L","D_S","S_L","S","I_S","I_S'","Wh_S","F_S","W_S"
,"R_S","C_S","A_S","B_E","B_E'","A_E","Ar_E","Ar_E'","T","T'","F"};  
//是否是终结符
int isPartVT(string ch)
{
	for (auto u : VT) {
		if (u == ch) {
			return 1;
		}
	}
	return 0;
}
//是否是非终结符
int isPartVN(string ch)
{
	for (auto u : VN) {
		if (u == ch) {
			return 1;
		}
	}
	return 0;
}
//分析表
void initpTab()
{
	pTab.push_back(Tab("P", "{", "{ D_L S_L }"));  //第一行 
	pTab.push_back(Tab("P", "#", "ε"));

	pTab.push_back(Tab("D_L", "if", "ε"));//2
	pTab.push_back(Tab("D_L", "else", "ε"));
	pTab.push_back(Tab("D_L", "while", "ε"));
	pTab.push_back(Tab("D_L", "for", "ε"));
	pTab.push_back(Tab("D_L", "write", "ε"));
	pTab.push_back(Tab("D_L", "read", "ε"));
	pTab.push_back(Tab("D_L", "int", "D_S D_L"));
	pTab.push_back(Tab("D_L", "{", "ε"));
	pTab.push_back(Tab("D_L", "}", "ε"));
	pTab.push_back(Tab("D_L", "ID", "ε"));
	pTab.push_back(Tab("D_L", ";", "ε"));

	pTab.push_back(Tab("D_S", "int", "int ID ;"));//3
	pTab.push_back(Tab("D_S", ";", "ε"));

	pTab.push_back(Tab("S_L", "if", "S S_L"));//4
	pTab.push_back(Tab("S_L", "else", "S S_L"));
	pTab.push_back(Tab("S_L", "while", "S S_L"));
	pTab.push_back(Tab("S_L", "for", "S S_L"));
	pTab.push_back(Tab("S_L", "write", "S S_L"));
	pTab.push_back(Tab("S_L", "read", "S S_L"));
	pTab.push_back(Tab("S_L", "{", "S S_L"));
	pTab.push_back(Tab("S_L", "}", "ε"));
	pTab.push_back(Tab("S_L", "ID", "S S_L"));
	pTab.push_back(Tab("S_L", ";", "S S_L"));

	pTab.push_back(Tab("S", "if", "I_S"));//5
	pTab.push_back(Tab("S", "else", "I_S"));
	pTab.push_back(Tab("S", "while", "Wh_S"));
	pTab.push_back(Tab("S", "for", "F_S"));
	pTab.push_back(Tab("S", "write", "W_S"));
	pTab.push_back(Tab("S", "read", "R_S"));
	pTab.push_back(Tab("S", "{", "C_S"));
	pTab.push_back(Tab("S", "ID", "A_S"));
	pTab.push_back(Tab("S", ";", ";"));

	pTab.push_back(Tab("I_S", "if", "if ( B_E ) S I_S'"));//6

	pTab.push_back(Tab("I_S'", "if", "ε"));//7
	pTab.push_back(Tab("I_S'", "else", "else S"));  
	pTab.push_back(Tab("I_S'", "while", "ε"));
	pTab.push_back(Tab("I_S'", "for", "ε"));
	pTab.push_back(Tab("I_S'", "write", "ε"));
	pTab.push_back(Tab("I_S'", "read", "ε"));
	pTab.push_back(Tab("I_S'", "{", "ε"));
	pTab.push_back(Tab("I_S'", "}", "ε"));
	pTab.push_back(Tab("I_S'", "ID", "ε"));
	pTab.push_back(Tab("I_S'", ";", "ε"));

	pTab.push_back(Tab("Wh_S", "while", "while ( B_E ) S"));//8
	pTab.push_back(Tab("F_S", "for", "for ( A_E ; B_E ; A_E ) S"));//9
	pTab.push_back(Tab("W_S", "write", "write Ar_E ;"));//10
	pTab.push_back(Tab("R_S", "read", "read ID ;"));//11
	pTab.push_back(Tab("C_S", "{", "{ S_L }"));//12
	pTab.push_back(Tab("A_E", "ID", "ID = Ar_E"));//13
	pTab.push_back(Tab("A_S", "ID", "A_E ;"));//14

	pTab.push_back(Tab("B_E", "ID", "Ar_E B_E'"));//15
	pTab.push_back(Tab("B_E", "NUM", "Ar_E B_E'"));
	pTab.push_back(Tab("B_E", "(", "Ar_E B_E'"));

	pTab.push_back(Tab("B_E'", ">", "> Ar_E"));//16
	pTab.push_back(Tab("B_E'", "<", "< Ar_E"));
	pTab.push_back(Tab("B_E'", ">=", ">= Ar_E"));
	pTab.push_back(Tab("B_E'", "<=", "<= Ar_E"));
	pTab.push_back(Tab("B_E'", "==", "== Ar_E"));
	pTab.push_back(Tab("B_E'", "!=", "!= Ar_E"));

	pTab.push_back(Tab("Ar_E", "ID", "T Ar_E'"));//17
	pTab.push_back(Tab("Ar_E", "NUM", "T Ar_E'"));
	pTab.push_back(Tab("Ar_E", "(", "T Ar_E'"));

	pTab.push_back(Tab("Ar_E'", "+", "+ T Ar_E'"));//18
	pTab.push_back(Tab("Ar_E'", "-", "- T Ar_E'"));
	pTab.push_back(Tab("Ar_E'", ">", "ε"));
	pTab.push_back(Tab("Ar_E'", "<", "ε"));
	pTab.push_back(Tab("Ar_E'", ">=", "ε"));
	pTab.push_back(Tab("Ar_E'", "<=", "ε"));
	pTab.push_back(Tab("Ar_E'", "!=", "ε"));
	pTab.push_back(Tab("Ar_E'", "==", "ε"));
	pTab.push_back(Tab("Ar_E'", ";", "ε"));
	pTab.push_back(Tab("Ar_E'", ")", "ε"));

	pTab.push_back(Tab("T", "ID", "F T'"));//19
	pTab.push_back(Tab("T", "NUM", "F T'"));
	pTab.push_back(Tab("T", "(", "F T'"));

	pTab.push_back(Tab("T'", "+", "ε"));//20
	pTab.push_back(Tab("T'", "-", "ε"));
	pTab.push_back(Tab("T'", "*", "* F T'"));
	pTab.push_back(Tab("T'", "/", "/ F T'"));
	pTab.push_back(Tab("T'", ">", "ε"));
	pTab.push_back(Tab("T'", "<", "ε"));
	pTab.push_back(Tab("T'", ">=", "ε"));
	pTab.push_back(Tab("T'", "<=", "ε"));
	pTab.push_back(Tab("T'", "!=", "ε"));
	pTab.push_back(Tab("T'", "==", "ε"));
	pTab.push_back(Tab("T'", ";", "ε"));
	pTab.push_back(Tab("T'", ")", "ε"));

	pTab.push_back(Tab("F", "ID", "ID"));//21
	pTab.push_back(Tab("F", "NUM", "NUM"));
	pTab.push_back(Tab("F", "(", "( Ar_E )"));
}
//打印栈
void printStack(string a)
{
	stack<string> pTemp_T;//倒过来存 实现先输出栈底符号
	stack<string> pTemp(pStack);
	string str_temp = "";
	while (pTemp.size() != 0) {
		pTemp_T.push(pTemp.top());
		pTemp.pop();
	}
	while (pTemp_T.size() != 0){
		str_temp += pTemp_T.top();
		str_temp += " ";
		pTemp_T.pop();
	}
	cout<< setw(45)<<str_temp<<"\t"<<a<<"\t";
}
//分割字符串
vector<string> split(const string &str, const string &pattern)
{
	//const char* convert to char*
	char * strc = new char[strlen(str.c_str()) + 1];
	strcpy(strc, str.c_str());
	vector<string> resultVec;
	char* tmpStr = strtok(strc, pattern.c_str());
	while (tmpStr != NULL)
	{
		resultVec.push_back(string(tmpStr));
		tmpStr = strtok(NULL, pattern.c_str());
	}
	delete[] strc;

	return resultVec;
}//分割字符串
//读入词法分析结果
void readInStr() 
{
	char buffer[256];
	fin_result1.open(inFileName_result1, ios::in);
	while (!fin_result1.eof())
	{
		vector<string>inStr_temp;
		//in_temp.clear();
		fin_result1.getline(buffer, 256, '\n');//读取一行
		inStr_temp = split(buffer, "\t");
		pStr.push_back(inStr_temp[0]);//输入需要判断的字符串
		pStr.push_back(inStr_temp[1]);//详细的字符串
		pStr.push_back(inStr_temp[2]);//字符所在行数
	}	
	fin_result1.close();
}
//读取下一个字符
string getsym()
{
	return pStr[(index++*3)];
}
//弹栈
string pop()
{
	if (pStack.size() != 0) {
		X = pStack.top();
		pStack.pop();
		return X;
	}
}
//反序压栈
void push(string tempProd)
{
	string temp_str;
	string temp_strArr[10];
	int k = 0;
	char temp_char = NULL;
	int i = 0;//字符串长度
	int len = tempProd.size();
	while (len--)
	{
		temp_char = tempProd[i];
		if (temp_char != ' ')//不是空格
		{
			temp_str += temp_char;
		}
		if (len == 0 || temp_char == ' ')//遇到了一个空格 或者到最后了 说明一个单词就读完了 
		{
			temp_strArr[k] = temp_str;//暂存
			k++;
			temp_str = "";//清零
		}
		i++;
	}
	//单词提取完了  逆序压栈
	while (--k >= 0)
	{
		pStack.push(temp_strArr[k]);//压栈
	}
}
//报错
void error(string X,string a)
{
	string str_temp="";
	for (auto u : pTab) //遍历分析表
	{
		if (u.noend == X) //X行a列  
		{
			str_temp += u.end;
			str_temp += "  ";
		}
	}
	if (a == "#")
	{
		cout << endl << endl << "there has a error :  expected: '} '"
			<< " before: '" << a << ": " << pStr[(index - 1) * 3 + 1] << "'" << endl;
	}
	else if (a=="ID"||a=="}")
	{
		cout <<endl << endl <<"there has a error :  expected: '; '"
			<< " before: '" << a << ": " << pStr[(index - 1) * 3 + 1] << "'" << endl;
	}

	else if (str_temp !="")
	{
		cout << endl << endl << "expected:   '" 
			<< str_temp << "' before: '" << a << ": " << pStr[(index - 1) * 3 + 1] << "'"
			<< "\tline:	" << pStr[(index - 1) * 3 + 2] << endl;
	}
	else
	{
		cout << endl << endl << "there has a error : " 
			"expected: '"<<X<<"' before: '"<< a << ": " << pStr[(index - 1) * 3 + 1]<<"'"
			<<"\tline:	"<< pStr[(index - 1) * 3 + 2] << endl;
	}

}
//分析
int synaly()
{
	//开始符号、# 压入栈
	pStack.push("#");
	pStack.push(BEGIN);

	//读入当前输入符  送a
	a = getsym();
	bool flag = true;
	while (flag) {
		cout <<left<<setw(3) <<num++ << "       ";//输出行号
		printStack(a);
		//将符号栈顶给X     
		X = pop();
		//如果是VT
		if (isPartVT(X))
		{
			if (X == a) {//X==a  读下一个符号
				cout << endl;
				a = getsym();
			}
			else {//出错
				error(X,a);
				return 0;
			}

		}
		else if (X == "#") //不是VT
		{
			if (X == a) {	//X == a == # 表示输入串完全匹配，分析成功。
				flag = false;
			}
			else {//出错
				error(X, a);
				return 0;
			}

		}
		//如果X是非终结符  则查分析表X行a列  
		//如果有一个产生式  则将X出栈、产生式反序入栈  若为空 出错
		else if (isPartVN(X) ) {
			int type = 0; //是否为 ε
			for (auto u : pTab) //遍历分析表
			{
				if (u.noend == X && u.end == a) //X行a列  
				{
					string tempProd = u.prod;//右部产生式
					cout << X << "->" << tempProd << endl;

					if (tempProd == "ε") //为空不压栈
					{
						type = 1;
						break;
					}
					//右部产生式 反序压入栈  注意单词分开 空格为分隔符
					else {
						push(tempProd);
						type = 1;
						break;
					}
				}
			}
			if (type != 1) {
				error(X, a);
				return 0;
			}
		}
		else {
			error(X, a);
			return 0;
		}
	}
	return 1;
}

//**************************	语法分析总控程序	************************
void Gramma_analysis()
{
	cout << "开始语法分析！！！" << endl << endl;
	cout << "步骤" << setw(12) <<"分析栈" <<setw(45)<< "当前符号" <<setw(15)<< "所用产生式" << endl;
	//初始化LL1分析表
	initpTab();
	//读入要分析的程序
	readInStr();
	//分析
	int flag = synaly();
	if (flag == 1) {
		cout << '\n' << "分析成功" << endl;
	}
	else {
		cout << '\n' << "分析失败" << endl;
	}
}

int main(int argc, char *argv[])
{
	//词法分析
	lexical_analysis();
	//语法分析
	Gramma_analysis();
	system("pause");
	return 0;
}
