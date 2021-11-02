#ifndef __Msocket_H__
#define __Msocket_H__

#ifdef __WINSOCKET_P__

#include <winsock2.h>
#include <windows.h>

#define s_addr S_un.S_addr

typedef int socklen_t;

#else

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SOCKET int
#define INVALID_SET_FILE_POINTER  -1
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket close

typedef struct sockaddr* LPSOCKADDR;
typedef struct sockaddr SOCKADDR;
typedef unsigned char byte;

#endif

#include <mutex>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <string>
using std::string;
using std::time;
using std::vector;
using std::mutex;


#define DATA_SIZE_IN_BYTE 20000
#define FLAG_SIZE 10
#ifdef QT_ENV_DEFINE
#include <QDebug>
#define printf qDebug
#endif
// #pragma commet(lib, "ws2_32.lib")
/** 说明：
 *      实现方法:
 *          1\ 设置ip、端口，并建立连接
 *              winsocks 流程：
 *                      1、 创建套接字
 *                      2、 绑定IP和端口
 *                      3、 监听阻塞
 *          2\ 读取数据的方法
 *          3\ etc
 */
class Mserver{
private:
    static Mserver* instance;
    Mserver() ;

    /** start
     *      绑定并开始监听端口
     */
    bool start();
    //  Start Listening
    bool start(bool ifShowMsg);

    /** 服务器信息
     *       sin_family             :   地址族
     *       sin_port               :   16位端口号
     *       sin_addr               :   32位ip地址
     *          S_un.S_addr         :   ip
     */
    struct sockaddr_in sin;
    SOCKET slisten;
    SOCKET* server_ = NULL;

    // 检查winsocks版本, 加载socket资源
    // 在Linux环境下不需要检测。
    bool wsainit() {
#ifdef __WINSOCKET_P__
        WORD sockVersion = MAKEWORD(2, 2);
        WSADATA wsaData;
        if (WSAStartup(sockVersion, &wsaData) != 0){
            return false;
        }
#endif
        return true;
    }

public:
///// Mserver setings /////
    void setAddr(u_long ipAddr);

    void setAddr(u_long ipAddr, u_short port);

    void setPort(u_short port);

///////////////////////////
    // grab instance
    static Mserver* getInstance();

    // Show server msg
    void display();

    // Accept clients
    SOCKET acceptClient(bool ifShowMsg, struct sockaddr_in &fClientAddr) ;

    // 关闭服务器
    void shutdown(){
        closesocket(*server_);
        server_ = NULL;

#ifdef __WINSOCKET_P__
        WSACleanup();
#endif
    }

    // TODO: write data into named pipes
};


class Msession{
private:
    char buffer[DATA_SIZE_IN_BYTE + FLAG_SIZE];
    static char FLAG[FLAG_SIZE];

    SOCKET client;
    struct sockaddr_in ClientAddr;
    // status
    bool isWorking;
    // Real Data
    // 4 times of real pack size
    short data[DATA_SIZE_IN_BYTE / 2] = {0x0000};
//    mutex read_mtx ;
    mutex write_mtx;
//    int read_cnt = 0; // 已加读锁个数
    size_t timeSpear;
public:
    // new session binding
    Msession(SOCKET client, struct sockaddr_in fClientAddr) {
        this->ClientAddr = fClientAddr;
        this->client = client;
        timeSpear = 0;
    }

    // seting of the working status
    void setStatus(bool ifWork) {
        isWorking = ifWork;
    }

    void closeSession() {
        closesocket(client);
    }

    bool getStatus(){
        return isWorking;
    }
    // read session
    void read() {
        memset(buffer, 0x00, sizeof (buffer));

        const int offset = 2;
        unsigned char isInit = offset;
        while(isWorking) {
            // make sure the client is alive.
            if (client == INVALID_SOCKET) {
                isWorking = false;
                break;
            }
            if(isInit == offset) printf("reset\n");
            // 找到FLAG，初始化设备连接
            while(isInit != sizeof(FLAG)) {
                // 逐个byte读
                char b[1];
                int datalen = recv(client, b, 1, 0);
                if(datalen > 0) {
                    if(b[0] == FLAG[isInit]) {
                        isInit ++;
                    }else if(b[0] == FLAG[offset]){
                        isInit = offset+1;
                    }else if(isInit != offset) {
                        isInit = offset;
                    }
                }else if(datalen == 0) {
                    fprintf(stderr,"Stuff %s went down!", inet_ntoa(ClientAddr.sin_addr));
                    fflush(stderr);
                    isWorking = false;
                    return;
                }else {
                    isWorking = false;
                    closesocket(client);
                    return;
                }
                // reset data
            }
            int totalLen = 0;
            int num = 10;
            while(totalLen < DATA_SIZE_IN_BYTE+FLAG_SIZE){
                // 准备就绪，读取DATA_SIZE_IN_BYTE+FLAG_SIZE个数据
                int datalen = recv(client, buffer + totalLen, num, 0);
                if(datalen == 0){
                    isWorking = false;
                    return;
                }else if(datalen < 0){
                    isWorking = false;
                    closesocket(client);
                    return;
                }
                totalLen += datalen;
                if(totalLen + 10 > DATA_SIZE_IN_BYTE+FLAG_SIZE){
                    num = DATA_SIZE_IN_BYTE+FLAG_SIZE - totalLen;
                }
            }

            // 数据完整性检查
            for (isInit = offset;isInit < sizeof (FLAG); isInit++ ) {
                if (buffer[DATA_SIZE_IN_BYTE + isInit] != FLAG[isInit]){
                    break;
                }
            }
            if(isInit != sizeof (FLAG)) {
                isInit = offset;
                printf("\nfail to sync! reseting... \n");
            }else{
                // 数据转换 写入加锁
                write_mtx.lock();
                for (size_t i = 0; i < DATA_SIZE_IN_BYTE / 2; i++) {
                    unsigned short tmp = (byte)buffer[i * 2];
                    tmp = tmp * 256;
                    tmp += (byte) buffer[i * 2 + 1];
                    data[i] = (short) tmp;
                }
                //
                timeSpear = time(NULL);
                write_mtx.unlock();
            }
        }

    }

    // get data
    bool fetchData(size_t &spear, short *res) {
        if (spear != timeSpear){
            write_mtx.lock();
            spear = timeSpear;
            for (int i = 0 ;i < DATA_SIZE_IN_BYTE / 2; i ++) {
                res[i] = data[i];
            }
            write_mtx.unlock();
            return true;
        }
        return false;
    }

    bool fetchData(size_t &spear, vector<float> &res) {
        if (spear != timeSpear){
            write_mtx.lock();
            spear = timeSpear;
            vector<float> tmp;
            res.swap(tmp);
            for (int i = 0 ;i < DATA_SIZE_IN_BYTE / 2; i ++) {
                res.push_back((float)data[i]);
            }
            write_mtx.unlock();
            return true;
        }
        return false;
    }

    ~Msession(){
        setStatus(false);
    }
};
//==================================
//  用于进程通信    【挫版】
//      计划之后修改为其他的方式。
//      由于一般在系统中与 Mserver 一同使用，故没有实现初始化方法。
//==================================
class MsgNotifier {
protected:
    void init(u_short port) {
        sout.sin_family = AF_INET;              //  设置地址族/协议族
        sout.sin_port = htons(port);            //  设置端口号
        sout.sin_addr.s_addr = INADDR_ANY;      //  设置回调地址 用于进程通信

    }

    //  开启socket以及端口连接
    bool start(){
        NotifierSocket = socket(AF_INET, SOCK_STREAM, 0);
        if ( NotifierSocket < 0) {
            perror ("Notifier socket creation failed!\n");
            return false;
        }
        if ( connect(NotifierSocket, (LPSOCKADDR)&sout, sizeof(sout)) < 0 ) {
            perror ("Notifier client creation failed!\n");
            return false;
        }
//        int flag = 1;
//        setsockopt(NotifierSocket, SOL_SOCKET, SO_KEEPALIVE, (void *)&flag, sizeof(int));
//        setsockopt(NotifierSocket, IPPROTO_TCP, TCP_NODELAY, (void *) &flag, sizeof(int));
        return true;
    }

    SOCKET NotifierSocket;
    struct sockaddr_in sout;    //  网络地址结构体
    bool isWorking;
    mutex workingLock;
public:
    MsgNotifier (){
        MsgNotifier(8188);
    }

    MsgNotifier (u_short port) {
        init(port);
        isWorking = start();
    }

    ~MsgNotifier(){
        setWorking(false);
    }

    bool setWorking(bool state) {
        workingLock.lock();
        if (state && !isWorking){
            isWorking = start();
            state = isWorking;
        } else if (!state && isWorking) {
            isWorking = state;
            closesocket(NotifierSocket);
        }
        workingLock.unlock();
        return state;
    }

    bool getisWorking() {
        return isWorking;
    }

    bool sendMsg(const char * str, size_t len){
        bool state;
        workingLock.lock();
        state = isWorking;
        if (isWorking){
            // auto slen = send(NotifierSocket, str.c_str(), len, 0);
            send(NotifierSocket, str, len+1, 0);
        }
        workingLock.unlock();
        return state;
    }

    void display(){
        // TODO：Display server info
        printf("Notify message binding on %s:%hu",inet_ntoa(sout.sin_addr), ntohs(sout.sin_port));
    }

    void setPort(u_short port){
        sout.sin_port = htons(port);
    }
};

//=============     TODO: 增加多个客户的需求    =============
// session 队列
typedef struct{
    int num;
    Msession* line;
} Services;

// Note: 注意内存管理
class DataCenter{
protected:
    Msession * m_session;
public:
    DataCenter(){m_session = nullptr;}
    DataCenter(Msession * csession):m_session(csession){}
    bool stop() {m_session->closeSession();}
    ~DataCenter(){m_session = nullptr;}
};
//=========================================================
#endif


