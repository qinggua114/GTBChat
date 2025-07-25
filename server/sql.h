#include <iostream>
#include <fstream>

#include "logcolor.h"

using namespace std;
class GTBSQL{
    private:
    struct USER{
        string UserName="";
        string PassWord="";
        int UUID=-1;
    };
    fstream dialog,SQLlog;
    public:
        void init(int usersum){
            USER user[usersum];
            dialog.open("dialogs.txt",ios::app);
            SQLlog.open("sqllog.txt",ios::app);
            cout << MSG_STYLE_PRESET << "SQL init successfully" <<endl;
            SQLlog << LOG_STYLE_INFO << "SQL start successfully" <<endl;
        }
        void closesql(){
            cout << MSG_STYLE_INFO << "SQL closed" <<endl;
            SQLlog << LOG_STYLE_INFO << "SQL closed" <<endl;
            dialog.close();
            SQLlog.close();
        }
        int test;
        
};
