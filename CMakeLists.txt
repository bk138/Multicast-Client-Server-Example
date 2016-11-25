cmake_minimum_required(VERSION 3.0)
project(Multicast-Client-Server-Example LANGUAGES C)

if(UNIX)
    add_definitions(-DUNIX)
endif(UNIX)

if(WIN32)
    add_definitions(-DWIN32)
    set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} ws2_32)
endif(WIN32)


# the server

SET(server_SRCS
   src/server.c
   src/msock.c
)

add_executable(server ${server_SRCS})

target_link_libraries(server
                      ${ADDITIONAL_LIBS}
)

install(TARGETS server DESTINATION bin)


# the client

SET(client_SRCS
   src/client.c
   src/msock.c
)

add_executable(client ${client_SRCS})

target_link_libraries(client
                      ${ADDITIONAL_LIBS}
)

install(TARGETS client DESTINATION bin)

