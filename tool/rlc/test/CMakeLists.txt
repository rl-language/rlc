macro (rlc_test_file file_name)
add_test(NAME compile_${file_name} COMMAND rlc::rlc ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}.compiled -O2 -i ${CMAKE_SOURCE_DIR}/stdlib/)
add_test(NAME dump_unchecked_ast_${file_name} COMMAND rlc::rlc ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} --unchecked -i ${CMAKE_SOURCE_DIR}/stdlib/)
add_test(NAME dump_ast_${file_name} COMMAND rlc::rlc ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} --rlc -i ${CMAKE_SOURCE_DIR}/stdlib/)
add_test(NAME execute_${file_name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${file_name}.compiled)
set_tests_properties(execute_${file_name} PROPERTIES DEPENDS compile_${file_name})
set_tests_properties(dump_unchecked_ast_${file_name} PROPERTIES DEPENDS compile_${file_name})
set_tests_properties(dump_ast_${file_name} PROPERTIES DEPENDS compile_${file_name})
endmacro()


find_package(Python COMPONENTS Interpreter REQUIRED)
add_test(NAME python_tris COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/solve.py --source ${CMAKE_CURRENT_SOURCE_DIR}/tris.rl --rlc $<TARGET_FILE:rlc>)


file(GLOB files "*.rl")
foreach(file ${files})
	get_filename_component(name ${file} NAME)
	rlc_test_file(${name})
endforeach()

