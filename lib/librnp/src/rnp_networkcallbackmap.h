#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include "rnp_packet.h"


namespace NetworkCallbackMap
{
    static constexpr auto pair_hash_func = [](const std::pair<uint8_t, uint8_t>& p) {
            return (static_cast<std::size_t>(p.first) << 8) + static_cast<std::size_t>(p.second);
        };
};

using NetworkCallbackMap_t =  std::unordered_map<std::pair<uint8_t, uint8_t>, std::function<void(std::unique_ptr<RnpPacketSerialized>)>, decltype(NetworkCallbackMap::pair_hash_func)>;
