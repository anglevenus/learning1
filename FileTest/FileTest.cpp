// FileTest.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <windows.h>
#include <io.h>
#include <map>

#include "md5.h"

#include "FileTest.h"

#include "PreGenerate.h"


extern "C" {
	#include "xxhash/xxhash.h"


	#include "lz4io.h"    /* LZ4IO_compressFilename, LZ4IO_decompressFilename, LZ4IO_compressMultipleFilenames */
	#include "lz4hc.h"    /* LZ4HC_CLEVEL_MAX */
	#include "lz4.h"      /* LZ4_VERSION_STRING */
}


using namespace std;

const char* inputPath = "F:\\SteamLibrary\\steamapps\\common\\The Witcher 3";
const char* cachedPath = "F:\\Tmp";

int main()
{
	//TestFileState();

	bigfile::PreGenerateManager::CreateInstance();

		
	TestReadFileTimeTest();
	
	
	//TestMD5Len();

	//TestHash32();

	//TestHash64();




    return 0;
}


string GetMD5(const string& src)
{
	MD5 md5;
	md5.update(src.c_str(), src.length());
	return md5.toString();
}

string GetMD5(ifstream& src)
{
	MD5 md5;
	md5.update(src);
	return md5.toString();
}

void TestFileState()
{
	const char* path = "E:\\test.txt";

	struct stat buf;

	int result = 0;
	result = stat(path, &buf);

	if (result != 0)
		perror("没有改文件");
	else
	{
		std::cout << "文件大小:" << buf.st_size << "字节" << endl;
		std::cout << "所在磁盘盘符 :";
		std::cout << char(buf.st_dev + 'A') << endl;
		std::cout << "文件创建时间:" << ctime(&buf.st_ctime);
		std::cout << "访问日期:" << ctime(&buf.st_atime);//注意这里访问时间为00:00:00为正常
		std::cout << "最后修改日期:" << ctime(&buf.st_mtime);

		std::cout << "文件创建时间:" << buf.st_ctime << endl;
		std::cout << "访问日期:" << buf.st_atime << endl;
		std::cout << "最后修改日期:" << buf.st_mtime << endl;

	}
}





void getFileList(const char* dirPath, std::vector<string> &files)
{
	intptr_t hFile = 0;
	struct _finddata_t fileinfo;
	string temp;

	int result = 0;
	if ( (hFile = _findfirst(temp.assign(dirPath).append("\\*").c_str(), &fileinfo)) != -1L)
	{
		do
		{
			//is a sub dir
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFileList(temp.assign(dirPath).append("\\").append(fileinfo.name).c_str(), files);
			}
			else
			{
				files.push_back(temp.assign(dirPath).append("\\").append(fileinfo.name));
			}

			;

		} while ((result = _findnext(hFile, &fileinfo)) == 0);

		_findclose(hFile);
	}
}

void TestReadFileTimeTest() 
{
	long long time = GetTickCount();
	
	std::vector<string> files;

	getFileList(inputPath, files);

	std::cout << "getfiles:" << files.size() << endl;

	


	std::map<string, string> contentMD5Map;

	//test just open handle time cost
	long long openCostTotal = 0;
	long long md5Time = 0;
	string output;
	int countForCompress = 0;
	for (std::vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		std::cout << "work on " << *it << endl;

		string fileNameMD5 = GetMD5(*it);

		long long start1 = GetTickCount();
		ifstream pfile(*it);
		if (!pfile.is_open())
		{
			std::cout << "can not open " << *it << endl;
			continue;
		}
		string contentM = GetMD5(pfile);
		pfile.close();
		md5Time += GetTickCount() - start1;

		std::cout << "Content MD5:" << contentM << endl;

		if (contentMD5Map.find(contentM) == contentMD5Map.end())
		{
			contentMD5Map[contentM] = *it;
		}
		else
		{
			std::cout << "重复 " << *it <<" with "<< contentMD5Map[contentM]<<endl;
			continue;
		}
		


		start1 = GetTickCount();
		output.assign(cachedPath).append("\\").append(fileNameMD5);

		int result = LZ4IO_compressFilename((*it).c_str(), output.c_str(), 0);

		countForCompress++;
		openCostTotal += GetTickCount() - start1;
	}

	std::cout << "md5Time use is " << (md5Time / files.size()) << endl;


	std::cout << "compress use is " << (openCostTotal / countForCompress) << endl;


	std::cout << "Total used time = " << (GetTickCount() - time) << endl;     
}
 
 


void TestMD5Len()
{
	const char* dirPath = "F:\\Games\\FFXIV170918";
	std::vector<string> files;

	getFileList(dirPath, files);

	std::map<string, string> md5Map;

	for (std::vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		string md5 = GetMD5(*it);

		if (md5Map.find(md5) == md5Map.end())
		{
			md5Map[md5] = *it;
			std::cout << "md5  for file " << *it <<" size = "<<sizeof(char)*md5.length()<< " : "<<md5<<endl;
		}
		else
			std::cout << "Already has the md5 " << md5 << " for file " << *it << endl;
	}
}


void TestHash64() 
{
	const char* dirPath = "F:\\Games\\FFXIV170918";
	std::vector<string> files;

	getFileList(dirPath, files);

	std::map<unsigned long long, string> map;

	for (std::vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		const char* path = (*it).c_str();
		unsigned long long md5 = XXH64(path, sizeof(char)*(*it).size(), 0);

		if (map.find(md5) == map.end())
		{
			map[md5] = *it;
			std::cout << "md5  for file " << *it << " size = " << sizeof(md5) << " : " << md5 << endl;
		}
		else
			std::cout << "Already has the md5 " << md5 << " for file " << *it << endl;
	}

}

void TestHash32()
{
	const char* dirPath = "F:\\Games\\FFXIV170918";
	std::vector<string> files;

	getFileList(dirPath, files);

	std::map<unsigned int, string> map;

	for (std::vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		const char* path = (*it).c_str();
		unsigned int md5 = XXH32(path, sizeof(char)*(*it).size(), 0);

		if (map.find(md5) == map.end())
		{
			map[md5] = *it;
			std::cout << "md5  for file " << *it << " size = " << sizeof(md5) << " : " << md5 << endl;
		}
		else
			std::cout << "Already has the md5 " << md5 << " for file " << *it << endl;
	}

}


