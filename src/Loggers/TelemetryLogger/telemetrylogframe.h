#pragma once

#include <librnp/rnp_serializer.h>
#include <unistd.h>

class TelemetryLogframe{
private:
    static constexpr auto getSerializer()
    {
        auto ret = RnpSerializer(
            &TelemetryLogframe::timestamp,
            &TelemetryLogframe::qdVoltageMV,
            &TelemetryLogframe::baroAlt,
            &TelemetryLogframe::deployed
        );
        return ret;
    }

public:
    uint64_t timestamp = 0;
    uint32_t qdVoltageMV = 0; // QDVoltage in mV
    bool deployed = false;
    float baroAlt = 0;

    std::string stringify()const{
        return getSerializer().stringify(*this) + "\n";
    };

};
