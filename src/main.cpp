// Copyright Danko Fox 2023
#include "TCPServer.h"

int main(int argc, char *argv[]) {
  TCPServer tcpserver(8080);
  tcpserver.start();
  return 0;
}
