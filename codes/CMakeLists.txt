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
  date
  GIT_REPOSITORY https://github.com/HowardHinnant/date.git
  GIT_TAG v3.0.1
)
FetchContent_MakeAvailable(date)

find_library(LIBEDIT_LIBRARY edit)

add_executable(hr_test
        main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp
        infra/socketUtils.cpp
        infra/dateManager.cpp
        infra/saveLog.cpp
        infra/client.cpp
        )
add_executable(hr_server
        server/main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp
        infra/socketUtils.cpp
        infra/dateManager.cpp
        infra/saveLog.cpp
        infra/client.cpp
        )
add_executable(hr_client
        client/main.cpp
        infra/filehandler.cpp
        infra/hotel.cpp
        infra/server.cpp
        infra/user.cpp
        infra/socketUtils.cpp
        infra/dateManager.cpp
        infra/saveLog.cpp
        infra/client.cpp
        front/cli.cpp
        )

target_link_libraries(hr_test nlohmann_json glog::glog ${LIBEDIT_LIBRARY} date)
target_link_libraries(hr_server nlohmann_json glog::glog date)
target_link_libraries(hr_client nlohmann_json glog::glog ${LIBEDIT_LIBRARY} date)

