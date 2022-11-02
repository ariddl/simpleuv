project(Network)
cmake_minimum_required(VERSION 3.18)

add_library(${PROJECT_NAME} STATIC 
	${PROJECT_SOURCE_DIR}/PacketFunctor.h
	${PROJECT_SOURCE_DIR}/PacketFunctorBase.h
	${PROJECT_SOURCE_DIR}/PacketHandler.h
	${PROJECT_SOURCE_DIR}/INetPacket.h
	${PROJECT_SOURCE_DIR}/PacketQueueElemBase.h
	${PROJECT_SOURCE_DIR}/PacketQueueElem.h
)
target_include_directories(${PROJECT_NAME}
	PUBLIC
		${PROJECT_SOURCE_DIR}/
		${PROJECT_SOURCE_DIR}/Serialize
		${PROJECT_SOURCE_DIR}/Net
)

target_link_libraries(${PROJECT_NAME} uvw::uvw ${LIBUV_LIBRARY})

add_subdirectory(Serialize)
add_subdirectory(Net)