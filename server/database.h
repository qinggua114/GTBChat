#include <iostream>
#include <fstream>

#include "logcolor.h"


struct USER{
    string userName = "";
    string passWord = "";
    string UUID = "";
    string rtvalue = "";
    char* IP = "";
    int Port = -1;
    bool Logged = false;
    bool deleted = false;
};
using namespace std;
class GTBDB{
    private:
        fstream dialog,DBlog,udat;
        USER userdata[32767];
        short udatCount=0;
    public:
        void init(){
            dialog.open( "dialogs.txt" , ios::app);
            DBlog.open( dbLogPath , ios::app);
            udat.open( "database\\UserData.db" , ios::out | ios::in);
            cout << MSG_STYLE_PRESET << "Database init successfully" <<endl;
            DBlog << LOG_STYLE_INFO << "Database start successfully" <<endl;
        }
        void closedb(){
            cout << MSG_STYLE_INFO << "Database closed" <<endl;
            DBlog << LOG_STYLE_INFO << "Database closed" <<endl;
            dialog.close();
            DBlog.close();
        }

        void readDb(){
            short serial;
            while(true){
                udat >> serial;
                if(serial == -1) break;
                udat >> userdata[serial].userName;
                udat >> userdata[serial].passWord;
                udat >> userdata[serial].UUID;
                udat >> userdata[serial].deleted;
                udatCount += 1;
            }
        }
        void shearch(USER* userp,string type,string key){
            USER user=*userp;
            if(type == "uname"){
                for(int i=0;i<udatCount;i++){
                    if(userdata[i].deleted) continue;
                    if(userdata[i].userName == key){
                        *userp = userdata[i];
                        return;
                    }
                }
                user.rtvalue = "Connot find user called \"" + key + "\"";
            }

            if(type == "uuid"){
                for(int i=0;i<udatCount;i++){
                    if(userdata[i].deleted) continue;
                    if(userdata[i].UUID == key){
                        *userp = userdata[i];
                        return;
                    }
                }
                user.rtvalue = "The user with uid \"" + key + "\"was not found";
            }

            *userp = user;
            return;
        }
};
