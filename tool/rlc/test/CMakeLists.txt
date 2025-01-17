file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fuzzer_lib_location.txt CONTENT "$<TARGET_FILE:rlc::fuzzer>")
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/runtime_lib_location.txt CONTENT "$<TARGET_FILE:rlc::runtime>")
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pyrlc_lib_location.txt CONTENT "$<TARGET_FILE:rlc::pyrlc>")

rlcAddLitTest(rlc)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(NAME python_tic_tac_toe COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/solve.py ${CMAKE_CURRENT_SOURCE_DIR}/tic_tac_toe.rl --rlc $<TARGET_FILE:rlc> --stdlib ${CMAKE_SOURCE_DIR}/stdlib/ --runtime $<TARGET_FILE:rlc::runtime> --pyrlc $<TARGET_FILE:rlc::pyrlc>)



