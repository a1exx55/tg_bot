cmake_minimum_required(VERSION 3.27.7)

project(tg_bot)

#set flags for compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -g -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

find_package(Boost COMPONENTS REQUIRED 
	system
	json
)
find_package(OpenSSL REQUIRED)

#include all source files
set(SRC 
	src/main.cpp
	src/keyboard_utils.cpp
	src/handlers.cpp
	)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE "src/")

target_link_libraries(${PROJECT_NAME} 
	-lTgBot
	Boost::system
	Boost::json
	OpenSSL::SSL OpenSSL::Crypto
)