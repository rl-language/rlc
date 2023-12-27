rlcAddLitTest(rlc)

find_package(Python COMPONENTS Interpreter REQUIRED)
add_test(NAME python_tris COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/solve.py --source ${CMAKE_CURRENT_SOURCE_DIR}/tris.rl --rlc $<TARGET_FILE:rlc> -i ${CMAKE_SOURCE_DIR}/stdlib/)



