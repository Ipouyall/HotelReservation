cmake_minimum_required(VERSION 3.24)
project(codes VERSION 0.0.1 LANGUAGES CXX)

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

add_executable(codes
        main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp
        )
target_link_libraries(codes nlohmann_json glog::glog)
