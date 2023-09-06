//
// Created by lamuguo on 9/5/23.
//

#ifndef CANOPEN_LITE_SOCKETCAN_NETWORK_H
#define CANOPEN_LITE_SOCKETCAN_NETWORK_H

#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <memory>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace colite {

class SocketCANNetwork {
private:
    int s; // socket描述符
    struct sockaddr_can addr;
    struct ifreq ifr;

public:
    SocketCANNetwork(const char* channel);
    ~SocketCANNetwork();

    int send_message(uint32_t can_id, const std::vector<uint8_t>& data);
    std::unique_ptr<can_frame> recv_message();

};  // class SocketCANNetwork

}  // namespace colite

#endif //CANOPEN_LITE_SOCKETCAN_NETWORK_H
