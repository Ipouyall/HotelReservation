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

include(FetchContent)
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
        infra/user.cpp)
target_link_libraries(codes glog::glog)