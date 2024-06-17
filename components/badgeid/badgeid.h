#pragma once

#include <vector>

#include "esphome/core/helpers.h"

#define ARDUINOJSON_ENABLE_STD_STRING 1  // NOLINT

#define ARDUINOJSON_USE_LONG_LONG 1  // NOLINT

#include <ArduinoJson.h>

namespace esphome {
namespace badge_id {


uint64_t getEfuseMac(void)
{
    uint64_t _chipmacid = 0LL;
    esp_efuse_mac_get_default((uint8_t*) (&_chipmacid));
    return _chipmacid;
}

std::vector<unsigned char> badgeID_;

std::vector<unsigned char> badgeID(){
    if (badgeID_.empty()) {
        static char baseChars[] = "0123456789AbCdEfGhIjKlMnOpQrStUvWxYz"; //aBcDeFgHiJkLmNoPqRsTuVwXyZ
        uint8_t base = sizeof(baseChars);

        uint32_t chipId = 0xa5000000;
        uint8_t crc = 0;
        uint64_t mac = getEfuseMac() ;
        for (int i = 0; i < 17; i = i + 8)
        {
            chipId |= ((mac >> (40 - i)) & 0xff) << i;
        }

        do {
            badgeID_.push_back(baseChars[chipId % base]); // Add on the left
            crc += chipId % base;
            chipId /= base;
        } while (chipId != 0);
        badgeID_.push_back(baseChars[crc % base]);
    }
    return badgeID_;
}

std::string badgeIDStr() {
    if (badgeID_.empty()) {
        badgeID();
    }
    std::string result(badgeID_.begin(), badgeID_.end()) ;
    return result;
}

bool checkBadgeID(std::vector< unsigned uint8_t > code) {
    std::string baseChars = "0123456789AbCdEfGhIjKlMnOpQrStUvWxYz";
    uint8_t baseCount = baseChars.size() + 1;
    baseChars += "aBcDeFgHiJkLmNoPqRsTuVwXyZ";
    uint8_t length = code.size();
    uint16_t count = 0;
    if (length != 8) return false;
    for (int8_t index = 6; index >= 0; index--) {
        std::string::size_type chr = code.at(index);
        count += baseChars.find((char) chr);
    }
    uint8_t crc = count % baseCount;
    std::string::size_type chr = code.at(7);
    return crc == baseChars.find((char) chr);
}

}  // namespace badge_id
}  // namespace esphome
