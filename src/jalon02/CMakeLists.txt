cmake_minimum_required(VERSION 2.8.3)

project(RE216_JALON02)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	client.c
	functions.c
	)


add_executable(RE216_JALON02_CLIENT ${SRC_CLIENT})


#add here source files you need to compile and link to the server app
SET(SRC_SERVER
        server.c
        functions.c
        )


add_executable(RE216_JALON02_SERVER  ${SRC_SERVER})
find_package(Threads)
target_link_libraries(RE216_JALON02_SERVER ${CMAKE_THREAD_LIBS_INIT})


