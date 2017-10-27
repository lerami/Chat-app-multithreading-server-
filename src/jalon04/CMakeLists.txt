cmake_minimum_required(VERSION 2.8.3)

project(RE216_JALON04)


source_group("Headers" REGULAR_EXPRESSION ".*\\.h$")


include_directories( 
	commons
	server
	client)
#add here sources that are common between server and client
SET(SRC_COMMONS
	commons/functions.c
)


#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
client/client.c
)


add_executable(RE216_JALON04_CLIENT ${SRC_CLIENT} ${SRC_COMMONS})
find_package(Threads)
target_link_libraries(RE216_JALON04_CLIENT ${CMAKE_THREAD_LIBS_INIT})

#add here source files you need to compile and link to the server app
SET(SRC_SERVER
server/server.c
server/users.c
server/channel.c
server/file_transfer.c
)


add_executable(RE216_JALON04_SERVER  ${SRC_SERVER} ${SRC_COMMONS})
find_package(Threads)
target_link_libraries(RE216_JALON04_SERVER ${CMAKE_THREAD_LIBS_INIT})
