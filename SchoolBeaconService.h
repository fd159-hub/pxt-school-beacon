#ifndef SCHOOL_BEACON_SERVICE_H
#define SCHOOL_BEACON_SERVICE_H
#include <stdint.h>

// One advertising set for the lifetime of the program; never allocate in static constructors.
class SchoolBeaconService {
public:
    SchoolBeaconService();
    bool start(const uint8_t uuid[16], uint16_t major, uint16_t minor);
    bool stop();
    bool isActive() const { return advertising; }
    uint32_t error() const { return lastError; }
private:
    bool initialize();
    bool advertising;
    uint32_t lastError;
};
#endif
