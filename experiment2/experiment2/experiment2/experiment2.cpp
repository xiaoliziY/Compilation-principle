// experiment2.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <vector>

using namespace std;

typedef struct
{
	char * m_word;
	int m_line;
}wordinformation;

vector<string>identityWord;
vector<string>wrongWord;
vector<string>keyWord;
vector<wordinformation>WordInformation;
char * presentword;
int row = -1;
ifstream fin;
ofstream fout;

void getNextSymbol(char *&presentword);
void returnWord();
void error(char *errorInformation, int line);
int judgeSet(char *word, char(*set)[20], int setsize);

void factor();
void expression();
void term();
void term1();
void addtive_expr();
void addtive_expr1();
void bool_expr();
void bool_expr1();
void expression_stat();
void compound_stat();
void read_stat();
void write_stat();
void for_stat();
void while_stat();
void if_stat();
void statement();
void statement_list();
void declaration_stat();
void declaration_list();
void program();

int IsKeyWord(char * word)
{
	char *keyWord[8] = { "if", "else", "for", "while", "do", "int", "write", "read" };
	int i = 0;
	while (i < 8 && strcmp(keyWord[i], word))
	{
		i++;
	}
	if (i < 8)
	{
		return 1;
	}
	return 0;
}



int WordAnalysis() {
	char tempArray[30];
	char Tokens[30] = " \n\t";
	char singleTokens[10] = "(){};+-*";
	char doubleTokens[10] = "=<>!";
	int i = 0;
	int line = 1;
	if (!fin.is_open())
		return 1;
	if (!fout.is_open())
		return 2;
	char ch;
	ch = fin.get();
	while (ch != EOF)
	{
		if (ch == '\n')
			line++;
		if (isalpha(ch))            //处理标识符
		{
			tempArray[i++] = ch;
			ch = fin.get();
			while (isalnum(ch)) {
				tempArray[i++] = ch;
				ch = fin.get();
			}
			tempArray[i++] = '\0';
			i = 0;
			string s(tempArray);
			identityWord.push_back(s);
			if (IsKeyWord(tempArray))
			{
				keyWord.push_back(s);
				wordinformation word;
				word.m_line = line;
				word.m_word = new char[10];
				strcpy_s(word.m_word, 10, s.data());
				WordInformation.push_back(word);
			}
			else
			{
				wordinformation word;
				word.m_line = line;
				word.m_word = new char[10];
				strcpy_s(word.m_word, 10, "ID");
				WordInformation.push_back(word);
			}
		}
		else if (ch == '/')          //处理 /*   */和/
		{
			tempArray[i++] = '/';
			ch = fin.get();
			if (ch == '*')            //处理 /*   */
			{
				tempArray[i++] = '*';
				ch = fin.get();
				char ch0 = ch;
				ch = fin.get();
				while (!(ch0 == '*' && ch == '/') && (ch != '\n' || ch == EOF))
				{
					ch0 = ch;
					ch = fin.get();
				}
				if (ch == '\n' || ch == EOF) {
					stringstream strs;
					strs << line;
					string str;
					strs >> str;
					strs.clear();
					string str1 = "";
					wrongWord.push_back("Error : Line " + str + ":“/*    */”不完整");
					i = 0;
					ch = fin.get();
				}
				else
				{
					tempArray[i++] = '*';
					tempArray[i++] = '/';
					tempArray[i++] = '\0';
					i = 0;
					string str(tempArray);
					identityWord.push_back(str);
					ch = fin.get();
				}
			}
			else
			{
				tempArray[i++] = '\0';		//处理 /
				i = 0;
				string str(tempArray);
				identityWord.push_back(str);
				string str1 = "/";
				wordinformation word;
				word.m_line = line;
				word.m_word = new char[10];
				strcpy_s(word.m_word, 10, "/");
				WordInformation.push_back(word);
			}
		}
		else if (isdigit(ch))
		{				//处理数字
			tempArray[i++] = ch;
			if (ch != '0')
			{
				ch = fin.get();
				while (isdigit(ch))
				{
					tempArray[i++] = ch;
					ch = fin.get();
				}
			}
			else {
				ch = fin.get();
			}
			tempArray[i++] = '\0';
			i = 0;
			string str(tempArray);
			identityWord.push_back(str);
			wordinformation word;
			word.m_line = line;
			word.m_word = new char[10];
			strcpy_s(word.m_word, 10, "NUM");
			WordInformation.push_back(word);
		}
		else if (strchr(singleTokens, ch) != NULL)
		{		//处理单分界符 
			tempArray[i++] = ch;
			tempArray[i++] = '\0';
			i = 0;
			string str(tempArray);
			identityWord.push_back(str);
			ch = fin.get();
			wordinformation word;
			word.m_line = line;
			word.m_word = new char[10];
			strcpy_s(word.m_word, 10, str.data());
			WordInformation.push_back(word);
		}
		else if (strchr(doubleTokens, ch) != NULL) {		//处理双分界符
			tempArray[i++] = ch;
			ch = fin.get();
			if (ch == '=') {
				tempArray[i++] = ch;
				ch = fin.get();
			}
			else if (tempArray[i - 1] == '!') {
				stringstream strs;
				strs << line;
				string str;
				strs >> str;
				strs.clear();
				tempArray[i++] = '\0';
				i = 0;
				string str1(tempArray);
				wrongWord.push_back("Error : Line " + str + ": " + str1 + "错误");
				continue;
			}
			tempArray[i++] = '\0';
			i = 0;
			string str(tempArray);
			identityWord.push_back(str);
			wordinformation word;
			word.m_line = line;
			word.m_word = new char[10];
			strcpy_s(word.m_word, 10, str.data());
			WordInformation.push_back(word);
		}
		else {
			if (!strchr(Tokens, ch)) {
				stringstream strs;
				strs << line;
				string str;
				strs >> str;
				strs.clear();
				tempArray[i++] = ch;
				i = 0;
				string str1(tempArray);
				wrongWord.push_back("Error : Line " + str + ": " + str1 + "错误");
			}
			ch = fin.get();
		}

	}
	return 0;
}

int main()
{
	fin.open("TEST.txt");
	fout.open("out.txt");
	int error = WordAnalysis();
	if (error == 1)
		cout << "打开输入文件错误" << endl;
	else if (error == 2)
	{
		cout << "打开输出文件错误" << endl;
	}
	else
	{
		cout << "success\n" << endl;
		fout << "分析得到的单词:\n";
		for (int i = 0; i < identityWord.size(); i++)
		{
			fout << identityWord[i] << "\n";
		}
		fout << "关键词：\n";
		for (int i = 0; i < keyWord.size(); i++)
		{
			fout << keyWord[i] << "\n";
		}
		fout << "错误单词:\n";
		for (int i = 0; i < wrongWord.size(); i++) {
			cout << wrongWord[i] << "\n";
		}
	}
	fin.close();
	fout.close();
	cout << "错误信息：\n";
	program();
	system("pause");
	return 0;
}

/************语法分析*************/

void getNextSymbol(char *&word)
{
	if (row < (int)WordInformation.size() - 1)
	{
		word = WordInformation[++row].m_word;
	}
	else if (row == (int)WordInformation.size() - 1)
	{
		strcpy_s(word, 2, "#");
	}
}

void returnWord()
{
	if (row >= 1)
	{
		row--;
	}
}

void error(char *errorinformation, int line)
{
	if (line < 10)
	{
		cout << "Error : Line  " << line << " : " << errorinformation << " " << endl;
	}
	if (line >= 10)
	{
		cout << "Error : Line " << line << " : " << errorinformation << " " << endl;
	}
}

int judgeSet(char *word, char(*set)[20], int setsize)
{
	if (word == NULL)
	{
		return 0;
	}
	for (int i = 0; i < setsize; i++)
	{
		if (strcmp(word, set[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

//1) <program>→{<declaration_list><statement_list>}
void program()
{
	getNextSymbol(presentword);
	if (strcmp(presentword, "{") == 0)
		getNextSymbol(presentword);
	else
		error("缺少 “{”", WordInformation[row].m_line);
	declaration_list();
	statement_list();
	if (strcmp(presentword, "}") == 0)
		getNextSymbol(presentword);
	else
		error("整体语句缺少 “}”", WordInformation[row].m_line);
}

/*2) <declaration_list>→<declaration_list><declaration_stat> | ε*/
void declaration_list()
{
	char FIRSTset[][20] = { "int" };
	char FOLLOWset[][20] = { "if", "while", "for", "read", "write", "{", "}", "(" };

	if (judgeSet(presentword, FIRSTset, 1) == 1) {
		declaration_stat();
		declaration_list();
	}
	else if (judgeSet(presentword, FOLLOWset, 8) == 1)
		return;
	else {
		while (!(judgeSet(presentword, FIRSTset, 1) == 1
			|| judgeSet(presentword, FOLLOWset, 8) == 1))
			getNextSymbol(presentword);
		declaration_list();
	}
}

// 3)<declaration_stat>→int ID;
void declaration_stat()
{
	if (strcmp(presentword, "int") == 0)
		getNextSymbol(presentword);
	else
	{
		error("声明缺少 “int”", WordInformation[row].m_line);
		return;
	}
	if (strcmp(presentword, "ID") == 0)
		getNextSymbol(presentword);
	else
	{
		error("声明缺少 “ID”", WordInformation[row].m_line);
		return;
	}
	if (strcmp(presentword, ";") == 0)
		getNextSymbol(presentword);
	else
	{
		error("声明缺少 “;”", WordInformation[row].m_line);
		return;
	}
}

/*
4)//<statement_list>→<statement><statement_list> | ε
4) FIRST(<statement><statement_list>)={if、while、for、read、write、{、(、ID、NUM、;  、ε}
4）FOLLOW(statement_list)={ }  }
*/
void statement_list() {
	char FIRSTset[][20] = { "if", "while", "for", "read", "write", "{", "(", "ID", "NUM", ";" };
	char FOLLOWset[][20] = { "}" };
	if (judgeSet(presentword, FIRSTset, 10) == 1)
	{
		statement();
		statement_list();
	}
	else if (judgeSet(presentword, FOLLOWset, 1) == 1)
		return;
}

/*
5)<statement>→ <if_stat>|<while_stat>|<for_stat>|<read_stat>|<write_stat>
| <compound_stat> |<expression_stat>
FIRST<if_stat>::={ if }
FIRST<while_stat>)={while}
FIRST(<for_stat>)<statement>)={for}
FIRST(<read_stat>)={read}
FIRST(<write_stat>)={write}
FIRST(<compound_stat>)={{}
12)FRIST(<expression_stat>)={(、ID、NUM、；}
*/
void statement() {
	char FIRSTsetIf_stat[][20] = { "if" };
	char FIRSTsetWhile_stat[][20] = { "while" };
	char FIRSTsetFor_stat[][20] = { "for" };
	char FIRSTsetRead_stat[][20] = { "read" };
	char FIRSTsetWrite_stat[][20] = { "write" };
	char FIRSTsetCompound_stat[][20] = { "{" };
	char FIRSTsetExpression[][20] = { "(", "ID", "NUM", ";" };
	if (judgeSet(presentword, FIRSTsetIf_stat, 1) == 1)
		if_stat();
	else if (judgeSet(presentword, FIRSTsetWhile_stat, 1) == 1)
		while_stat();
	else if (judgeSet(presentword, FIRSTsetFor_stat, 1) == 1)
		for_stat();
	else if (judgeSet(presentword, FIRSTsetRead_stat, 1) == 1)
		read_stat();
	else if (judgeSet(presentword, FIRSTsetWrite_stat, 1) == 1)
		write_stat();
	else if (judgeSet(presentword, FIRSTsetCompound_stat, 1) == 1)
		compound_stat();
	else if (judgeSet(presentword, FIRSTsetExpression, 4) == 1)
		expression_stat();
}

/*
6) <if_stat>→ if (<expr>) <statement > [else < statement >]
FIRST(if (<expr>) <statement > [else < statement >])={ if }
*/
void if_stat()
{
	if (strcmp(presentword, "if") == 0)
		getNextSymbol(presentword);
	else
		error("缺少 “if”", WordInformation[row].m_line);
	if (strcmp(presentword, "(") == 0)
		getNextSymbol(presentword);
	else
		error("if缺少 “(”", WordInformation[row].m_line);
	expression();
	if (strcmp(presentword, ")") == 0)
		getNextSymbol(presentword);
	else
		error("if缺少“)”", WordInformation[row].m_line);
	statement();
	if (strcmp(presentword, "else") == 0)
	{
		getNextSymbol(presentword);
		statement();
	}
}

/*
7)<while_stat>→ while (<expr >) < statement >
FIRST(while (<expr >) < statement >)={ while }
*/
void while_stat()
{
	if (strcmp(presentword, "while") == 0)
		getNextSymbol(presentword);
	else
		error("缺少 “while”", WordInformation[row].m_line);
	if (strcmp(presentword, "(") == 0)
		getNextSymbol(presentword);
	else
		error("while缺少 “(r”", WordInformation[row].m_line);
	expression();
	if (strcmp(presentword, ")") == 0)
		getNextSymbol(presentword);
	else
		error("while缺少 “)”", WordInformation[row].m_line);
	statement();
}

/*
8)<for_stat>→ for(<expr>;<expr>;<expr>)<statement>
FIRST(for(<expr>;<expr>;<expr>)<statement>) < statement >)={for }
*/
void for_stat()
{
	if (strcmp(presentword, "for") == 0)
		getNextSymbol(presentword);
	else
		error("缺少 “for”", WordInformation[row].m_line);
	if (strcmp(presentword, "(") == 0)
		getNextSymbol(presentword);
	else
		error("for缺少 “(”", WordInformation[row].m_line);
	expression();
	if (strcmp(presentword, ";") == 0)
		getNextSymbol(presentword);
	else
		error("for缺少第一个 “;” ", WordInformation[row].m_line);
	expression();
	if (strcmp(presentword, ";") == 0)
		getNextSymbol(presentword);
	else
		error("for缺少第二个 “;”", WordInformation[row].m_line);
	expression();
	if (strcmp(presentword, ")") == 0)
		getNextSymbol(presentword);
	else
		error("for缺少 “)”", WordInformation[row].m_line);
}

/*
9)<write_stat>→write <expression>;
FIRST(write <expression>;)={write }
*/
void write_stat()
{
	if (strcmp(presentword, "write") == 0)
	{
		getNextSymbol(presentword);
	}
	else
	{
		error("缺少 “write”", WordInformation[row].m_line);
	}
	expression();
	if (strcmp(presentword, ";") == 0)
	{
		getNextSymbol(presentword);
	}
	else
	{
		error("缺少 “;”", WordInformation[row].m_line);
	}
}
/*
10)<read_stat>→read ID;
FIRST(read ID;)={read }
*/
void read_stat()
{
	if (strcmp(presentword, "read") == 0)
		getNextSymbol(presentword);
	else
		error("缺少 “read”", WordInformation[row].m_line);
	if (strcmp(presentword, "ID") == 0)
		getNextSymbol(presentword);
	else
		error("缺少 标识符", WordInformation[row].m_line);
	if (strcmp(presentword, ";") == 0)
		getNextSymbol(presentword);
	else
		error("缺少 “;”", WordInformation[row].m_line);
}

/*
11)<compound_stat>→{<statement_list>}
FIRST({<statement_list>})={{ }
*/
void compound_stat()
{
	if (strcmp(presentword, "{") == 0)
	{
		getNextSymbol(presentword);
	}
	else
	{
		error("缺少 “{”", WordInformation[row].m_line);
	}
	statement_list();
	if (strcmp(presentword, "}") == 0)
	{
		getNextSymbol(presentword);
	}
	else
	{
		error("缺少 “}”", WordInformation[row].m_line);
	}
}

/*
12)<expression_stat>→< expression >;|;
FIRST(< expression >;)={ ID}
FIRST(;})={;}
*/
void expression_stat()
{
	char FIRSTset[][20] = { "ID" };
	if (judgeSet(presentword, FIRSTset, 1) == 1)
	{
		expression();
		if (strcmp(presentword, ";") == 0)
		{
			getNextSymbol(presentword);
		}
		else
		{
			error("缺少 “;”", WordInformation[row].m_line);
		}
	}
	else if (strcmp(presentword, ";") == 0)
	{
		getNextSymbol(presentword);
	}
}

/*
13)< expression >→ ID=<bool_expr>|<bool_expr>
FIRST(ID=<bool_expr>|<bool_expr>)={ID }
13)FRIST(<bool_expr>)={ID、(、NUM}
*/
void expression()
{
	char FIRSTset[][20] = { "(", "ID", "NUM" };
	char *aheadword = NULL;      //定义一个超前符号来解决首符号集相交的问题
	getNextSymbol(aheadword);
	returnWord();

	//当前字符在FRIST(<bool_expr>)={ID、(、NUM}中，并且下一个符号不是=时 选用布尔表达式
	if (judgeSet(presentword, FIRSTset, 3) == 1 && strcmp(aheadword, "=") != 0)
	{
		bool_expr();
	}
	else if (strcmp(presentword, "ID") == 0)
	{
		if (strcmp(presentword, "ID") == 0)
		{
			getNextSymbol(presentword);
		}
		else
		{
			error("缺少 “ID”", WordInformation[row].m_line);
		}
		if (strcmp(presentword, "=") == 0)
		{
			getNextSymbol(presentword);
		}
		else
		{
			error("缺少 “=”", WordInformation[row].m_line);
		}
		bool_expr();
	}
}

/*
14)<bool_expr>→<additive_expr><bool_expr1>
<bool_expr1>→ >< additive_expr >|<< additive_expr >|>=< additive_expr >|<=<
<additive_expr >|==< additive_expr >|!=< additive_expr >|ε
FRIST(<additive_expr>) = {（，ID，NUM}
FIRST(<bool_expr1>)={ }
FOLLOW(<bool_expr1>)={ ;、） }
*/
void bool_expr()
{
	char FIRSTset[][20] = { "(", "ID", "NUM" };
	if (judgeSet(presentword, FIRSTset, 3) == 1)
	{
		addtive_expr();
		bool_expr1();
	}
}
void bool_expr1()
{
	char FIRSTset[][20] = { "<", ">", "<=", ">=", "!=", "==" };
	char FOLLOWset[][20] = { ")", ";" };
	if (judgeSet(presentword, FIRSTset, 6) == 1)
	{
		getNextSymbol(presentword);
		addtive_expr();
	}
	else if (judgeSet(presentword, FOLLOWset, 2) == 1)
	{
		return;
	}
	else
	{
		if (judgeSet(presentword, FIRSTset, 6) == 0 && judgeSet(presentword, FOLLOWset, 2) == 0)
		{
			error("缺少逻辑符号", WordInformation[row].m_line);
		}
	}
}

/*
15)< additive_expr>→<term>< additive_expr1>
< additive_expr1>→+< term >< additive_expr1>|-< term >< additive_expr1>|ε
FRIST(<additive_expr>) = {（，ID，NUM}
FRIST(<additive_expr1>) = {+,-}
FOLLOW(< additive_expr1>) =={ < 、>  、<= 、 >= 、== 、!= 、； 、) }
*/
void addtive_expr()
{
	char FIRSTset[][20] = { "(", "ID", "NUM" };
	if (judgeSet(presentword, FIRSTset, 3) == 1)
	{
		term();
		addtive_expr1();
	}
	else
	{
		error("缺少 “ID” 或 缺少 “NUM”", WordInformation[row].m_line);
	}
}
void addtive_expr1()
{
	char FIRSTset[][20] = { "+", "-" };
	char FOLLOWset[][20] = { "<", ">", ">=", "!=", "==", ")", ";" };
	if (judgeSet(presentword, FIRSTset, 2) == 1)
	{
		getNextSymbol(presentword);
		term();
		addtive_expr1();
	}
	else if (judgeSet(presentword, FOLLOWset, 8) == 1)
	{
		return;
	}
}

/*
16)< term >→<factor><term1>
< term1>→*< factor >< term1> | / < factor >< term1> | ε
FRIST(<< factor>< term1 >> ) = { （，ID，NUM }
FIRST(<term1>) = { *、 / }
FOLLOW(<term1>) = { +、—、< 、>  、 <= 、 >= 、 == 、 != 、； 、) }
*/
void term()
{
	char FIRSTset[][20] = { "(", "ID", "NUM" };
	if (judgeSet(presentword, FIRSTset, 3) == 1)
	{
		factor();
		term1;
	}
}
void term1()
{
	char FIRSTset[][20] = { "*", "/" };
	char FOLLOWset[][20] = { "+", "-", ">", "<", "<=", ">=", "!=", "==", ";", ")" };
	if (judgeSet(presentword, FIRSTset, 2) == 1)
	{
		getNextSymbol(presentword);
		factor();
		term1();
	}
	else if (judgeSet(presentword, FOLLOWset, 10) == 1)
	{
		return;
	}
}

/*
17) <factor> →（<experssion>)|ID|NUM
FIRST(<experssion>) = { ( }
FIRST(ID) = { ID }
FIRST(NUM) = { NUM }
*/
void factor()
{
	char FIRSTset[][20] = { "(" };
	char FIRSTset1[][20] = { "ID" };
	char FIRSTset2[][20] = { "NUM" };
	if (judgeSet(presentword, FIRSTset, 1) == 1)
	{
		getNextSymbol(presentword);
		expression();
		if (strcmp(presentword, ")") == 0)
		{
			getNextSymbol(presentword);
		}
		else
			error("缺少 “ ) ”", WordInformation[row].m_line);
	}
	else if (judgeSet(presentword, FIRSTset1, 1) == 1)
	{
		getNextSymbol(presentword);
	}
	else if (judgeSet(presentword, FIRSTset2, 1) == 1)
	{
		getNextSymbol(presentword);
	}
}
