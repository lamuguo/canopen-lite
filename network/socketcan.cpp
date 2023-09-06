//
// Created by lamuguo on 9/5/23.
//
#include "socketcan.h"

#include <glog/logging.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>

namespace colite {

SocketCANNetwork::SocketCANNetwork(const char* channel) {
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Error while opening socket");
        exit(-1);
    }

    strcpy(ifr.ifr_name, channel);
    ioctl(s, SIOCGIFINDEX, &ifr);

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Error in socket bind");
        exit(-2);
    }
}

int SocketCANNetwork::send_message(uint32_t can_id, const std::vector<uint8_t>& data) {
    struct can_frame frame;
    frame.can_id = can_id;
    frame.can_dlc = data.size();
    memcpy(frame.data, data.data(), data.size());

    int nbytes = write(s, &frame, sizeof(struct can_frame));

    if (nbytes == -1) {
        PLOG(ERROR) << "Error sending CAN frame";
    } else if (nbytes != sizeof(struct can_frame)) {
        LOG(WARNING) << "Expected " << sizeof(struct can_frame) << " bytes, but only wrote " << nbytes << " bytes.";
    } else {
        LOG(INFO) << "Sent CAN frame with ID: " << std::hex << can_id;
    }
    return nbytes;
}

struct can_frame SocketCANNetwork::recv_message() {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(s, &readSet);

    struct timeval timeout;
    timeout.tv_sec = 1;  // 1 second timeout
    timeout.tv_usec = 0;

    int selectRes = select(s + 1, &readSet, NULL, NULL, &timeout);
    if (selectRes == -1) {
        PLOG(ERROR) << "Error during select";
        exit(-1);
    } else if (selectRes == 0) {
        LOG(WARNING) << "No CAN frame received in the allotted time.";
        struct can_frame emptyFrame = {};
        return emptyFrame;
    }

    struct can_frame frame;
    int nbytes = read(s, &frame, sizeof(struct can_frame));

    if (nbytes == -1) {
        PLOG(ERROR) << "Error reading CAN frame";
        exit(-1);
    }

    LOG(INFO) << "Received CAN frame with ID: " << std::hex << frame.can_id;

    return frame;
}

SocketCANNetwork::~SocketCANNetwork() {
    close(s);
}

}