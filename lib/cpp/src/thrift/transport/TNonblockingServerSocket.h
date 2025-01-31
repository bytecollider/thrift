/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _THRIFT_TRANSPORT_TNONBLOCKINGSERVERSOCKET_H_
#define _THRIFT_TRANSPORT_TNONBLOCKINGSERVERSOCKET_H_ 1

#include <thrift/transport/TNonblockingServerTransport.h>
#include <thrift/transport/PlatformSocket.h>

#ifdef THRIFT_NO_SERVER
#error "Trying to build a server with THRIFT_NO_SERVER defined."
#endif

namespace apache {
namespace thrift {
namespace transport {

class TSocket;

/**
 * Nonblocking Server socket implementation of TNonblockingServerTransport. Wrapper around a unix
 * socket listen and accept calls.
 *
 */
class TNonblockingServerSocket : public TNonblockingServerTransport {
public:
  typedef std::function<void(THRIFT_SOCKET fd)> socket_func_t;

  const static int DEFAULT_BACKLOG = 1024;

  /**
   * Constructor.
   *
   * @param port    Port number to bind to
   */
  TNonblockingServerSocket(int port);

  /**
   * Constructor.
   *
   * @param port        Port number to bind to
   * @param sendTimeout Socket send timeout
   * @param recvTimeout Socket receive timeout
   */
  TNonblockingServerSocket(int port, int sendTimeout, int recvTimeout);

  /**
   * Constructor.
   *
   * @param address Address to bind to
   * @param port    Port number to bind to
   */
  TNonblockingServerSocket(const std::string& address, int port);

  /**
   * Constructor used for unix sockets.
   *
   * @param path Pathname for unix socket.
   */
  TNonblockingServerSocket(const std::string& path);

  ~TNonblockingServerSocket() override;

  bool isOpen() const;

  void setSendTimeout(int sendTimeout);
  void setRecvTimeout(int recvTimeout);

  void setAcceptBacklog(int accBacklog);

  void setRetryLimit(int retryLimit);
  void setRetryDelay(int retryDelay);

  void setKeepAlive(bool keepAlive) { keepAlive_ = keepAlive; }

  void setTcpSendBuffer(int tcpSendBuffer);
  void setTcpRecvBuffer(int tcpRecvBuffer);

  // listenCallback gets called just before listen, and after all Thrift
  // setsockopt calls have been made.  If you have custom setsockopt
  // things that need to happen on the listening socket, this is the place to do it.
  void setListenCallback(const socket_func_t& listenCallback) { listenCallback_ = listenCallback; }

  // acceptCallback gets called after each accept call, on the newly created socket.
  // It is called after all Thrift setsockopt calls have been made.  If you have
  // custom setsockopt things that need to happen on the accepted
  // socket, this is the place to do it.
  void setAcceptCallback(const socket_func_t& acceptCallback) { acceptCallback_ = acceptCallback; }

  THRIFT_SOCKET getSocketFD() override { return serverSocket_; }

  int getPort() override;

  int getListenPort() override;

  std::string getPath() const;

  bool isUnixDomainSocket() const;

  void listen() override;
  void close() override;

protected:
  std::shared_ptr<TSocket> acceptImpl() override;
  virtual std::shared_ptr<TSocket> createSocket(THRIFT_SOCKET client);

private:
  void _setup_sockopts();
  void _setup_unixdomain_sockopts();
  void _setup_tcp_sockopts();

  int port_;
  int listenPort_;
  std::string address_;
  std::string path_;
  THRIFT_SOCKET serverSocket_;
  int acceptBacklog_;
  int sendTimeout_;
  int recvTimeout_;
  int retryLimit_;
  int retryDelay_;
  int tcpSendBuffer_;
  int tcpRecvBuffer_;
  bool keepAlive_;
  bool listening_;

  socket_func_t listenCallback_;
  socket_func_t acceptCallback_;
};
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_TNONBLOCKINGSERVERSOCKET_H_
