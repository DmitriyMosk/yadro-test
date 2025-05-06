# first test
add_executable(
    cases_complex 
    cases_complex.cpp
)
target_sources(
    cases_complex 
    PUBLIC ${CMAKE_SOURCE_DIR}/src/types/complex.cpp
)
add_test(NAME cases_complex COMMAND cases_complex)

# second test
add_executable(
    cases_qam_mapper
    cases_qam_mapper.cpp
)
target_sources(
    cases_qam_mapper 
    PUBLIC ${CMAKE_SOURCE_DIR}/src/types/complex.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/file/file.cpp
)
add_test(NAME cases_qam_mapper COMMAND cases_qam_mapper)

# third test
add_executable(
    cases_qam_modulator
    cases_qam_modulator.cpp
)
target_sources(
    cases_qam_modulator 
    PUBLIC ${CMAKE_SOURCE_DIR}/src/types/complex.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/file/file.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/phys/qam/qam_modulator.cpp
)
add_test(NAME cases_qam_modulator COMMAND cases_qam_modulator)

# 4th test
add_executable(
    cases_qam_demodulator
    cases_qam_demodulator.cpp
)
target_sources(
    cases_qam_demodulator 
    PUBLIC ${CMAKE_SOURCE_DIR}/src/types/complex.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/file/file.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/phys/qam/qam_modulator.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/phys/qam/qam_demodulator.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/phys/chan.cpp
)
add_test(NAME cases_qam_demodulator COMMAND cases_qam_demodulator)

# 5th test
add_executable(
    cases_qam_demod_chan
    cases_qam_demod_chan.cpp
)
target_sources(
    cases_qam_demod_chan 
    PUBLIC ${CMAKE_SOURCE_DIR}/src/types/complex.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/file/file.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/phys/qam/qam_modulator.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/phys/qam/qam_demodulator.cpp
    PUBLIC ${CMAKE_SOURCE_DIR}/src/phys/chan.cpp
)
add_test(NAME cases_qam_demod_chan COMMAND cases_qam_demod_chan)