# Find required packages
FIND_PACKAGE(Boost REQUIRED COMPONENTS system filesystem serialization program_options)

# Search for header and source files
FILE(GLOB bms_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Set compiler flags
SET(CMAKE_CXX_FLAGS "-std=c++11 -DHAVE_CONFIG_H")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-empty-body")
ENDIF()

# Add boost header include
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# Add library
ADD_LIBRARY(bms STATIC ${bms_source})

# Set dependency to other lib
TARGET_LINK_LIBRARIES(bms
    api
    core
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

# Make library accessible
TARGET_INCLUDE_DIRECTORIES(bms PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
