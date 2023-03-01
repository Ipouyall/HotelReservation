cmake_minimum_required(VERSION 3.24)
project(codes)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.4
)
FetchContent_MakeAvailable(json)
include_directories(${json_SOURCE_DIR}/single_include)


add_executable(codes
        main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp)
