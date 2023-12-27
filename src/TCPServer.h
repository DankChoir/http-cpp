#pragma once
#include <netinet/in.h>

class TCPServer {
 private:
  int serverSocket;
  int port;
  sockaddr_in serverAddress;

 public:
  explicit TCPServer(int port);
  ~TCPServer();

  void start();

 private:
  void createSocket();
  void setupServerAddress();
  void bindSocket();
  void listenForConnections();
  void acceptConnections();
  void handleClient(int);
  void closeServerSocket();
};
