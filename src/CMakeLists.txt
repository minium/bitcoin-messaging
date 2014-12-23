# Add library directories
ADD_SUBDIRECTORY(lib/api)
ADD_SUBDIRECTORY(lib/core)
ADD_SUBDIRECTORY(lib/bms)

# Find main executable source files
FILE(GLOB bms_source ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

# Add configuration files
CONFIGURE_FILE(config/bms.conf config/bms.conf COPYONLY)
CONFIGURE_FILE(config/huffcode.map config/huffcode.map COPYONLY)

# Set compiler flags
SET(CMAKE_CXX_FLAGS "-std=c++11 -DHAVE_CONFIG_H")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-empty-body")
ENDIF()

# Add boost header include
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# Add target executable
ADD_EXECUTABLE(BMS_Exec ${bms_source})
TARGET_LINK_LIBRARIES(BMS_Exec
    api			
    core
    bms
)

# Set different name for executable
SET_TARGET_PROPERTIES(BMS_Exec PROPERTIES OUTPUT_NAME bms)

INSTALL(TARGETS BMS_Exec RUNTIME DESTINATION bms/ RENAME bms)
INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/config" DESTINATION "bms")
