#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <fstream>

#include "sql.h"
#include "command.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
//define close closesocket
#define SHUT_RDWR SD_BOTH

using namespace std;

//Log
fstream log;


//Config

int usersum=0;



//Global
string Massage="";
struct USER{
    string UserName="";
    string PassWord="";
    string UUID="";
    char* IP="";
    int Port=-1;
    bool Logged=false;
};

string DealDATA(char* buffer, USER *user);


//SQL
GTBSQL SQL;



/*================WeboPeration================*/

SOCKET server_fd = INVALID_SOCKET;

//INIT WSA(Only on Windows)
bool init_network() {
    #ifdef _WIN32
	cout << MSG_STYLE_PRESET << "Starting up WSA..." << endl;
    log << LOG_STYLE_PRESET << "Starting up WSA..." << endl;
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << MSG_STYLE_FATAL << "WSAStartup Failed: " << WSAGetLastError() << endl;
        log << LOG_STYLE_FATAL << "WSAStartup Failed: " << WSAGetLastError() << endl;
        return false;
    }
    cout << MSG_STYLE_PRESET << "WSA started up successfully" << endl;
    log << LOG_STYLE_PRESET << "WSA started up successfully" << endl;
    #endif
    return true;
}


//Clean up WSA(Only on Windows)
void cleanup_network() {
    #ifdef _WIN32
    WSACleanup();
    #endif
    return;
}


//Create TCP server
SOCKET create_tcp_server(int port) {
	
	cout << MSG_STYLE_INFO << "Creating Socket..." << endl;
    log << LOG_STYLE_INFO << "Creating Socket..." << endl;
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        cerr << MSG_STYLE_FATAL << "Failed to create socket" << endl;
        log <<LOG_STYLE_FATAL << "Failed to create socket" << endl;
        return INVALID_SOCKET;
    }
	cout << MSG_STYLE_INFO << "Create Socket successfully" << endl;
    log << LOG_STYLE_INFO << "Create Socket successfully" << endl;
	
    // Set SO_REUSEADDR Options
    cout << MSG_STYLE_INFO << "Setting SO_REUSEADDR Options..."<< endl;
    log << LOG_STYLE_INFO << "Setting SO_REUSEADDR Options..."<< endl;
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, 
                  (const char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        cerr << MSG_STYLE_FATAL << "Failed to set SO_REUSEADDR" << endl;
        log << LOG_STYLE_FATAL << "Failed to set SO_REUSEADDR" << endl;
        close(server_fd);
        return INVALID_SOCKET;
    }
    
    // Set Address and Port
    cout << MSG_STYLE_INFO << "Setting Address and port..."<< endl;
    log << LOG_STYLE_INFO << "Setting Address and port..."<< endl;
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << MSG_STYLE_ERROR << "Failed to bind to port:" << port << endl;
        log << LOG_STYLE_ERROR << "Failed to bind to port:" << port << endl;
        close(server_fd);
        return INVALID_SOCKET;
    }

    //Start listening
    if (listen(server_fd, 10) == SOCKET_ERROR) {
        cerr << MSG_STYLE_ERROR << "Failed to listen to port:" << port << endl;
        log << LOG_STYLE_ERROR << "Failed to listen to port:" << port << endl;
        close(server_fd);
        return INVALID_SOCKET;
    }

    cout << MSG_STYLE_INFO <<"Server start at 127.0.0.1:" << port << endl;
    log << LOG_STYLE_INFO <<"Server start at 127.0.0.1:" << port << endl;
    return server_fd;
}


//Deal with client connection
void handle_client(SOCKET client_fd, sockaddr_in client_addr) {
    USER user;
    USER *userp=&user;
    char* client_ip = inet_ntoa(client_addr.sin_addr);
    if (!client_ip) {
        client_ip = "unknown";
        cerr << MSG_STYLE_ERROR << "Failed to get client IP" << endl;
        log << LOG_STYLE_ERROR << "Failed to get client IP" << endl;
    }
    int client_port = ntohs(client_addr.sin_port);
    
    cout << MSG_STYLE_INFO << "Client " << client_ip << ":" << client_port << " connected to the server" << endl;
    log << LOG_STYLE_INFO << "Client " << client_ip << ":" << client_port << " connected to the server" << endl;
    
//Set Receive timeout
    #ifdef _WIN32
    DWORD timeout = 5000/*ms*/;
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    #else
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    #endif

// Receive and Send Data
    char buffer[1024];
    while (true) {
        if(Massage=="stop"){
            const char* msg = "Lost connection:\nServer Closed.";
            send(client_fd, msg, strlen(msg), 0);
            break;
        }
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            //When received data
            cout << MSG_STYLE_INFO << "Massage From" << client_ip << ":" << client_port << buffer << endl;
            log << LOG_STYLE_INFO << "Massage From" << client_ip << ":" << client_port << buffer << endl;
            //Deal
            string result=DealDATA(buffer,userp);
            if(result[0]=='!'){
                cout<< MSG_STYLE_WARN << "Cannot analyze the massage from" << client_ip << ":" << result <<endl;
                log << LOG_STYLE_WARN << "Cannot analyze the massage from" << client_ip << ":" << result <<endl;
                send(client_fd, result.c_str(), result.size(), 0);
            }
            else{
                cout << MSG_STYLE_INFO << client_ip << ":" << result << endl;
                log << LOG_STYLE_INFO << client_ip << ":" << result << endl;
            }
/*           string response = "Server Info:";
            response += buffer;
            send(client_fd, response.c_str(), response.size(), 0);*/
        } 
        else if (bytes_received == 0) {
            //Connection closed
            cout << MSG_STYLE_INFO <<"Client " << client_ip << ":" << client_port << "lost connection" << endl;
            log << LOG_STYLE_INFO <<"Client " << client_ip << ":" << client_port << "lost connection" << endl;
            break;
        } 
        else {
            //Receiving error
            int error = 0;
            #ifdef _WIN32
            error = WSAGetLastError();
            if (error == WSAETIMEDOUT) {
                //Timeout continue waitting
                continue;
            }
            #else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                //Timeout continue waitting
                continue;
            }
            #endif
            cerr << MSG_STYLE_WARN << "Error happend when receive:" << error << endl;
            log << LOG_STYLE_WARN << "Error happend when receive:" << error << endl;
//            break;
        }
    }
    
    //Close connection
    shutdown(client_fd, SHUT_RDWR);
    closesocket(client_fd);
    return;
}

/*================DealCliData================*/

string tmpstr[10];
void strcut(string iptstr){
    if( iptstr.length() > 1024 ){
        cout<< MSG_STYLE_WARN << "[StrCut] String is too long to cut!" << endl;
        log << LOG_STYLE_WARN << "[StrCut] String is too long to cut!" << endl;
        return;
    }
    int i=0;
    int j=0;
    while( iptstr[i] != '\0' and iptstr[i-1] != '{' ) i++;
    for(;i<1024;i++)
    {
    	char tmp[30]="";
        int k=0;
        while( iptstr [i] != ',' and iptstr[i]!='\0' and iptstr[i] != '}' ){
            tmp[k]=iptstr[i];
            k++;
            i++;
        }
        tmpstr[j]=tmp;
        j++;
        if(iptstr[i]=='\0' or iptstr[i]=='}') break;
    }
    return;
}
void Cleartmpstr(){
    for(int i=0;i<10;i++) tmpstr[i]="";
    return;
}
void Deallogin(USER *tmpuser,string username,string password){
    USER user = *tmpuser;
    ;
    *tmpuser = user;
    return;
}
string Deal_Clicmd(USER *tmpuser,string clicmd,int cmdlen,string cliinfo){
    USER user = *tmpuser;
    if(user.Logged){
        if(clicmd=="login"){
            return "!User was already logged in";
        }
        else return "!Unknown operation";
    }
    else if(clicmd=="login"){
        strcut(cliinfo);
        Deallogin(tmpuser,tmpstr[0],tmpstr[1]);
    }
    else {
        return "!Unknown operation";
    }
    return "";
}
string DealDATA(char* buffer,USER *user){
    string client_info,clicmd="";
    client_info=buffer;
    int i=0;
    while(client_info[i]!=']'){
        clicmd[i]=client_info[i];
        i++;
    }
    return Deal_Clicmd(user,clicmd,i-1,client_info);
}


/*================Command================*/

string Command="";
void InputCMD()
{
    while(Massage!="stop")
    {
        cout << ">";
        cin>>Command;
        CommandDeal(&log,Command);
    }
    return;
}


/*================StopServer================*/
void ShutDownSer(){
    Massage="stop";
    if (server_fd != INVALID_SOCKET) {
        cout << MSG_STYLE_INFO << "Closing Socket" << endl;
        log << LOG_STYLE_INFO << "Closing Socket" << endl;
        #ifdef _WIN32
        shutdown(server_fd, SD_RECEIVE);
        closesocket(server_fd);
        #else
        shutdown(server_fd, SHUT_RD);
        close(server_fd);
        #endif
        server_fd = INVALID_SOCKET;
    }
    return;
}


/*================Main function================*/

int main() {

//Log
    log.open("logs\\log.txt",ios::app);

//Massage style up date
    thread MSU(MSG_STYLE_UP_DATE);
    MSU.detach();

//Early perpare
    cout << "Starting Server..." << endl;
    log << "Starting Server..." << endl;
    enableColorSupport();


//Config
    readcfg(&log);

//Init SQL
    SQL.init(usersum);

//INIT WSA(Only on Windows)
    if (!init_network()) {
        return 1001;
    }
    
//Create Socket
    server_fd = create_tcp_server(config.port);
    if (server_fd == INVALID_SOCKET) {
        cleanup_network();
        system("pause");
        return 1002;
    }

//Coammand input
    thread IPTCMD(InputCMD);
    IPTCMD.detach();

//Main loop
    while (true) {
        
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        SOCKET client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if(Massage=="stop"){
            if (client_fd != INVALID_SOCKET){
                #ifdef _WIN32
                closesocket(client_fd);
                #else
                close(client_fd);
                #endif
            }
            break;
        }
        if (client_fd == INVALID_SOCKET) {
            cerr << MSG_STYLE_WARN << "Cannot accept the connection" << endl;
            log << LOG_STYLE_WARN << "Cannot accept the connection" << endl;
            continue;
        }
        
        //Create new thread for each client
        if(Client_sum<config.Max_client){
            thread(handle_client, client_fd, client_addr).detach();
            Client_sum++;
        }
        else{
            cout << MSG_STYLE_WARN << "Refuse connection,Client count reach the limit (" << config.Max_client << ")" << endl;
            log << LOG_STYLE_WARN << "Refuse connection,Client count reach the limit (" << config.Max_client << ")" << endl;
            const char* msg = "Server is busy,please try again later.";
            send(client_fd, msg, strlen(msg), 0);
            close(client_fd);
        }
    }

    if (server_fd != INVALID_SOCKET) {
        #ifdef _WIN32
        closesocket(server_fd);
        #else
        close(server_fd);
        #endif
    }   
    cleanup_network();
    SQL.closesql();
    cout << MSG_STYLE_INFO << "Server closed" << endl;
    log << LOG_STYLE_INFO << "Server closed" << endl;
    log.close();
    system("pause");
    return 0;
}