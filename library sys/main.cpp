#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <fstream>
#include "Book.h"

#define CMD_COLS 80
#define CMD_LINES 25
using namespace std;

void SetScreenGrid();
void ClearScreen();
void SetSysCaption();
void SetSysCaption(const char* pText);
void ShowWelcome();
void ShowRootMenu();
void WaitView(int  iCurPage);
void WaitUser();
void GuideInput();
int GetSelect();
long GetFileLength(ifstream& ifs);
void ViewData(int iSelPage);
void DeleteBookFromFile();
void mainloop();

void SetScreenGrid()
{
	char sysSetBuf[80];
	sprintf_s(sysSetBuf, "mode con cols=%d lines=%d", CMD_COLS, CMD_LINES);
	system(sysSetBuf);
}
void ClearScreen()
{
	system("cls");
}
void SetSysCaption()
{
	system("title Sample");
}
void SetSysCaption(const char* pText)
{
	char sysSetBuf[80];
	sprintf_s(sysSetBuf, "title %s", pText);
	system(sysSetBuf);
}
void ShowWelcome()
{
	for (int i = 0; i < 7; i++)
	{
		cout << endl;
	}
	cout << setw(40);
	cout << "   *****************" << endl;
	cout << setw(40);
	cout << "    阅界藏书管理系统" << endl;
	cout << setw(40);
	cout << "   *****************" << endl;
}
void ShowRootMenu()
{
	cout << setw(38);
	cout << "请选择功能：" << endl;
	cout << endl;
	cout << setw(36);
	cout << "1 添加新书" << endl;
	cout << endl;
	cout << setw(36);
	cout << "2 浏览全部" << endl;
	cout << endl;
	cout << setw(36);
	cout << "3 删除图书" << endl;
}


void WaitView(int  iCurPage)
{
	char buf[256];
	gets_s(buf);
	if (buf[0] == 'q')
		system("exit");
	if (buf[0] == 'm')
		mainloop();
	if (buf[0] == 'n')
		ViewData(iCurPage);
}
void WaitUser()
{
	int iInputPage = 0;
	cout << "enter返回主菜单，q退出" << endl;
	char buf[256];
	gets_s(buf);
	if (buf[0] == 'q')
		system("exit");
}
void GuideInput()
{
	char inName[NUM1];
	char inIsdn[NUM1];
	char inPrice[NUM2];
	char inAuthor[NUM2];

	cout << "输入书名" << endl;
	cin >> inName;
	cout << "输入ISBN" << endl;
	cin >> inIsdn;
	cout << "输入价格" << endl;
	cin >> inPrice;
	cout << "输入作者" << endl;
	cin >> inAuthor;
	CBook book(inName, inIsdn, inPrice, inAuthor);
	book.WriteData();
	cout << "Write Finish" << endl;
	WaitUser();
}

int GetSelect()
{
	char buf[256];
	gets_s(buf);
	return atoi(buf);
}
long GetFileLength(ifstream& ifs)
{
	long tmppos;
	long respos;
	tmppos = ifs.tellg();//获得当前位置
	ifs.seekg(0, ios::end);
	respos = ifs.tellg();
	ifs.seekg(tmppos, ios::beg);//恢复当前位置
	return respos;
}


void ViewData(int iSelPage = 1)
{
	int iPage = 0;
	int iCurPage = 0;
	int iDataCount = 0;
	char inName[NUM1];
	char inIsbn[NUM1];
	char price[NUM2];
	char inAuthor[NUM2];
	bool bIndex = false;
	int iFileLength;
	iCurPage = iSelPage;
	ifstream ifile;
	ifile.open("book.dat", ios::binary);
	iFileLength = GetFileLength(ifile);
	iDataCount = iFileLength / (NUM1 + NUM1 + NUM2 + NUM2);
	if (iDataCount >= 1)
		bIndex = true;
	iPage = iDataCount / 20 + 1; //每页20条记录
	ClearScreen();
	cout << " 共有记录" << iDataCount << " ";
	cout << " 共有页数" << iPage << " ";
	cout << " 当前页数" << iCurPage << " ";
	cout << " n显示下一页 m返回" << endl;
	cout << setw(5) << "Index";
	cout << setw(22) << "Name" << setw(22) << "Isbn";
	cout << setw(15) << "Price" << setw(15) << "Author";
	cout << endl;

	try
	{
		ifile.seekg((iCurPage - 1) * 20 * (NUM1 + NUM1 + NUM2 + NUM2), ios::beg);
		if (!ifile.fail())
		{
			for (int i = 1; i < 21; i++)
			{
				memset(inName, 0, 128);
				memset(inIsbn, 0, 128);
				memset(price, 0, 50);
				memset(inAuthor, 0, 50);
				if (bIndex)
					cout << setw(3) << ((iCurPage - 1) * 20 + i);
				ifile.read(inName, NUM1);
				cout << setw(24) << inName;
				ifile.read(inIsbn, NUM1);
				cout << setw(24) << inIsbn;
				ifile.read(price, NUM2);
				cout << setw(12) << price;
				ifile.read(inAuthor, NUM2);
				cout << setw(12) << inAuthor;
				cout << endl;//一条纪录
				if (ifile.tellg() < 0)
					bIndex = false;
				else
					bIndex = true;
			}
		}
	}
	catch (...)
	{
		cout << "throw file exception" << endl;
		ifile.close(); // 先关闭文件再抛出，避免资源泄漏
		throw "file error occurred";
	}

	// 关键修复：所有读写完成后立即关闭文件，再等待用户输入
	ifile.close();

	if (iCurPage < iPage)
	{
		iCurPage = iCurPage + 1;
		WaitView(iCurPage);
	}
	else
	{
		WaitView(iCurPage);
	}
}


void DeleteBookFromFile()
{
	int iDelCount;
	cout << "Input delete index" << endl;
	cin >> iDelCount;

	// 校验输入是否为有效数字
	if (cin.fail())
	{
		cin.clear();          // 清除输入错误状态
		cin.ignore(1024, '\n'); // 清空输入缓冲区残留内容
		cout << "输入错误，请输入有效数字！" << endl;
		WaitUser();
		return;
	}
	// 清除缓冲区残留换行符，避免后续 gets_s 读取空行
	cin.ignore(1024, '\n');

	try
	{
		CBook tmpbook;
		tmpbook.DeleteData(iDelCount);
		cout << "Delete Finish" << endl;
	}
	catch (const char* errMsg)
	{
		cout << "删除失败：" << errMsg << endl;
	}

	WaitUser();
}
void mainloop()
{
	ShowWelcome();
	while (1)
	{
		ClearScreen();
		ShowWelcome();
		ShowRootMenu();
		switch (GetSelect())
		{
		case 1:
			ClearScreen();
			GuideInput();
			break;
		case 2:
			ClearScreen();
			ViewData();
			break;
		case 3:
			ClearScreen();
			DeleteBookFromFile();
			break;
		}
	}
}
void main()
{

	SetScreenGrid();
	SetSysCaption("阅界藏书管理系统");
	mainloop();
}
