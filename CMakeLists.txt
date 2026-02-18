cmake_minimum_required(VERSION 4.1)
project(email_spam_filter)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "start running cmake...")

find_package(Boost 1.90.0)

if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(email_spam_filter main.cpp)

if(Boost_FOUND)
    target_link_libraries(email_spam_filter ${Boost_LIBRARIES})
endif()