//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

// Third-party includes
#include <NativeEthernet.h>

// Local includes
#include "EthernetNetworkHub.h"
#include "NetworkFactory.h"
#include "NetworkClient.h"
#include "NetworkClientWrapper.h"
#include "NetworkUDP.h"
#include "NetworkUDPWrapper.h"
#include "NetworkServer.h"
#include "NetworkServerWrapper.h"

// NetworkClientWrapper implementation for NativeEthernet EthernetClient.
//
class EthernetClientWrapper : public NetworkClientWrapper {
  public:
    int connect(IPAddress ip, uint16_t port) { return _ethernetClient.connect(ip, port); };
    int connect(const char *host, uint16_t port) { return _ethernetClient.connect(host, port); };
    size_t write(uint8_t b) { return _ethernetClient.write(b); };
    size_t write(const uint8_t *buf, size_t size) { return _ethernetClient.write(buf, size); };
    int available() { return _ethernetClient.available(); };
    int read() { return _ethernetClient.read(); };
    int read(uint8_t *buf, size_t size) { return _ethernetClient.read(buf, size); };
    int peek() { return _ethernetClient.peek(); };
    void flush() { _ethernetClient.flush(); };
    void stop() { _ethernetClient.stop(); };
    uint8_t connected() { return _ethernetClient.connected(); };
    operator bool() { return _ethernetClient ? true : false; };
    IPAddress remoteIP() { return _ethernetClient.remoteIP(); };
    uint16_t remotePort() { return _ethernetClient.remotePort(); };
    
  private:
    friend class EthernetNetworkHub;
    friend class EthernetServerWrapper;
    
    EthernetClientWrapper() {};
    EthernetClientWrapper(EthernetClient& ethernetClient) {
      _ethernetClient = ethernetClient;
    };
    
    EthernetClient _ethernetClient;
};

// NetworkServerWrapper implementation for NativeEthernet EthernetServer.
//
class EthernetServerWrapper : public NetworkServerWrapper {
  public:
    NetworkClient available() {
      EthernetClient ethernetClient = _ethernetServer.available();
      
      EthernetClientWrapper* clientWrapper = new EthernetClientWrapper(ethernetClient);
      
      return NetworkFactory::createNetworkClient(clientWrapper);
    };
    
    void begin() { _ethernetServer.begin(); };
    size_t write(uint8_t b) { return _ethernetServer.write(b); };
    size_t write(const uint8_t *buf, size_t size) { return _ethernetServer.write(buf, size); };
    
  private:
    friend class EthernetNetworkHub;
    
    EthernetServerWrapper(EthernetServer& ethernetServer) {
      _ethernetServer = ethernetServer;
    };
    
    EthernetServer _ethernetServer;
};

// NetworkUDPWrapper implementation for NativeEthernet EthernetUDP.
//
class EthernetUDPWrapper : public NetworkUDPWrapper {
  public:
    
    uint8_t begin(uint16_t port) { return _ethernetUDP.begin(port); };
    uint8_t beginMulticast(IPAddress ip, uint16_t port) { return _ethernetUDP.beginMulticast(ip, port); };
    void stop() { _ethernetUDP.stop(); };
    int beginPacket(IPAddress ip, uint16_t port) { return _ethernetUDP.beginPacket(ip, port); };
    int beginPacket(const char *host, uint16_t port) { return _ethernetUDP.beginPacket(host, port); };
    int endPacket() { return _ethernetUDP.endPacket(); };
    size_t write(uint8_t b) { return _ethernetUDP.write(b); };
    size_t write(const uint8_t *buffer, size_t size) { return _ethernetUDP.write(buffer, size); };
    int parsePacket() { return _ethernetUDP.parsePacket(); };
    int available() { return _ethernetUDP.available(); };
    int read() { return _ethernetUDP.read(); };
    int read(unsigned char* buffer, size_t len) { return _ethernetUDP.read(buffer, len); };
    int read(char* buffer, size_t len) { return _ethernetUDP.read(buffer, len); };
    int peek() { return _ethernetUDP.peek(); };
    void flush() { _ethernetUDP.flush(); };
    IPAddress remoteIP() { return _ethernetUDP.remoteIP(); };
    uint16_t remotePort() { return _ethernetUDP.remotePort(); };
    
  private:
    friend class EthernetNetworkHub;
    
    EthernetUDPWrapper(EthernetUDP& ethernetUDP) {
      _ethernetUDP = ethernetUDP;
    };
    
    EthernetUDP _ethernetUDP;
};

bool EthernetNetworkHub::begin(uint8_t *macAddress, Print* printer) {

  bool hadError = false;
  
  // start the Ethernet connection
  if (hasConfiguredLocalIPAddress()) {
    printer->print("Connecting with ip address ");
    printer->println(getConfiguredLocalIPAddress());
    Ethernet.begin(macAddress, getConfiguredLocalIPAddress(), getConfiguredDNSIPAddress(),
      getConfiguredGatewayIPAddress(), getConfiguredSubnetMask());
  } else {
    printer->println("Connecting with ip address from DHCP");
    int status = Ethernet.begin(macAddress);
    if (status == 0) {
      printer->println("Failed to configure Ethernet using DHCP");
      hadError = true;
    }
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    printer->println("Ethernet hardware was not found.");
    hadError = true;
  }
  
  if (Ethernet.linkStatus() == LinkOFF) {
    printer->println("Ethernet cable is not connected.");
    hadError = true;
  }
  
  return !hadError;
}

IPAddress EthernetNetworkHub::getLocalIPAddress() {
  return Ethernet.localIP();
}

NetworkClient EthernetNetworkHub::getClient() {
  EthernetClientWrapper* clientWrapper = new EthernetClientWrapper();
  
  return NetworkFactory::createNetworkClient(clientWrapper);
}

NetworkServer* EthernetNetworkHub::getServer(uint32_t portNum) {
  EthernetServer tcpServer(portNum);
  
  EthernetServerWrapper* serverWrapper = new EthernetServerWrapper(tcpServer);
  
  return NetworkFactory::createNetworkServer(serverWrapper);
}
    
NetworkUDP* EthernetNetworkHub::getUDP() {
  EthernetUDP udp;
  
  EthernetUDPWrapper* udpWrapper = new EthernetUDPWrapper(udp);
  
  return NetworkFactory::createNetworkUDP(udpWrapper);
}


void EthernetNetworkHub::printStatus(Print* printer) {
  printer->print("Hardware Status: ");
  switch(Ethernet.hardwareStatus()) {
    case EthernetNoHardware:
      printer->println("No Hardware");
      break;
      
    case EthernetW5100:
      printer->println("EthernetW5100");
      break;
      
    case EthernetW5200:
      printer->println("EthernetW5200");
      break;
      
    case EthernetW5500:
      printer->println("EthernetW5500");
      break;
  }
  
  printer->print("Link Status: ");
  switch (Ethernet.linkStatus()) {
    case Unknown:
      printer->println("Unknown");
      break;
      
    case LinkON:
      printer->println("Connected");
      break;

    case LinkOFF:
      printer->println("Not connected");
      break;
  }
  
  uint8_t macAddress[6];
  Ethernet.MACAddress(macAddress);
  printer->print("MAC Address: ");
  for (size_t x = 0; x < sizeof(macAddress); x++) {
    printer->print(macAddress[x], HEX);
    if (x < sizeof(macAddress) - 1) {
      printer->print(":");
    }
  }
  printer->println();
  
  printer->print("IP Address: ");
  printer->println(Ethernet.localIP());
  
  printer->print("Subnet Mask: ");
  printer->println(Ethernet.subnetMask());
  
  printer->print("Gateway IP: ");
  printer->println(Ethernet.gatewayIP());
  
  printer->print("DHCP Server IP: ");
  printer->println(Ethernet.dhcpServerIP());
  
  printer->print("DNS Server IP: ");
  printer->println(Ethernet.dnsServerIP());
}

// Static members and methods

EthernetNetworkHub* EthernetNetworkHub::_ethernetNetworkHub = NULL;

// Returns the instance of EthernetNetworkHub
EthernetNetworkHub EthernetNetworkHub::getInstance() {
  if (_ethernetNetworkHub == NULL) {
    _ethernetNetworkHub = new EthernetNetworkHub();
  }
  return *_ethernetNetworkHub;
};
    
