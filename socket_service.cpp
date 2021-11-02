#include "socket_service.h"
////// class Mserver ///////
Mserver::Mserver() {
    // DEFAULTS {AF_INET, 8888, INADDR_ANY(inet_addr('0.0.0.0'))}
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8999);
    setAddr(INADDR_ANY);
}

bool Mserver::start(){
    return start(false);
}

bool Mserver::start(bool ifShowMsg){
    //  显示服务器信息
    if(ifShowMsg)
        display();
    //  创建套接字
    printf("Init Socket...\n");
    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SET_FILE_POINTER) {
        printf("Socket Error !\n");
        return false;
    }
    printf("Binding Port On %hu...\n", ntohs(sin.sin_port));

//    struct linger nolinger;
//    nolinger.l_onoff = 0; // 设置不等待直接释放资源
//    setsockopt(slisten,SOL_SOCKET,SO_LINGER,&nolinger,sizeof(struct linger));

    //  绑定端口
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
        printf("Bind Error ！\n");
        closesocket(slisten);
        return false;
    }
    printf("Start Listening...\n");
    //  监听端口
    if(listen(slisten, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen Error!\n");
        closesocket(slisten);
        return false;
    }
    //  准备工作完成
    printf("Ready to accept clients !\n");
    server_ = &slisten;
    return true;
}

void Mserver::setAddr(u_long ipAddr){
    sin.sin_addr.s_addr = ipAddr;
}

void Mserver::setAddr(u_long ipAddr, u_short port){
    setAddr(ipAddr);
    sin.sin_port = htons(port);
}

void Mserver::setPort(u_short port) {
    sin.sin_port = htons(port);
}

Mserver* Mserver::getInstance(){
    if(instance == NULL){
        instance = new Mserver();
    }
    return instance;
}

void Mserver::display() {
    // TODO：Display server info
    printf("%s",inet_ntoa(sin.sin_addr));
}

SOCKET Mserver::acceptClient(bool ifShowMsg, struct sockaddr_in &fClientAddr){
    if (server_ == NULL) {
        wsainit();
        start(ifShowMsg);
    }
    int nAddrlen = sizeof(fClientAddr);
    SOCKET acClient = accept(*server_, (SOCKADDR *)&fClientAddr,(socklen_t *) &nAddrlen);

//    struct linger nolinger;
//    nolinger.l_onoff = 0; // 设置不等待直接释放资源
//    setsockopt(acClient,SOL_SOCKET,SO_LINGER,&nolinger,sizeof(struct linger));
    return acClient;
}

Mserver* Mserver::instance = NULL;
/////// Msession ///////
char Msession::FLAG[] = {
                        (char) 0xAA,
                        (char) 0xAA,
                        (char) 0xAA,
                        (char) 0xAA,
                        (char) 0x00,
                        (char) 0x00,
                        (char) 0x00,
                        (char) 0xBB,
                        (char) 0xBB,
                        (char) 0xBB
                };
