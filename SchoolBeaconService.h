#ifndef SCHOOL_BEACON_SERVICE_H
#define SCHOOL_BEACON_SERVICE_H
#include <stdint.h>
class SchoolBeaconService {
public:
    SchoolBeaconService();
    void start(const uint8_t uuid[16], uint16_t major, uint16_t minor, int8_t txPower);
    void stop();
private:
    bool advertising;
};
#endif
