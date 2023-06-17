//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef NETWORKHUB_H
#define NETWORKHUB_H

#include <Arduino.h>

#include "NetworkServer.h"
#include "NetworkUDP.h"

// Generic interface for the network hub. Implementations
// must implement the methods. The objects used to interact
// with the network are created by calls to the hub, and
// should not be created independently.
class NetworkHub {
  public:
    // Set a fixed IP address for the host. This is an
    // optional call, otherwise an IP address will be
    // assigned from the DHCP server.
    virtual void setHostIPAddress(IPAddress hostIPAddress) = 0;
    
    // Open a TCP port on the given port number.
    // Returns a NetworkServer for use.
    virtual NetworkServer* getServer(uint32_t portNum) = 0;

    // Open a UDP port on the given port number.
    // Returns a NetworkUDP for use.
    virtual NetworkUDP* getUDP(uint32_t portNum) = 0;
    
    // Print the status of the hub to the given
    // Print object (ie Serial).
    virtual void printStatus(Print* printer) = 0;
    
  protected:
    NetworkHub() { /* Nothing to see here, move along. */ }
};

#endif // NETWORKHUB_H
