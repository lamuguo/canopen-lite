//
// Created by lamuguo on 9/6/23.
//

#ifndef CANOPEN_LITE_CANOPEN_NODE_H
#define CANOPEN_LITE_CANOPEN_NODE_H

#include "sdo_client.h"
#include "od/eds.h"

namespace colite {
    // This is just a SDO processor, not a SDO server. the server functionalities
    // are included in Node.
    // TODO(lamuguo): Rename the class.
    class SDOServer {
    public:
        SDOServer(int node_id, ObjectDictionary& od) : node_id_(node_id), od_(od) {}

        std::unique_ptr<can_frame> process_request(const struct can_frame& frame) {
            auto response = std::make_unique<can_frame>();

            // Simplified logic to handle expedited upload request and fetch data from OD
            if(frame.data[0] == 0x40) {
                int index = (static_cast<int>(frame.data[2]) << 8) | frame.data[1];
                int subindex = frame.data[3];

                auto variable = od_.get_variable(index, subindex);
                uint32_t value = variable->default_value.get<uint32_t>();  // Assuming int32_t for simplicity

                response->data[0] = 0x43;  // Positive expedited upload response
                response->data[1] = frame.data[1];
                response->data[2] = frame.data[2];
                response->data[3] = frame.data[3];
                std::memcpy(&response->data[4], &value, sizeof(int32_t));
            } else {
                // Handle other SDO requests or return an error
            }

            return response;
        }

    private:
        int node_id_;
        ObjectDictionary& od_;
    };

    class CANopenNode {
    public:
        CANopenNode(const char* channel, int node_id, const std::string& eds_path)
                : node_id_(node_id),
                  od_(import_eds(eds_path, node_id)),
                  sdo_server_(node_id, od_),
                  can_socket_(channel) {}  // Assuming "can0" as the CAN interface, modify as needed

        std::unique_ptr<can_frame> process_frame(const struct can_frame& frame) {
            if (frame.can_id == 0x600 + node_id_) {
                return sdo_server_.process_request(frame);
            }
            // Handle other types of frames or return a default frame / error

            // TODO(lamuguo): more logic for the placeholder.
            return std::make_unique<can_frame>();
        }

        void run() {
            while (true) {
                auto frame = can_socket_.recv_message();
                if ((frame->can_id & 0x7F) == node_id_) {
                    auto response = process_frame(*frame);
                    std::vector<uint8_t> data(response->data, response->data + sizeof(response->data));
                    can_socket_.send_message(0x580 + node_id_, data);
                }
            }
        }

    private:
        int node_id_;
        SDOServer sdo_server_;
        ObjectDictionary od_;
        SocketCANNetwork can_socket_;
    };
}

#endif //CANOPEN_LITE_CANOPEN_NODE_H
