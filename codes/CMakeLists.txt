cmake_minimum_required(VERSION 3.24)
project(HotelReservation VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.10.4
)
FetchContent_GetProperties(nlohmann_json)
if(NOT nlohmann_json_POPULATED)
    FetchContent_Populate(nlohmann_json)
    add_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR})
endif()

FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG v0.5.0
)
FetchContent_MakeAvailable(glog)
include_directories(${glog_SOURCE_DIR}/src)

FetchContent_Declare(
    OpenSSL
    URL https://www.openssl.org/source/openssl-1.1.1k.tar.gz
    URL_HASH SHA256=892a0875b9872acd04a9fde79b1f943075d5ea162415de3047c327df33fbaee5
)
FetchContent_GetProperties(OpenSSL)
if(NOT OpenSSL_POPULATED)
    FetchContent_Populate(OpenSSL)
    add_subdirectory(${openssl_SOURCE_DIR} ${openssl_BINARY_DIR})
endif()

add_executable(hr_test
        main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp
        infra/socketUtils.cpp
        infra/saveLog.cpp
        infra/decoder.cpp
        infra/encoder.cpp
        )
add_executable(hr_server
        server/main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp
        infra/socketUtils.cpp
        infra/saveLog.cpp
        infra/decoder.cpp
        infra/encoder.cpp
        )
add_executable(hr_client
        client/main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp
        infra/socketUtils.cpp
        infra/saveLog.cpp
        infra/decoder.cpp
        infra/encoder.cpp
        )

target_link_libraries(hr_test nlohmann_json glog::glog OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(hr_server nlohmann_json glog::glog OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(hr_client nlohmann_json glog::glog OpenSSL::SSL OpenSSL::Crypto)
