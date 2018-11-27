// experiment3.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
//语法、语义分析及代码生成
#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include<stdio.h>
#include<ctype.h>
#include<conio.h>
#include<string.h>
#include"stdlib.h"
#include<iostream>
#include <string>
#include<fstream>
using namespace std;

ifstream fin;
ofstream fout;
//测试文件 test  要求文件 pragram
char *inFileName = "code.txt";
char *outFileName = "program.txt";

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
				fout << keyWord[isKeywords(str)] << str_temp;
				//cout << "\t  S" << now << "\tkeyWord  " << keyWord[isKeywords(str)] << endl;
				fout << str << str_temp << endl;//存放到文件中 << posLine
			}
			else if (now != S11)
			{
				if (now != S1&& now != S2&& now != S3)
				{
					fout << str << str_temp << str << str_temp << endl;//存放到文件中  << posLine
				}
				else
				{
					fout << state_class[now] << str_temp;
					//cout << "\t  S" << now << "\t" << state_class[now] << endl;
					fout << str << str_temp << endl;//存放到文件中 << posLine
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
//************************************* 词法分析结束


#define maxvartablep 500 	//定乂符号表的容量
int TESTparse();
int program();
int compound_stat();
int statement();
int expression_stat();
int expression();
int bool_expr();
int additive_expr();
int term();
int factor();
int if_stat();
int while_stat();
int for_stat();
int write_stat();
int read_stat();
int declaration_stat();
int declaration_list();
int statement_list();
int compound_stat();
int name_def(char *name);
char token[20], token1[40];          //token保存单词符号,token1保存单词值  
char Scanout[300] = "F:\\学习\\学习资料\\大三\\编译原理\\experiment\\2018\\实验三\\experiment3\\experiment3\\program.txt";
char Codeout[300] = "F:\\学习\\学习资料\\大三\\编译原理\\experiment\\2018\\实验三\\experiment3\\experiment3\\result.txt";
//保存词法分析输出文件名
FILE *fp, *fout_p;                     //用于指向输入输出文件的指针 

struct {                               //定义符号表结构
	char name[20];
	int address;
	int notInit = 0;
}vartable[maxvartablep];              //改符号表最多容纳maxvarltablep个记录           

int vartablep = 0, labelp = 0, datap = 0;


//插入符号表动作@name-def↓n,t的程序如下：
int name_def(char * name)
{
	int i, es = 0;
	if (vartablep >= maxvartablep) return(21);
	for (i = vartablep - 1; i >= 0; i--)              //查符号表
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			es = 22;                                    //22表示变量重复声明
			break;
		}
	}
	if (es>0) return(es);
	strcpy(vartable[vartablep].name, name);
	vartable[vartablep].address = datap;
	datap++;                                     //分配一个单元,数据区指针加1
	vartablep++;
	return(es);
}

//查询符号表返回地址
int lookup(char * name, int * paddress)
{
	int i, es = 0;
	for (i = 0; i<vartablep; i++)
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			*paddress = vartable[i].address;
			return(es);
		}
	}
	es = 23;                                       //变量没有声明
	return(es);
}
//检查是否有初始值
int check(char *name)
{
	int i;
	int es = 0;
	for (i=0;i<vartablep;i++)
	{
		if (strcmp(vartable[i].name,name)==0)
		{
			if (vartable[i].notInit==0)
			{
				//报错 未付初值
				printf("\n%s变量未赋值初值\n",vartable[i].name);
				es = 24;
				break;
			}
		}

	}
	return es;
}

//改变初值状态
void change_init(char *name)
{
	for (int i=0;i<vartablep;i++)
	{
		if (strcmp(vartable[i].name, name) == 0)
		{
			vartable[i].notInit = 1;
		}
	}
}

//语法、语义分析及代码生成程序 
int TESTparse()
{
	int es = 0;
	//fp = fopen
	//printf("请输入词法分析结果文件名(包括路径):");
	//scanf("%s", Scanout);/*ERROR*/
	printf("词法分析结果：program.txt\t");
	if ((fp = fopen(Scanout, "r")) == NULL)
	{
		printf("\n打开%s错误！\n", Scanout);
		es = 10;
		return(es);
	}

	//printf("请输入目标文件名(包括路径):");
	//scanf("%s", Codeout);/*ERROR*/
	printf("中间代码结果：result.txt\n");
	if ((fout_p = fopen(Codeout, "w")) == NULL)
	{
		printf("\n 创建 %s 错误！ \n", Codeout);
		es = 10;
		return(es);
	}
	if (es == 0) es = program();
	printf("==语法、语义分析及代码生成程序结果==\n");
	switch (es)
	{
	case 0: printf("语法、语义分析成功并抽象机汇编生成代码!\n"); break;
	case 10:printf("打开文件 %s 失败\n", Scanout); break;
	case 1: printf("缺少{! \n"); break;
	case 2: printf("缺少}! \n"); break;
	case 3: printf("缺少标识符!"); break;
	case 4: printf("少分号! \n"); break;
	case 5: printf("缺少(！\n"); break;
	case 6: printf("缺少)！\n"); break;
	case 7: printf("缺少操作数!\n"); break;
	case 21:printf("符号表溢出！\n"); break;
	case 22:printf("变量重复定义！\n"); break;
	case 23:printf("变量未声明!\n"); break;
	case 24:printf("变量未赋初值!\n"); break;

	}

	fclose(fp);
	fclose(fout_p);
	return(es);
}

//program::= {<declaration_list><statement list> }
int program()
{
	int es = 0, i;
	fscanf(fp, "%s %s\n", token, token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "{"))	                             //判断是否"{"
	{
		es = 1;
		return(es);
	}
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = declaration_list();
	if (es>0) return(es);
	printf("符号表\n");
	printf("名字\t地址\n");
	for (i = 0; i<vartablep; i++)
		printf("%s\t%d\n", vartable[i].name, vartable[i].address);
	es = statement_list();
	if (es>0) return(es);
	if (strcmp(token, "}")) //判断是否"}"
	{
		es = 2;
		return(es);
	}
	fprintf(fout_p, "	STOP\n");	            //产生停止指令
	return(es);
}
//<declaration_list>::=<declaration_list><declaration_stat>|<declarartion_stat>
//改成<declaration_list>::={<declarationi_stat>}
int declaration_list()
{
	int es = 0;
	while (strcmp(token, "int") == 0)
	{
		es = declaration_stat();
		if (es>0) return(es);
	}
	return(es);
}

//<declaration_stat>↓vartablep,datap,codep->int ID↑n@name-def↓n,t;
int declaration_stat()
{
	int es = 0;
	fscanf(fp, "%s %s\n", &token, &token1); printf("%s %s\n", token, token1);
	if (strcmp(token, "ID")) return(es = 3);	     //不是标识符
	es = name_def(token1);	                     //插入符号表
	if (es>0) return(es);
	fscanf(fp, "%s%s\n", &token, &token1); printf("%s %s\n", token, token1);
	if (strcmp(token, ";")) return(es = 4);
	fscanf(fp, "%s %s\n", &token, &token1); printf("%s%s\n", token, token1);
	return(es);
}

//<statement_list>::=<statement_list><statement>|<statement>
//改成<statement_list>::={<statement>}
int statement_list()
{
	int es = 0;
	while (strcmp(token, "}"))
	{
		es = statement();
		if (es>0) return(es);
	}
	return(es);
}
//<statement> ::*<if_stat> |<while_stat> |<for_stat>
//  	        |<compound_stat> |<expression_stat> 
int statement()
{
	int es = 0;
	if (es = 0 && strcmp(token, "if") == 0)       es = if_stat();//<if语句>
	if (es == 0 && strcmp(token, "while") == 0) es = while_stat();//<while 语句> 
	if (es == 0 && strcmp(token, "for") == 0)
		es = for_stat();				//<for语句>
										//可在此处添加过do语句调用
	if (es == 0 && strcmp(token, "read") == 0)  es = read_stat();//<read 语句>
	if (es == 0 && strcmp(token, "write") == 0) es = write_stat();//<write @^> 
	if (es == 0 && strcmp(token, "{") == 0)es = compound_stat();//<复合语句> 
	if (es == 0 && (strcmp(token, "ID") == 0 || strcmp(token, "NUM") == 0 || strcmp(token, "(") == 0))
		es = expression_stat();	        //<表达式语句>
	return(es);
}

//<if_stat>::=if(<expr)<statement>[else<statement>]
/*
if(<expression>)@BRF↑label1<statement>@BR↑label2@SETlabel↓label1
[else<statement>]@SETlabel↓label2
其动作符号的含义如下
@BRF↑label1：输出BRF labell
@BR↑label↓label1:设置标号label1
@SETlabel↓label2：设置标号label2
*/
int if_stat()
{
	int es = 0, label1, label2;                      //if
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "(")) return(es = 5);        //少左括号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es> 0) return(es);
	if (strcmp(token, ")")) return(es = 6);      //少右括号 
	label1 = labelp++;                            //用label1记住条件为假时要转向的标号
	fprintf(fout_p, "	BRF LABEL%d\n", label1);//输出假转移指令
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = statement();
	if (es>0) return(es);
	label2 = labelp++;                            //用label2记住要转向的挥号                  
	fprintf(fout_p, "		BR LABEL%d\n", label2);      //输出无条件转移指令
	fprintf(fout_p, "LABEL%d: \n", label1);       //设置label1记住的标号  
	if (strcmp(token, "else") == 0)               //else部分处理
	{
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = statement();
		if (es>0) return(es);
	}


	fprintf(fout_p, "LABEL%d:\n", label2);                    //设置1沾612记住的标号 
	return(es);

}

//<while_stat> :: = while(<expression> ) <statement>
//<while_stat>::=while@SETlabel↑label1(expression>)@BRF↑label2
//				<statement>@BR↓label1@SETlabel↓label2
//动作解释如下：
//@SETlabel↑label1：设置标号 lable1
//@BRF↑label2：输出 BRF label2
//@BR↓label1：输出 BR label1
//@SETlabel↓label2：设置标号 label2
int while_stat()
{
	int es = 0, label1, label2; label1 = labelp++;
	fprintf(fout_p, "LABEL%d:\n", label1);	//设置1标号
	fscanf(fp, "%s %s\n", &token, &token1); printf("%s %s\n", token, token1);
	if (strcmp(token, "(")) return(es = 5);	//少左括号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es>0) return(es);
	if (strcmp(token, ")")) return(es = 6);	//少右括号
	label2 = labelp++;
	fprintf(fout_p, "		BRF LABEL%d\n", label2); //输出假转移指令
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = statement();
	if (es>0) return(es);
	fprintf(fout_p, "		BR LABEL%d\n", label1);	//输出无条件转移指令
	fprintf(fout_p, "LABEL%d:\n", label2);	    //设置2标号.
	return(es);

}

//< for_stat> ::for(<expr> ,<expr> ,<expr>) < statement>
/*
<for_stat>::=for(<expression>@POP;
@SETlabel↑label1<expression>@BRF↑label2@BR↓label3;
@SETlabel↑label4<expression>@POP@BR↓label1)
@SETlabel↑label3<语句>@BR↓label4@SETlabel↑label2
动作解释：
1、@SETlabel↑label1：设置标号 label1
2、@BRF↑label2：输出 BRF label2
3、@BR↓label3：输出 BR label3
4、@SETlabel↑label4：设置标号 label1
5、@BR↓label1：输出 BR label1
6、@SETlabel↑label3：设置标号 label3
7、@BR↓label4：输出 BR label4
8、@SETlabel↑label2：设置标号 label2
*/
int for_stat()
{
	int es = 0, label1, label2, label3, label4;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "(")) return(es = 5);	//少左括号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es>0) return(es);
	fprintf(fout_p, "	POP\n");                 //输出出栈指令
	if (strcmp(token, ";")) return(es *= 4);//少分号
	label1 = labelp++;
	fprintf(fout_p, "LABEL%d:\n", label1);       //设置 label1标号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	change_init(token1);
	es = expression();
	if (es>0) return(es);
	label2 = labelp++;
	fprintf(fout_p, " 	BRF LABEL%d\n", label2); //输出假条件转移指令  
	label3 = labelp++;
	fprintf(fout_p, "	    BR LABEL%d\n", label3);   //输出无条件转移指令
	if (strcmp(token, ";"))  return(es = 4);         //少分号
	label4 = labelp++;
	fprintf(fout_p, "LABEL%d:\n", label4);         //设置label4标号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es>0) return(es);
	fprintf(fout_p, "	POP\n");                //输出出找指令        
	fprintf(fout_p, "	BR LABEL%d\n", label1);     //输出无条件转移指令
	if (strcmp(token, ")")) return(es = 6);        //少右括号
	fprintf(fout_p, "LABEL%d:\n", label3);          //设置label3标号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = statement();
	if (es>0) return(es);
	fprintf(fout_p, "	BR LABEL%d\n", label4);     //输出无条件转移指令
	fprintf(fout_p, "LABEL%d:\n", label2);       //设置label2标号
	return(es);

}

//<write_stat>::=write<expression>;
//<write_stat>::=write<expression>@OUT;
//动作解释：
//@OUT：输出OUT
int write_stat()
{
	int es = 0;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	es = expression();
	if (es>0) return(es);
	if (strcmp(token, ";")) return(es = 4);	//少分号
	fprintf(fout_p, "	OUT\n");	            //输出0ut指令
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	return(es);
}

//<read_stat>::==read ID;
//<read_stat>::read ID
//<read_stat>::read ID↑n@LOOK↓n↑d@IN@STO↓d@POP;
//动作解释：
//@LOOK↓n↑d：查符号表n，给出变量地址d，若没有，则变量未定义
//@IN:输出 IN
//@STO↓d：输出指令代码STO d
int read_stat()
{
	int es = 0, address;
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, "ID")) return(es = 3);    //少标识符
	es = lookup(token1, &address);
	if (es > 0) return(es);
	change_init(token1);//改变初始值状态
	fprintf(fout_p, "	IN\n");               //输入指令
	fprintf(fout_p, "	STO %d\n", address);    //输出STO指令
	fprintf(fout_p, "	POP\n");
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	if (strcmp(token, ";")) return(es = 4);    //少分号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s%s\n", token, token1);
	return(es);
}


//<compound_stat> ::= {<statement_list>} 
//复合语句函数
int compound_stat()
{
	int es = 0;
	fscanf(fp, "%s %s\n", &token, &token1); //复合语句函数
	printf("%s %s\n", token, token1);
	es = statement_list();
	//ERROR
	if (strcmp(token, "}")) return(es = 2);	//少右大括号
	fscanf(fp, "%s %s\n", &token, &token1);
	printf("%s %s\n", token, token1);
	return(es);
}

//<expression_stat>::==<expression> @ POP;|; 
int expression_stat()
{
	int es = 0;
	if (strcmp(token, ";") == 0)
	{
		fscanf(fp, "%s%s\n", &token, &token1);
		printf("%s%s\n", token, token1);
		return(es);
	}
	es = expression();
	if (es>0) return(es);
	fprintf(fout_p, "	POP\n");       //输出POP指令
	if (strcmp(token, ";") == 0)
	{
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		return(es);
	}
	else
	{
		es = 4;
		return(es);                //少分号

	}

}

//<expression>::=ID↑n@LOOK↓n↑d@ASSIGN=<bool_expr>@STO↓d@POP|<bool_expr>
int expression()
{
	int es = 0, fileadd;
	char token2[20], token3[40];
	if (strcmp(token, "ID") == 0)
	{
		fileadd = ftell(fp);	//@ASSIGN己住当前文件位置
		fscanf(fp, "%s %s\n", &token2, &token3);
		printf("%s %s\n", token2, token3);
		if (strcmp(token2, "=") == 0)
		{
			int address;
			es = lookup(token1, &address);
			fscanf(fp, "%s %s\n", &token, &token1);
			printf("%s %s\n", token, token1);
			change_init(token1);
			es = bool_expr();
			if (es>0)  return(es);
			fprintf(fout_p, "   STO %d\n", address);
		}
		else
		{
			fseek(fp, fileadd, 0);	//文件指针回到之前的标识符
			printf("%s %s\n", token, token1);
			es = bool_expr();
			if (es> 0) return(es);
		}
	}
	else es = bool_expr();

	return(es);
}

//<bool_expr>::=<additive_expr>
//	|<additive_expr>(>|<|>=|<=|==|!=)<additive_expr>
/*
<bool_expr>::=<additive_expr>
|<additive_expr>><additive_expr>@GT
|<additive_expr><<additive_expr>@LES
|<additive_expr>>=<additive_expr>@GE
|<additive_expr><=<additive_expr>@LE
|<additive_expr>==<additive_expr>@EQ
|<additive_expr>!=<additive_expr>@NOTEQ
*/
int bool_expr()
{
	int es = 0;
	es = additive_expr();
	if (es>0) return(es);
	
	if (strcmp(token, ">") == 0 || strcmp(token, ">=") == 0 || strcmp(token, "<") == 0 || strcmp(token, "<=") == 0 || strcmp(token, "==") == 0 || strcmp(token, "!=") == 0)
	{
		char token2[20];
		strcpy(token2, token);	//保存运算符
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s%s\n", token, token1);
		es = additive_expr();
		if (es > 0) return(es);
		if (strcmp(token2, ">") == 0) fprintf(fout_p, "   GT\n");
		if (strcmp(token2, ">=") == 0) fprintf(fout_p, "   GE\n1");
		if (strcmp(token2, "<") == 0) fprintf(fout_p, "   LES\n");
		if (strcmp(token2, "<=") == 0) fprintf(fout_p, "   LE\n");
		if (strcmp(token2, "==") == 0) fprintf(fout_p, "   EQ\n");
		if (strcmp(token2, "!=") == 0) fprintf(fout_p, "   NOTEQ\n");
	}

	return(es);
}

//<additive_expr>::<term>{(+|-)<term>}
//<additibe_expr>::<term>{(+<term>@ADD|-<term>@SUB)}
int additive_expr()
{
	int es = 0;
	es = term();
	if (es>0) return(es);
	while (strcmp(token, "+") == 0 || strcmp(token, "-") == 0)
	{
		char token2[20];
		strcpy(token2, token);
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = term();
		if (es>0) return(es);
		if (strcmp(token2, "+") == 0) fprintf(fout_p, "	ADD\n");
		if (strcmp(token2, "-") == 0)fprintf(fout_p, "	SUB\n");
	}

	return(es);
}

//<term>::=<factor>{(*|/)<factor>}
//<term>::=<factor>{(*<factor>@MULT|/<factor>@DIV)}
int term()
{
	int es = 0;
	es = factor();
	if (es>0) return(es);
	while (strcmp(token, "*") == 0 || strcmp(token, "/") == 0)
	{
		char token2[20];
		strcpy(token2, token);
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = factor(); if (es>0) return(es);
		if (strcmp(token2, "*") == 0) fprintf(fout_p, "	MULT\n");
		if (strcmp(token2, "/") == 0)fprintf(fout_p, "	DIV\n");
	}

	return(es);

}

//<factor>::=(<additive_expr>)|ID|NUM
//<factor>::=(<expressioin>)|ID↑n@LOOK↓n↑d@LOAD↓d|NUM↑i@LOADI↓i
int factor()
{
	int es = 0;
	if (strcmp(token, "(") == 0)
	{
		fscanf(fp, "%s %s\n", &token, &token1);
		printf("%s %s\n", token, token1);
		es = expression();
		if (es>0) return(es);
		if (strcmp(token, ")")) return(es = 6);	//少右括号
		fscanf(fp, "%s %s\n", &token, &token1); printf("%s %s\n", token, token1);
	}
	else
	{
		if (strcmp(token, "ID") == 0)
		{
			int address;
			es = lookup(token1, &address);	//査符号表,获取变量地址
			es = check(token1);//检查是否附了初值
			if (es>0) return(es);	        //变量没声明  或者未付初值
			fprintf(fout_p, "	LOAD %d\n", address);
			fscanf(fp, "%s %s\n", &token, &token1);
			printf("%s %s\n", token, token1);
			return(es);
		}
		if (strcmp(token, "NUM") == 0)
		{
			fprintf(fout_p, "	LOADI %s\n", token1);
			fscanf(fp, "%s %s\n", &token, &token1);
			printf("%s %s\n", token, token1);
			return(es);
		}
		else
		{
			es = 7;	//缺少操作数
			return(es);
		}
	}

	return(es);
}



//extern int TESTmachine();
int TESTmachine()
{
	int es = 0, i, k = 0, codecount, stack[1000], stacktop = 0;
	char Codein[300] = "F:\\学习\\学习资料\\大三\\编译原理\\experiment\\2018\\实验三\\experiment3\\experiment3\\result.txt";
	char code[1000][20];
	int data[1000]; //用于接收输入文件名
	int label[100] = { 0 };
	char lno[4];
	FILE *fin;	//用于指向输入输出文件的指针
				//printf("请输入目标文件名(包括路径):");
				//scanf("%s", Codein);
	if ((fin = fopen(Codein, "r")) == NULL)
	{
		printf("\n打开%s错误!\n", code);
		es = 10;
		return(es);
	}
	codecount = 0;
	i = fscanf(fin, "%s", &code[codecount]);
	while (!feof(fin))
	{
		//printf("0000 %d %s \n", codecount, code[codecount]);
		i = strlen(code[codecount]) - 1;
		if (code[codecount][i] == ':')//表示是标号
		{
			i = i - 5;
			strncpy(lno, &code[codecount][5], i);//复制自定长度的子串
			lno[i] = '\0';
			label[atoi(lno)] = codecount;	//用label数组记住每个标号的地址
			//printf("label[%d]= %d \n", atoi(lno), label[atoi(lno)]);
			code[codecount][0] = ':';
			code[codecount][1] = '\0';
			strcat(code[codecount], lno);
			k++;
		}
		codecount++;
		i = fscanf(fin, "%s", &code[codecount]);
	}
	fclose(fin);
	for (i = 0; i < 10; i++)
		//printf("label %d %d \n", i, label[i]);

	for (i = 0; i < codecount; i++)
	{
		int l;
		l = strlen(code[i]);
		//printf("label[2] = %d  %d  %s  %d\n", label[2], i, code[i], l);
		if ((l > 1) && (code[i][1] == 'A'))
		{
			strncpy(lno, &code[i][5], l - 5);
			//printf("1111111111label[2]=%d lho= %s\n", label[2], lno);
			lno[l - 5] = '\0';
			_itoa(label[atoi(lno)], code[i], 10);
		}
	}
	for (k = 0; k < 5; k++)
		//printf("label %d %d\n", k, label[k]);
	i = 0;
	while (i < codecount)	//执行每条指令
	{
		//printf("code %d %s \n", i, code[i]);
		if (strcmp(code[i], "LOAD") == 0)	  //LOAD D将D中的内容加载到操作数栈
		{
			i++;
			stack[stacktop] = data[atoi(code[i])];
			stacktop++;
		}
		if (strcmp(code[i], "LOADI") == 0)	 //LOADI 8将常量己压入操作数栈
		{
			i++;
			stack[stacktop] = atoi(code[i]);
			stacktop++;
		}
		//STO D将操作数栈栈顶单元内容存入D,且栈顶单元内容保持不变 
		if (strcmp(code[i], "STO") == 0)
		{
			i++;
			//_itoa(stack[stacktop - 1], &data[atoi(code[i])], 10); ERROR
			data[atoi(code[i])] = stack[stacktop - 1];
			//printf("sto stack %d\n", stack[stacktop - 1]);
			//printf("sto data %d\n", data[atoi(code[i])]);
		}
		//POP桟顶单元内容出桟 
		if (strcmp(code[i], "POP") == 0)
		{
			stacktop--;
		}
		//ADD将次桟顶单元与栈顶单元内容出栈并相加，和置于栈顶 
		if (strcmp(code[i], "ADD") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] + stack[stacktop - 1];
			//printf("add %d\n", stack[stacktop - 1]);
			stacktop--;
		}
		//SUB 将次栈顶单元减去栈顶单元内容并出栈，差置于栈顶 
		if (strcmp(code[i], "SUB") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] - stack[stacktop - 1];
			stacktop--;
		}
		//MULT 将次栈顶单元与栈顶单元内容出栈并相乘，积置于栈顶 
		if (strcmp(code[i], "MULT") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] * stack[stacktop - 1];
			stacktop--;
		}
		//DIV 将次栈顶单元与栈顶单元内容出栈并相除,商置于栈顶 
		if (strcmp(code[i], "DIV") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] / stack[stacktop - 1];
			stacktop--;
		}
		//BR lab无条件转移到lab 
		if (strcmp(code[i], "BR") == 0)
		{
			i++;
			i = atoi(code[i]);
		}
		//BRF lab检查栈顶单元逻辑值并出栈,若为假(0)则转移到lab 
		if (strcmp(code[i], "BRF") == 0)
		{
			i++;
			if (stack[stacktop - 1] == 0) i = atoi(code[i]);
			stacktop--;
		}
		//EQ将栈顶两单元做等于比较并出栈,并将结果真或假(1或0)置于栈顶 
		if (strcmp(code[i], "EQ") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] == stack[stacktop - 1];
			stacktop--;
		}
		//NOTEQ栈顶两单元做不等于比较并出栈，并将结果真或假(1或0)置于栈顶 
		if (strcmp(code[i], "NOTEQ") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] != stack[stacktop - 1];
			stacktop--;
		}
		//GT 次栈顶大于栈顶操作数并出栈，则栈顶置1,否则置0 
		if (strcmp(code[i], "GT") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] > stack[stacktop - 1];
			stacktop--;
		}
		//LES次桟顶小于栈顶操作数并出栈，则栈顶置1,否则置0 
		if (strcmp(code[i], "LES") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] < stack[stacktop - 1];
			stacktop--;
		}
		//GE次栈顶大于等手栈顶操作数并出栈，则桟顶置1,否则置0 
		if (strcmp(code[i], "GE") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] >= stack[stacktop - 1];
			stacktop--;
		}
		//LE次栈顶小于等于桟顶操作数并出栈，则栈顶置1,否则置0 
		if (strcmp(code[i], "LE") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] <= stack[stacktop - 1];
			stacktop--;
		}
		//AND将栈顶两单元做逻辑与运算并出栈，并将结果真或假(1或0>置于栈顶 
		if (strcmp(code[i], "AND") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] && stack[stacktop - 1];
			stacktop--;
		}
		//0R将栈顶两单元做逻辑或运算并出栈，并将结果真或假(1或0)置于栈顶 
		if (strcmp(code[i], "OR") == 0)
		{
			stack[stacktop - 2] = stack[stacktop - 2] || stack[stacktop - 1];
			stacktop--;
		}
		//NOT将栈顶的逻辑值取反 
		if (strcmp(code[i], "NOT") == 0)
		{
			stack[stacktop - 1] = !stack[stacktop - 1];
		}
		//IN从标准输入设备(键盘)读入一个整型数据,并入栈 
		if (strcmp(code[i], "IN") == 0)
		{
			printf("请输入数据：");
			scanf("%d", &stack[stacktop]);
			stacktop++;
		}
		//OUT将栈顶单元内容出栈，并输出到标准输出设备上(显示器) 
		if (strcmp(code[i], "OUT") == 0)
		{
			printf("程序输出：%d\n", stack[stacktop - 1]);
			stacktop--;
		}
		//STOP停止执行
		if (strcmp(code[i], "STOP") == 0)
		{
			break;
		}
		i++;
		//显示栈内容
		//for (k=stacktop - 1; k> = 0;k--)
		// printf(" %d\n",stack[k]); 
		//printf(" %d %d\n", data[0], data[1]);
	}
}


void main()
{
	//词法分析
	//打开文件
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
	//语法 语义分析
	int es = 0;
	es = TESTparse();
	if (es == 0)  printf("中间代码生成成功!\n");
	else printf("中间代码生成失败!\n");
	es = TESTmachine();//调用抽象机模拟
	system("pause");
}