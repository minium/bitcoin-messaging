# Find files
FILE(GLOB bitcoinapi_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Add dependency first
ADD_SUBDIRECTORY(jsonrpc)

# Set compiler settings
SET(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")

# Add as a static library
ADD_LIBRARY(api STATIC ${bitcoinapi_source})

# Link it with the JSON library
TARGET_LINK_LIBRARIES(api jsonrpc)

# Make library public for other includes
TARGET_INCLUDE_DIRECTORIES(api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
