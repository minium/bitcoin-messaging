# Find required packages
FIND_PACKAGE(OpenSSL REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(Boost REQUIRED COMPONENTS system thread)

# Search for header and source files
FILE(GLOB core_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Set compiler flags
SET(CMAKE_CXX_FLAGS "-DHAVE_CONFIG_H")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
ENDIF()


# Add boost header include
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# Set target 'core' to be a static library
ADD_LIBRARY(core STATIC ${core_source})

# Link it with the appropriate libraries
TARGET_LINK_LIBRARIES(core
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_SSL_LIBRARY}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

# Make library public for other includes
TARGET_INCLUDE_DIRECTORIES(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


