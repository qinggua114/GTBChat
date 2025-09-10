#ifndef CALCULATOR_H 
#define CALCULATOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RDWR SD_BOTH
using namespace std;
//Get time
auto now = chrono::system_clock::now();
string getCurrentTime(){
    auto now_time = chrono::system_clock::to_time_t(now);
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    tm bt;
    #ifdef _WIN32
    localtime_s(&bt, &now_time); // Windows
    #else
    localtime_r(&now_time, &bt); // POSIX
    #endif
    
    stringstream ss;
    ss << put_time(&bt, "%H:%M:%S");//<< '.' << setfill('0') << setw(3) << ms.count();
    return ss.str();
}
string getDay(){
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm local_time = *localtime(&now_time);
    
    int year = local_time.tm_year + 1900;
    int month = local_time.tm_mon + 1;
    int day = local_time.tm_mday;
    return to_string(year)+"-"+to_string(month)+"-"+to_string(day);
}

string logPathStr = "logs\\"+getDay()+"-log.txt";
string dbLogPathStr = "dblogs\\"+getDay()+"-log.txt";
const char* logPath = logPathStr.c_str();
const char* dbLogPath = dbLogPathStr.c_str();

// ANSI ColorCode
const string COLOR_RESET = "\033[0m";
const string COLOR_RED = "\033[31m";
const string COLOR_GREEN = "\033[32m";
const string COLOR_YELLOW = "\033[93m";
const string COLOR_BLUE = "\033[34m";
const string COLOR_MAGENTA = "\033[35m";
const string COLOR_FATAL = "\033[37;41;1m";

//Log style
string MSG_STYLE_PRESET;
string MSG_STYLE_INFO;
string MSG_STYLE_WARN;
string MSG_STYLE_ERROR;
string MSG_STYLE_FATAL;
string LOG_STYLE_PRESET;
string LOG_STYLE_INFO;
string LOG_STYLE_WARN;
string LOG_STYLE_ERROR;
string LOG_STYLE_FATAL;
void MsgStyleUpDate(){
    while(true){
        MSG_STYLE_PRESET= "["+getCurrentTime()+COLOR_BLUE+" PRESET"+COLOR_RESET+"] ";
        MSG_STYLE_INFO= "["+getCurrentTime()+COLOR_GREEN+" INFO"+COLOR_RESET+"] ";
        MSG_STYLE_WARN= "["+getCurrentTime()+COLOR_YELLOW+" WARN"+COLOR_RESET+"] ";
        MSG_STYLE_ERROR= "["+getCurrentTime()+COLOR_RED+" ERROR"+COLOR_RESET+"] ";
        MSG_STYLE_FATAL= "["+getCurrentTime()+COLOR_FATAL+" FATAL"+COLOR_RESET+"] ";
        LOG_STYLE_PRESET= "["+getCurrentTime()+" PRESET] ";
        LOG_STYLE_INFO= "["+getCurrentTime()+" INFO] ";
        LOG_STYLE_WARN= "["+getCurrentTime()+" WARN] ";
        LOG_STYLE_ERROR= "["+getCurrentTime()+" ERROR] ";
        LOG_STYLE_FATAL= "["+getCurrentTime()+" FATAL] ";
        Sleep(250);
    }
}


//Enable Console color support(Only on Windows)
void enableColorSupport() {
    #ifdef _WIN32
	cout << "[" << getCurrentTime() << " PRESET] " << "Enabling ConsoleColorSupport..."<< endl;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    cout << MSG_STYLE_PRESET << "ConsoleColorSupport Enabled"<< endl;
    #endif
    return;
}

#endif