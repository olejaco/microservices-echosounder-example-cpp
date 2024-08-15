cmake_minimum_required(VERSION 3.15)
project(EchoSounderSystem)

# Conan setup
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

# Find dependencies
find_package(fastdds REQUIRED)
find_package(fastcdr REQUIRED)

# Add IDL compilation command
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/EchoData.cxx ${CMAKE_CURRENT_BINARY_DIR}/EchoDataPubSubTypes.cxx
    COMMAND fastddsgen -replace -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/common/EchoData.idl
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/common/EchoData.idl
    COMMENT "Generating code from IDL"
)

# Add IDL compilation target
add_custom_target(idl_compilation
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/EchoData.cxx ${CMAKE_CURRENT_BINARY_DIR}/EchoDataPubSubTypes.cxx
)

# Include generated files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Add targets for each controller
add_executable(acquisition_controller 
    src/acquisition_controller/main.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/EchoData.cxx 
    ${CMAKE_CURRENT_BINARY_DIR}/EchoDataPubSubTypes.cxx
)
add_dependencies(acquisition_controller idl_compilation)
target_link_libraries(acquisition_controller fastrtps fastcdr)

add_executable(processing_controller 
    src/processing_controller/main.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/EchoData.cxx 
    ${CMAKE_CURRENT_BINARY_DIR}/EchoDataPubSubTypes.cxx
)
add_dependencies(processing_controller idl_compilation)
target_link_libraries(processing_controller fastrtps fastcdr)

add_executable(visualization_controller 
    src/visualization_controller/main.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/EchoData.cxx 
    ${CMAKE_CURRENT_BINARY_DIR}/EchoDataPubSubTypes.cxx
)
add_dependencies(visualization_controller idl_compilation)
target_link_libraries(visualization_controller fastrtps fastcdr)