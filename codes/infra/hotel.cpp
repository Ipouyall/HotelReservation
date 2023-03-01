#include "hotel.h"

HotelRoom get_hotel_information(string path) {
    auto info = HotelRoom{};

    auto j = readJsonFile(path);
    if (j == nullptr)
        return info;

    return info;
}