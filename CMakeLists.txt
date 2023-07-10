cmake_minimum_required(VERSION 3.10)

## set appropriately the following line to create a Double Precision version
set(DOUBLE_PRECISION ON)

project(readBinaries C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

## adding the following lines while cleaning the code
MESSAGE( STATUS "CMAKE_C_COMPILER_ID: "         ${CMAKE_C_COMPILER_ID} )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-result -Wno-unused-parameter -ftree-vectorize -fopt-info -ffast-math -mavx2 ")
    #set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-result -Wno-unused-parameter -ftree-vectorize -fopt-info -ffast-math ")
    #set(CMAKE_EXE_LINKER_FLAGS "-lm" )
elseif ( CMAKE_C_COMPILER_ID STREQUAL "IntelLLVM" )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -fvectorize  -qopt-report-file=stderr -qopt-report=max  -xCORE-AVX2")
    #set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -qopt-report=5 -qopt-report-phase=vec -xCORE-AVX2")
    #set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -qopt-report=5 -qopt-report-phase=vec ")
elseif ( CMAKE_C_COMPILER_ID STREQUAL "NVHPC" )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Minform=inform  -Mnodepchk -Minfo=all -Mvect=simd ")
    #set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Minform=inform  -Mnodepchk -Minfo=all -Mvect=simd -tp=px ")
endif ( )
## end of adding the following lines while cleaning the code


if (DOUBLE_PRECISION)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DDOUBLE")
endif()

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

include_directories("src/include")
add_subdirectory(src)

add_executable(parallelSpmv ${parallelSpmv_Src})
target_link_libraries(parallelSpmv ${MPI_LIBRARIES} ${CMAKE_EXE_LINKER_FLAGS})


############### Debug messages ####################
#MESSAGE( STATUS "CMAKE_BINARY_DIR: "            ${CMAKE_BINARY_DIR} )
#MESSAGE( STATUS "CMAKE_C_COMPILER_ID: "         ${CMAKE_C_COMPILER_ID} )
#MESSAGE( STATUS "CMAKE_C_FLAGS: "               ${CMAKE_C_FLAGS} )
#MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: "    ${CMAKE_CURRENT_SOURCE_DIR} )
########### end of Debug messages #################
