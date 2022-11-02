project(Auth)
cmake_minimum_required(VERSION 3.18)

add_executable(${PROJECT_NAME}
	Main.cpp
	AuthServer.cpp
	AuthConnection.cpp
	AuthServer.h
	AuthConnection.h
)

target_link_libraries(${PROJECT_NAME} Network Shared)