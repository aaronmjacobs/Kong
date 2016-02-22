# Project definition
cmake_minimum_required(VERSION 3.1)
project(Kong LANGUAGES CXX)

# Executable definition and features
add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Libraries
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Kontroller")
target_include_directories(${PROJECT_NAME} PRIVATE $<TARGET_PROPERTY:Kontroller,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(${PROJECT_NAME} PRIVATE Kontroller $<TARGET_PROPERTY:Kontroller,INTERFACE_LINK_LIBRARIES>)
target_compile_features(${PROJECT_NAME} PRIVATE $<TARGET_PROPERTY:Kontroller,INTERFACE_COMPILE_FEATURES>)
target_compile_definitions(${PROJECT_NAME} INTERFACE $<TARGET_PROPERTY:Kontroller,INTERFACE_COMPILE_DEFINITIONS>)