project(Shared)
cmake_minimum_required(VERSION 3.18)

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/ClientToAuth
	${CMAKE_CURRENT_LIST_DIR}/AuthToClient
)

target_link_libraries(${PROJECT_NAME} Network)

add_subdirectory(ClientToAuth)
add_subdirectory(AuthToClient)