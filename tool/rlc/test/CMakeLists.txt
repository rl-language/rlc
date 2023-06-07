macro (rlc_test_file file_name)
add_test(NAME compile_${file_name} COMMAND rlc::rlc ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}.compiled -O2)
add_test(NAME dump_unchecked_ast_${file_name} COMMAND rlc::rlc ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} --unchecked)
add_test(NAME dump_ast_${file_name} COMMAND rlc::rlc ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} --rlc)
add_test(NAME execute_${file_name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${file_name}.compiled)
set_tests_properties(execute_${file_name} PROPERTIES DEPENDS compile_${file_name})
endmacro()

file(GLOB files "*.rl")
foreach(file ${files})
	get_filename_component(name ${file} NAME)
	rlc_test_file(${name})
endforeach()

