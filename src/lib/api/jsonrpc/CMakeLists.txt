# Require curl to be present
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(CURL REQUIRED)

# Glob the source files
FILE(GLOB_RECURSE jsonrpc_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Add it as a library
ADD_LIBRARY(jsonrpc STATIC ${jsonrpc_source})

# Link it with other libraries
TARGET_LINK_LIBRARIES(jsonrpc
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURL_LIBRARIES}
)

# Make library public for other includes
# TARGET_INCLUDE_DIRECTORIES(jsonrpc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
