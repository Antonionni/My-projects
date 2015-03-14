#include "stdafx.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <atlstr.h>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <fstream>


using namespace std;
char buffer[BUFSIZ];
atomic<HANDLE> file;
atomic<bool> finish_flag(false);
CString folder_path;
atomic<int> sum(0);

void function_on_thread();

int _tmain(int argc, _TCHAR* argv[]) {
	folder_path = _T("C:\\Users\\Antosha\\Desktop\\folder");
	WIN32_FIND_DATA file_data;
	DWORD size_count;
	//HANDLE read_file;
	vector<thread> threads;

	CString file_path = folder_path + _T("\\*.txt");
	file = FindFirstFile(file_path, &file_data);
	if (file == INVALID_HANDLE_VALUE) {
		wcout << "Bad path to file" << endl;
		return 0;
	}

	CString file_name(file_data.cFileName);
	CString txt_file_name = folder_path + _T("\\") + file_name;


	ifstream read_file;
	read_file.open(txt_file_name);
	if (!read_file.is_open()) {
		wcout << "Bad path to file" << endl;
	} else {
		int data;
		read_file >> data;
		if (!read_file.eof()) {
			wcout << "File empty" << endl;
		} else {
			sum += data;
			wcout << (const wchar_t*)file_name << ":" << data << endl;
		}
		read_file.close();
	}
	
	while (finish_flag != true) {
		threads.push_back(thread(function_on_thread));
	}

	for (int i = 0; i < threads.size(); ++i) {
		if (threads[i].joinable()) {
			threads[i].join();
		}
	}

	wcout << "Summ = " << sum << endl;
	system("pause");
	return 0;
}

void function_on_thread() {
	std::mutex mutex_;
	WIN32_FIND_DATA data;
	DWORD size_count;
	HANDLE read_file;
	if (FindNextFile(file, &data) != 0) {
		mutex_.lock();
		CString name(data.cFileName);
		CString file_name = folder_path + _T("\\") + name;

		ifstream read_file;
		read_file.open(file_name);
		if (!read_file.is_open()) {
			wcout << "Bad file!" << endl;
		} else {
			int data;
			read_file >> data;
			if (!read_file.eof()) {
				wcout << "File is empty" << endl;
			} else {
				sum += data;
				wcout << (const wchar_t*)name << ":" << data << endl;
			}
			read_file.close();
		}
		mutex_.unlock();
		Sleep(1000);
	}
	else if (GetLastError() == ERROR_NO_MORE_FILES) {
		finish_flag = true;
	}
}