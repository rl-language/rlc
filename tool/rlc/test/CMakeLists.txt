file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fuzzer_lib_location.txt CONTENT "$<TARGET_FILE:rlc::fuzzer>")
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/runtime_lib_location.txt CONTENT "$<TARGET_FILE:rlc::runtime>")

rlcAddLitTest(rlc)

find_package(Python COMPONENTS Interpreter REQUIRED)
add_test(NAME python_tic_tac_toe COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/solve.py ${CMAKE_CURRENT_SOURCE_DIR}/tic_tac_toe.rl --rlc $<TARGET_FILE:rlc> -i ${CMAKE_SOURCE_DIR}/stdlib/ --runtime $<TARGET_FILE:rlc::runtime>)



