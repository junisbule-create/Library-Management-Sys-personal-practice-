#include "Book.h"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
using namespace std;
CBook::CBook(char* cName, char* cIsbn, char* cPrice, char* cAuthor)
{
	strncpy_s(m_cName, cName, NUM1);
	strncpy_s(m_cIsbn, cIsbn, NUM1);
	strncpy_s(m_cPrice, cPrice, NUM2);
	strncpy_s(m_cAuthor, cAuthor, NUM2);
}
char* CBook::GetName()
{
	return m_cName;
}
void CBook::SetName(char* cName)
{
	strncpy_s(m_cName, cName, NUM1);
}
char* CBook::GetIsbn()
{
	return m_cIsbn;
}
void CBook::SetIsbn(char* cIsbn)
{
	strncpy_s(m_cIsbn, cIsbn, NUM1);
}
char* CBook::GetPrice()
{
	return m_cPrice;
}
void CBook::SetPrice(char* cPrice)
{
	strncpy_s(m_cPrice, cPrice, NUM2);
}
char* CBook::GetAuthor()
{
	return m_cAuthor;
}
void CBook::SetAuthor(char* cAuthor)
{
	strncpy_s(m_cAuthor, cAuthor, NUM2);
}
void CBook::WriteData()
{
	ofstream ofile;
	ofile.open("book.dat", ios::binary | ios::app);
	try
	{
		ofile.write(m_cName, NUM1);
		ofile.write(m_cIsbn, NUM1);
		ofile.write(m_cPrice, NUM2);
		ofile.write(m_cAuthor, NUM2);
	}
	catch (...)
	{
		throw "file error occurred";
		ofile.close();
	}
	ofile.close();
}
void CBook::GetBookFromFile(int iCount)
{
	char cName[NUM1];
	char cIsbn[NUM1];
	char cPrice[NUM2];
	char cAuthor[NUM2];
	ifstream ifile;
	ifile.open("book.dat", ios::binary);
	try
	{
		ifile.seekg(iCount * (NUM1 + NUM1 + NUM2 + NUM2), ios::beg);
		ifile.read(cName, NUM1);
		if (ifile.tellg() > 0)
			strncpy_s(m_cName, cName, NUM1);
		ifile.read(cIsbn, NUM1);
		if (ifile.tellg() > 0)
			strncpy_s(m_cIsbn, cIsbn, NUM1);
		ifile.read(cPrice, NUM2);
		if (ifile.tellg() > 0)
			strncpy_s(m_cPrice, cPrice, NUM2);
		ifile.read(cAuthor, NUM2);
		if (ifile.tellg() > 0)
			strncpy_s(m_cAuthor, cAuthor, NUM2);
	}
	catch (...)
	{
		throw "file error occurred";
		ifile.close();
	}
	ifile.close();
}
void CBook::DeleteData(int iCount)
{
	const int RECORD_SIZE = NUM1 + NUM1 + NUM2 + NUM2;
	ifstream ifile;
	ofstream tmpfile;
	char* cTempBuf = nullptr;

	try
	{
		// 动态分配缓冲区，避免VLA兼容性问题
		cTempBuf = new char[RECORD_SIZE];
		memset(cTempBuf, 0, RECORD_SIZE);

		// 以二进制模式打开文件
		ifile.open("book.dat", ios::binary);
		tmpfile.open("temp.dat", ios::binary | ios::trunc);

		if (!ifile.is_open())
			throw "数据文件不存在，请先添加图书";
		if (!tmpfile.is_open())
			throw "临时文件创建失败，请检查磁盘权限";

		// 计算总记录数
		ifile.seekg(0, ios::end);
		long fileLen = ifile.tellg();
		int iDataCount = fileLen / RECORD_SIZE;

		// 序号合法性校验
		if (iCount < 1 || iCount > iDataCount)
			throw "输入序号错误，超出有效范围";

		// 回到文件开头，逐条复制，跳过待删除记录
		ifile.seekg(0, ios::beg);
		for (int i = 1; i <= iDataCount; i++)
		{
			memset(cTempBuf, 0, RECORD_SIZE);
			ifile.read(cTempBuf, RECORD_SIZE);
			if (i != iCount)
			{
				tmpfile.write(cTempBuf, RECORD_SIZE);
				if (tmpfile.fail())
					throw "临时文件写入失败";
			}
		}

		// 先关闭所有文件流，再操作文件（否则文件被占用必然失败）
		ifile.close();
		tmpfile.close();

		// 删除原文件+重命名临时文件，增加返回值校验
		if (remove("book.dat") != 0)
			throw "原文件删除失败（文件可能被占用）";
		if (rename("temp.dat", "book.dat") != 0)
			throw "文件重命名失败，数据保存在temp.dat中";

		delete[] cTempBuf;
	}
	catch (const char* errMsg)
	{
		// 异常分支统一释放资源
		if (cTempBuf != nullptr) delete[] cTempBuf;
		if (ifile.is_open()) ifile.close();
		if (tmpfile.is_open()) tmpfile.close();
		remove("temp.dat"); // 清理残留临时文件
		throw errMsg;
	}
}


