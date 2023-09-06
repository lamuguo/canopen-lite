//
// Created by lamuguo on 9/6/23.
//

#ifndef CANOPEN_LITE_SDO_CLIENT_H
#define CANOPEN_LITE_SDO_CLIENT_H

#include "network/socketcan.h"
#include "od/od_object_dictionary.h"

namespace colite {

    class SDOClient {
    public:
        SDOClient(const char* channel, int node_id)
            : network_(channel) {}

        std::unique_ptr<Variable> expedited_upload(int node_id, int index, int subindex) {
            std::vector<uint8_t> request = {
                    0x40, static_cast<uint8_t>(index),
                    static_cast<uint8_t>(index >> 8), static_cast<uint8_t>(subindex)
            };
            network_.send_message(node_id + 0x600, request);

            // Receive loop
            while (true) {
                auto frame = network_.recv_message();
                if (frame->can_id == node_id + 0x580) {
                    return parse_response(*frame, index, subindex);
                }
            }
        }

        std::unique_ptr<Variable> parse_response(const struct can_frame& frame, int index, int subindex) {
            auto var = std::make_unique<Variable>("FetchedVariable", index, subindex);
            // Extract data from response.
            // Note: This is a very simplified parser and assumes the data is an INT32
            uint32_t value = 0;
            std::memcpy(&value, &frame.data[4], sizeof(uint32_t));
            var->default_value = Data(UNSIGNED32, value);
            var->data_type = UNSIGNED32;
            return var;
        }

    private:
        SocketCANNetwork network_;
    };

}

#endif //CANOPEN_LITE_SDO_CLIENT_H
