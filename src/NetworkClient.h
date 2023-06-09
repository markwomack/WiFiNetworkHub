//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <Client.h>
#include "NetworkClientWrapper.h"

// The client used to interact with data sent to
// a server. Instances of NetworkClient will be
// returned from calls to NetworkHub.getClient and
// NeworkServer.available().
//
class NetworkClient : public Client {
  public:
    int connect(IPAddress ip, uint16_t port) { return _clientWrapper->connect(ip, port); };
    int connect(const char *host, uint16_t port) { return _clientWrapper->connect(host, port); };
    size_t write(uint8_t b) { return _clientWrapper->write(b); };
    size_t write(const uint8_t *buf, size_t size) { return _clientWrapper->write(buf, size); };
    int available() { return _clientWrapper->available(); };
    int read() { return _clientWrapper->read(); };
    int read(uint8_t *buf, size_t size) { return _clientWrapper->read(buf, size); };
    int peek() { return _clientWrapper->peek(); };
    void flush() { _clientWrapper->flush(); };
    void stop() { _clientWrapper->stop(); };
    uint8_t connected() { return _clientWrapper->connected(); };
    operator bool() { return *_clientWrapper ? true : false; };
    IPAddress remoteIP() { return _clientWrapper->remoteIP(); };
    uint16_t remotePort() { return _clientWrapper->remotePort(); };
    
    // copy assignment
    NetworkClient& operator=(const NetworkClient& other) {
      // Guard self assignment
      if (this == &other) {
          return *this;
      }
      
      _clientWrapper = other._clientWrapper->clone();
      
      return *this;
    }
    
    // This constructor can be used to declare variables, but
    // NetworkHub.createClient() or NetworkServer.available() should be
    // used to assign a usable version.
    NetworkClient() {
      _clientWrapper = new NullNetworkClientWrapper();
    }
    
    ~NetworkClient() {
      delete _clientWrapper;
    };
    
  protected:
    NetworkClientWrapper* _clientWrapper;
    
  private:
    friend class NetworkFactory;
    
    NetworkClient(NetworkClientWrapper* clientWrapper) {
      _clientWrapper = clientWrapper;
    };
};

#endif // NETWORKCLIENT_H
