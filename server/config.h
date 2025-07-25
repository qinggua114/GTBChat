#include <iostream>
#include <string>
#include <fstream>

#include "logcolor.h"

using namespace std;

fstream cfg;
int Client_sum=0;

struct CFG{
    int port=1147;
    int Max_client=20;
};
CFG config;

void readcfg(fstream *log){
    string cfgtmp;
    cfg.open( "server.properties" , ios::out | ios::in);
	cout << "["+get_current_time()+"\033[34m"+" PRESET"+"\033[0m"+"] " << "Loading config..." << endl;
    *log << LOG_STYLE_PRESET << "Loading config..." << endl;
    cfg << "#GTB Chat Server Config" << endl;
    while (cfg >> cfgtmp){
        if( cfgtmp == "#End_Cfg" ){
            cout << MSG_STYLE_PRESET << "Config loading completed" << endl;
            *log << LOG_STYLE_PRESET << "Config loading completed" << endl;
            break;
        }
        if( !cfgtmp.compare(0,4,"port") ){
            config.port = stoi(cfgtmp.substr(5,5));
        }
        if( !cfgtmp.compare(0,10,"Max_client") ){
            config.Max_client = stoi(cfgtmp.substr(11,4));
        }       
    }
    return;
}