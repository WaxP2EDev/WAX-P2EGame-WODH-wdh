cmake_minimum_required( VERSION 3.5 )

project(drhemslandsx)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

if(${BUILD_TESTS})
    add_definitions(-DDEBUG)
endif()

add_contract(drhemslandsx drhemslandsx
drhemslandsx.cpp
)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/tables)
