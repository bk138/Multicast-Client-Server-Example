cmake_minimum_required(VERSION 3.0)
project(Multicast-Client-Server-Example LANGUAGES C)

# the server

SET(server_SRCS
   src/server.c
   src/msock.c
)

add_executable(server ${server_SRCS})

target_link_libraries(server)

install(TARGETS server DESTINATION bin)


# the client

SET(client_SRCS
   src/client.c
   src/msock.c
)

add_executable(client ${client_SRCS})

target_link_libraries(client)

install(TARGETS client DESTINATION bin)

