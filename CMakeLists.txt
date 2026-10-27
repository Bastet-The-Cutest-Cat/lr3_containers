cmake_minimum_required(VERSION 3.16)
project(container_lab3 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(container_lab3 src/main.cpp)
target_include_directories(container_lab3 PRIVATE include)

include(CTest)
if(BUILD_TESTING)
    add_executable(container_tests tests/test_containers.cpp)
    target_include_directories(container_tests PRIVATE include)
    add_test(NAME container_tests COMMAND container_tests)
endif()

install(TARGETS container_lab3 RUNTIME DESTINATION bin)

set(CPACK_PACKAGE_NAME "container-lab3")
set(CPACK_PACKAGE_VENDOR "Student")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ dynamic container laboratory work")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "student@example.com")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Student")
include(CPack)
