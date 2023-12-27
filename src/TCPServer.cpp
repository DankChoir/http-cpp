#include "TCPServer.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

TCPServer::TCPServer(int port) : port(port) {}

TCPServer::~TCPServer() { closeServerSocket(); }

void TCPServer::start() {
  createSocket();
  setupServerAddress();
  bindSocket();
  listenForConnections();
  acceptConnections();
}

void TCPServer::createSocket() {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    perror("Error creating socket\n");
    exit(EXIT_FAILURE);
  }

  // Set option "SO_REUSEADDR" at level "SOL_SOCKET" to "resuse"
  // allows socket to reuse a local address immediately after the server is
  // closed, even if still in TIME_WAIT state
  int reuse = 1;
  int reuseFailure = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse,
                                sizeof(reuse)) < 0;

  if (reuseFailure) {
    perror("setsockopt(SO_REUSEADDR) failed");
    close(serverSocket);
    exit(EXIT_FAILURE);
  }
}

void TCPServer::setupServerAddress() {
  serverAddress.sin_family = AF_INET;  // IPv4 type
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
}

void TCPServer::bindSocket() {
  int bindFailure = bind(serverSocket, (struct sockaddr *)(&serverAddress),
                         sizeof(serverAddress)) < 0;

  if (bindFailure) {
    perror("Error binding socket\n");
    close(serverSocket);
    exit(EXIT_FAILURE);
  }
}

void TCPServer::listenForConnections() {
  const int connectQueueSize = 10;
  int listernFailure = listen(serverSocket, connectQueueSize) < 0;

  if (listernFailure) {
    perror("Error listening for Connections\n");
    close(serverSocket);
    exit(EXIT_FAILURE);
  }

  std::cout << "Server listening on port " << port << "...\n";
}

void TCPServer::acceptConnections() {
  while (true) {
    sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress,
                              &clientAddressSize);

    if (clientSocket < 0) {
      perror("Error accepting Connections\n");
      exit(EXIT_FAILURE);
    }

    handleClient(clientSocket);
  }
}

void TCPServer::handleClient(int clientSocket) {
  char buffer[1024];
  ssize_t bytesRead;

  char announce[] = "SERVER ECHO: ";

  // Receive and echo back data
  while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
    send(clientSocket, announce, static_cast<size_t>(sizeof(announce)), 0);
    send(clientSocket, buffer, static_cast<size_t>(bytesRead), 0);
  }

  close(clientSocket);
}

void TCPServer::closeServerSocket() {
  if (serverSocket != -1) {
    close(serverSocket);
    std::cout << "Server socket closed.\n";
  }
}
