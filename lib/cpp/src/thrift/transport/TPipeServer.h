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

#ifndef _THRIFT_TRANSPORT_TSERVERWINPIPES_H_
#define _THRIFT_TRANSPORT_TSERVERWINPIPES_H_ 1

#include <memory>
#include <thrift/transport/TServerTransport.h>
#ifndef _WIN32
#include <thrift/transport/TServerSocket.h>
#endif

#ifdef THRIFT_NO_SERVER
#error "Trying to build a server with THRIFT_NO_SERVER defined."
#endif

#define TPIPE_SERVER_MAX_CONNS_DEFAULT PIPE_UNLIMITED_INSTANCES

// Windows - set security to allow non-elevated apps
// to access pipes created by elevated apps.
// Full access to everyone
const std::string DEFAULT_PIPE_SECURITY{"D:(A;;FA;;;WD)"};

namespace apache {
namespace thrift {
namespace transport {

/**
 * Windows Pipes implementation of TServerTransport.
 * Don't destroy a TPipeServer at global scope, as that will cause a thread join
 * during DLLMain.  That also means that TServer's using TPipeServer shouldn't be at global
 * scope.
 */
#ifdef _WIN32
class TPipeServerImpl;
class TPipe;

class TPipeServer : public TServerTransport {
public:
  // Constructors
  // Named Pipe -
  TPipeServer(const std::string& pipename, uint32_t bufsize);
  TPipeServer(const std::string& pipename, uint32_t bufsize, uint32_t maxconnections);
  TPipeServer(const std::string& pipename,
              uint32_t bufsize,
              uint32_t maxconnections,
              const std::string& securityDescriptor);
  TPipeServer(const std::string& pipename);
  // Anonymous pipe -
  TPipeServer(int bufsize);
  TPipeServer();

  // Destructor
  virtual ~TPipeServer();

  bool isOpen() const override;

  // Standard transport callbacks
  void interrupt() override;
  void close() override;
  void listen() override;

  // Accessors
  std::string getPipename();
  void setPipename(const std::string& pipename);
  int getBufferSize();
  void setBufferSize(int bufsize);
  HANDLE getPipeHandle(); // Named Pipe R/W -or- Anonymous pipe Read handle
  HANDLE getWrtPipeHandle();
  HANDLE getClientRdPipeHandle();
  HANDLE getClientWrtPipeHandle();
  bool getAnonymous();
  void setAnonymous(bool anon);
  void setMaxConnections(uint32_t maxconnections);
  void setSecurityDescriptor(const std::string& securityDescriptor);

  // this function is intended to be used in generic / template situations,
  // so its name needs to be the same as TPipe's
  HANDLE getNativeWaitHandle();

protected:
  virtual std::shared_ptr<TTransport> acceptImpl();

private:
  std::shared_ptr<TPipeServerImpl> impl_;

  std::string pipename_;
  std::string securityDescriptor_;
  uint32_t bufsize_;
  uint32_t maxconns_;
  bool isAnonymous_;
};
#else //_WIN32
//*NIX named pipe implementation uses domain socket
typedef TServerSocket TPipeServer;
#endif
}
}
} // apache::thrift::transport

#endif // #ifndef _THRIFT_TRANSPORT_TSERVERWINPIPES_H_
