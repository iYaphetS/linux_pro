#pragma once
#include "header.h"

//创建服务器
int myServer(uint16_t port, const char *ip);
int myAccept(int server_fd, struct sockaddr *addr, socklen_t *addrlen);
//创建客户端
int myClient(uint16_t port, const char *ip);
//从管道读len大小的内容  向管道写len大小的内容
int myRead(int fd, char buf[], int len);
int myWrite(int fd, const char buf[], int len);


int myReadlen(int fd, uint32_t *len);
int myWritelen(int fd, uint32_t len);
//得到文件的大小
int getfilesize(const char *path);

char *myReadbuf(int fd);
void myWritebuf(int fd, const char *buf);
