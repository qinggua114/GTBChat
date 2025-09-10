#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <cstring>

#include "config.h"

using namespace std;

void shutDownSer();
bool cmdflag=0;
void CommandDeal(fstream *log,string Command){
    if(Command=="stop"){
        cout << MSG_STYLE_INFO << "Stopping server" << endl;
        *log << LOG_STYLE_INFO << "Stopping server" << endl;
        shutDownSer();
    }
    else if(Command == "list"){
        cout << MSG_STYLE_INFO << "There are " << Client_sum << " users on line now(Max user count is " << config.Max_client << ")" << endl;
    }
    else{
        cout << MSG_STYLE_WARN << "Unknown command: " << Command << endl;
        *log << LOG_STYLE_WARN << "Unknown command: " << Command << endl;
    }
    return;
}