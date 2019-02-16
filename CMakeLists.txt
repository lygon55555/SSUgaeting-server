cmake_minimum_required(VERSION 3.10)

project(ssugaeting LANGUAGES C)
find_package(Threads)

add_executable(ssugaeting main.c http-parser.c http-parser.h request.c request.h)

TARGET_LINK_LIBRARIES(ssugaeting ${CMAKE_THREAD_LIBS_INIT} mysqlclient)

install(TARGETS ssugaeting RUNTIME DESTINATION bin)




