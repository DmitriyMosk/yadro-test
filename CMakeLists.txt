# CMAKE VERSION
cmake_minimum_required(VERSION 3.10)

# PROJECT NAME
project(yadro_1v LANGUAGES CXX C) 


# ====== project params
set(CMAKE_CXX_STANDARD  20)
set(FORCE_TEST          OFF)


include_directories(include) 

######### MAIN PATH 
set(MAIN_APP
    ${CMAKE_SOURCE_DIR}/src/app/main.cpp
)

set(TYPES_SRC
    ${CMAKE_SOURCE_DIR}/src/types/complex.cpp
)

set(PHYS_SRC
    ${CMAKE_SOURCE_DIR}/src/phys/qam/qam_modulator.cpp
    ${CMAKE_SOURCE_DIR}/src/phys/qam/qam_demodulator.cpp
    ${CMAKE_SOURCE_DIR}/src/phys/qam/qam.cpp
    ${CMAKE_SOURCE_DIR}/src/phys/chan.cpp
)


set(FILE_SRC
    ${CMAKE_SOURCE_DIR}/src/file/file.cpp
    ${CMAKE_SOURCE_DIR}/src/file/csv_writer.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${TYPES_SRC}
    ${PHYS_SRC}
    ${FILE_SRC}
    ${MAIN_APP} 
) 

if (NOT FORCE_TEST)
    enable_testing()
    add_subdirectory(tests) 
endif() 