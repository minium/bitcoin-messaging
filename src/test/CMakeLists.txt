# Find test source files
FILE(GLOB bms_test_source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Add configuration files
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/config/bms.conf config/bms.conf COPYONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/config/huffcode.map config/huffcode.map COPYONLY)

# Set compiler settings
SET(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall -DHAVE_CONFIG_H")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
ENDIF()

# Include header directory
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib/api/bms/)

# Add target executable
ADD_EXECUTABLE(tests EXCLUDE_FROM_ALL ${bms_test_source})
TARGET_LINK_LIBRARIES(tests
    core
    bms
    api
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

# Set different name for executable
SET_TARGET_PROPERTIES(tests PROPERTIES OUTPUT_NAME bms_tests)
