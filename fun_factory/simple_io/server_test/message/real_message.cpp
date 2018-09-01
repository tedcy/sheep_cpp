#include "real_message.h"
#include <iostream>
using namespace std;

ssize_t RealMessage::ReqHeaderLen() {
    return 1;
}
ssize_t RealMessage::ReqBodyLen(char *recvMsg, int recvLen) {
    return 5;
}
void RealMessage::Service(char *recvMsg, ssize_t recvLen, char **sendMsg, ssize_t* needSendLen) {
    cout << recvMsg << endl;
    *sendMsg = (char*)"hello back";
    *needSendLen = 11;
    return;
}
