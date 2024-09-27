cmake_minimum_required(VERSION 3.24)
project(RayMarchEngine LANGUAGES CXX)

#Fetching
include(FetchContent)
FETCHCONTENT_DECLARE(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        )

set(SFML_BUILD_AUDIO OFF)

FETCHCONTENT_MAKEAVAILABLE(SFML)

FETCHCONTENT_DECLARE(EIGEN
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        )
FETCHCONTENT_MAKEAVAILABLE(EIGEN)

set(SOURCE_FILE main.cpp)
include_directories(src)


if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 main.cpp)
else()
    add_executable(${PROJECT_NAME} ${SOURCE_FILE})
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rayMarch.frag ${CMAKE_CURRENT_BINARY_DIR}/rayMarch.frag COPYONLY)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(TARGETS ${PROJECT_NAME})
