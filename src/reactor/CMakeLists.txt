project(reactor CXX)
cmake_minimum_required(VERSION 3.1...3.14)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/include/reactor/reactor.hpp)

add_library(reactor-lib ${SOURCES})
set_target_properties(reactor-lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(reactor-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(UTEST)
  add_subdirectory("test")
endif()

