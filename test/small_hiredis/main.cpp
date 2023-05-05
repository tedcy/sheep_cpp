#include "small_hiredis.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
 
int asyncFrameWork(const uint8_t* sendBuf, int sendBufLen, uint8_t* recvBuf, int* recvBufLen)
{
  if (sendBuf == NULL || recvBuf == NULL || sendBufLen < 1 || *recvBufLen < 1) {
    printf("asyncFrameWork param error.\n");
    return -1;
  }
 
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    printf("socket error : %d\n", errno);
    return -1;
  }
 
  struct sockaddr_in addr;
  memset(&addr, 0x00, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6379);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
  int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
  if (ret) {
    printf("connect error : %d\n", errno);
    close(fd);
    return -2;
  }
 
  ret = send(fd, sendBuf, sendBufLen, 0);
  if (ret < 0) {
    printf("send error : %d\n", errno);
    close(fd);
    return -3;
  }
 
  ret = recv(fd, recvBuf, *recvBufLen, 0);
  if (ret <= 0) {
    printf("recv error : %d\n", errno);
    close(fd);
    return -4;
  }
 
  *recvBufLen = ret;
  printf("asyncFrameWork recv len = %d\n", ret);
  close(fd);
  return 0;
}
 
int setCmd(const char* key, const char* value)
{
  char* cmdBuf = NULL;
  int len = 0;
  
  len = redisFormatCommand(&cmdBuf, "SET %s %s", key, value);
  if (len < 0) {
    printf("redisFormatCommand error! len = %d\n", len);
    return -1;
  }
 
  printf("redisFormatCommand result: len = %d, cmdBuf = %s\n", len, cmdBuf);
  
  int ret = 0;
  char recvBuf[1024] = {0};
  int recvBufLen = sizeof(recvBuf);
  ret = asyncFrameWork((const uint8_t*)cmdBuf, len, (uint8_t*)recvBuf, &recvBufLen);
  if (ret) {
    printf("asyncFrameWork error:%d!\n", ret);
    delete(cmdBuf);
    cmdBuf = NULL;
    return -2;
  }
  printf("asyncFrameWork return : recvBuf = %s, recvBufLen = %d\n", recvBuf, recvBufLen);
  delete(cmdBuf);
  cmdBuf = NULL;
 
  redisReader *reader = NULL;
  void *reply = NULL;
  reader = redisReaderCreate();
  if (reader == NULL) {
    printf("redisReaderCreate error!\n");
    return -3;
  }
  redisReaderFeed(reader, recvBuf, recvBufLen);
  ret = redisReaderGetReply(reader, &reply);
  if (ret != REDIS_OK) {
    printf("redisReaderGetReply error:%d!\n", ret);
    redisReaderFree(reader);
    reader = NULL;
    return -4;
  }
 
  if (reply == NULL) {
    printf("redisReply is NULL!\n");
    redisReaderFree(reader);
    reader = NULL;
    return -5;
  }
 
  printf("redisReader get reply : %d\n", *((int*)reply));
 
  struct redisReply *r = (struct redisReply*)reply;
  printf("r->type = %d\n", r->type);
  if (REDIS_REPLY_ERROR == r->type) {
    printf("redisReply return REDIS_REPLY_ERROR!\n");
    freeReplyObject(reply);
    redisReaderFree(reader);
    reader = NULL;
    return -6;
  }
  
  if (REDIS_REPLY_STATUS != r->type) {
    printf("redisReply return is't REDIS_REPLY_STATUS!\n");
    freeReplyObject(reply);
    redisReaderFree(reader);
    reader = NULL;
    return -7;
  }
 
  printf("-------------------------------------\n");
  if (r->integer == REDIS_OK)
    printf("set key(%s) OK\n", key);
  else
    printf("set key(%s) FAIL\n", key);
  printf("-------------------------------------\n");
 
  freeReplyObject(reply);
  redisReaderFree(reader);
 
  return 0;
}
 
int getCmd(const char* key)
{
  char* cmdBuf = NULL;
  int len = 0;
  
  len = redisFormatCommand(&cmdBuf, "GET %s", key);
  if (len < 0) {
    printf("redisFormatCommand error! len = %d\n", len);
    return -1;
  }
 
  printf("redisFormatCommand result: len = %d, cmdBuf = %s\n", len, cmdBuf);
  
  int ret = 0;
  char recvBuf[1024] = {0};
  int recvBufLen = sizeof(recvBuf);
  ret = asyncFrameWork((const uint8_t*)cmdBuf, len, (uint8_t*)recvBuf, &recvBufLen);
  if (ret) {
    printf("asyncFrameWork error:%d!\n", ret);
    delete(cmdBuf);
    cmdBuf = NULL;
    return -2;
  }
  printf("asyncFrameWork return : recvBuf = %s, recvBufLen = %d\n", recvBuf, recvBufLen);
  delete(cmdBuf);
  cmdBuf = NULL;
 
  redisReader *reader = NULL;
  void *reply = NULL;
  reader = redisReaderCreate();
  if (reader == NULL) {
    printf("redisReaderCreate error!\n");
    return -3;
  }
  redisReaderFeed(reader, recvBuf, recvBufLen);
  ret = redisReaderGetReply(reader, &reply);
  if (ret != REDIS_OK) {
    printf("redisReaderGetReply error:%d!\n", ret);
    redisReaderFree(reader);
    reader = NULL;
    return -4;
  }
 
  if (reply == NULL) {
    printf("redisReply is NULL!\n");
    redisReaderFree(reader);
    reader = NULL;
    return -5;
  }
 
  printf("redisReader get reply : %d\n", *((int*)reply));
 
  struct redisReply *r = (struct redisReply*)reply;
  printf("r->type = %d\n", r->type);
  if (REDIS_REPLY_ERROR == r->type) {
    printf("redisReply return REDIS_REPLY_ERROR!\n");
    freeReplyObject(reply);
    redisReaderFree(reader);
    reader = NULL;
    return -6;
  }
  
  if (REDIS_REPLY_STRING != r->type) {
 
    if (REDIS_REPLY_NIL == r->type) {
      printf("-------------------------------------\n");
      printf("key(%s) not exists!\n", key);
      printf("-------------------------------------\n");
    }
    else
      printf("redisReply return is't REDIS_REPLY_STRING!\n");
    freeReplyObject(reply);
    redisReaderFree(reader);
    reader = NULL;
    return -7;
  }
 
  printf("r->len = %d\n", r->len);
  printf("r->str = %s\n", r->str);
 
  printf("-------------------------------------\n");
  printf("key(%s), value=(%s)\n", key, r->str);
  printf("-------------------------------------\n");
 
  freeReplyObject(reply);
  redisReaderFree(reader);
 
  return 0;
}
 
int main(int argc, char** argv)
{
  setCmd("company", "leoox");
  getCmd("company");
  return 0;
}
