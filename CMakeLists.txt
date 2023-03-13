cmake_minimum_required(VERSION 3.9)
project(EctPi
    DESCRIPTION "EtherCAT toolbox for Raspberry Pi"
    VERSION 0.1.2
    LANGUAGES CXX C)
add_executable(ectpi src/ectpi.cpp)
find_package(Curses REQUIRED)
target_include_directories(ectpi PUBLIC
                           "${PROJECT_BINARY_DIR}/include"
                           "${PROJECT_SOURCE_DIR}/include"
                           "${PROJECT_SOURCE_DIR}/SOEM/soem"
                           "${CURSES_INCLUDE_DIR}"
                           )
#configure_file(include/configure.h.in include/configure.h)
add_subdirectory(SOEM)
target_link_libraries(ectpi PUBLIC soem ${CURSES_LIBRARIES})
