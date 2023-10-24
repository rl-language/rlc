add_executable(should_crash should_crash.cpp)

macro (rlc_test_file file_name)
add_test(NAME compile_${file_name} COMMAND rlc::rlc ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}.compiled -O2 -i ${CMAKE_SOURCE_DIR}/stdlib/)
# the execution of tests in this directory are expected to crash. Hence we invoke should_crash.
add_test(NAME execute_${file_name} COMMAND should_crash ${CMAKE_CURRENT_BINARY_DIR}/${file_name}.compiled)
set_tests_properties(execute_${file_name} PROPERTIES DEPENDS compile_${file_name})
endmacro()

file(GLOB files "*.rl")
foreach(file ${files})
	get_filename_component(name ${file} NAME)
	rlc_test_file(${name})
endforeach()

