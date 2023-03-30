cmake_minimum_required(VERSION 3.24)
project(RayMarchEngine LANGUAGES CXX)

#Fetching
include(FetchContent)
FETCHCONTENT_DECLARE(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
)
FETCHCONTENT_MAKEAVAILABLE(SFML)

FETCHCONTENT_DECLARE(EIGEN
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
)
FETCHCONTENT_MAKEAVAILABLE(EIGEN)

set(SOURCE_FILE main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILE})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS ${PROJECT_NAME})

