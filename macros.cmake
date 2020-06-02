##############################
###       InstallMacro     ###
##############################
macro(rlcInstall target)

include(GNUInstallDirs)
INSTALL(TARGETS ${target} EXPORT ${target}Targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include)

INSTALL(EXPORT ${target}Targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
	FILE ${target}Targets.cmake
	NAMESPACE rlc::)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${target}ConfigVersion.cmake
	VERSION ${example_VERSION}
	COMPATIBILITY SameMajorVersion)

INSTALL(FILES ${target}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${target}ConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

endmacro(rlcInstall)

##############################
###    addLibraryMacro     ###
##############################
macro(rlcAddLibrary target)
add_library(${target} ${ARGN})

add_library(rlc::${target} ALIAS ${target})

target_include_directories(${target}
	PRIVATE 
		src 
	PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
		)

target_compile_features(${target} PUBLIC cxx_std_17)

#if (BUILD_FUZZER)
	#target_compile_options(${target} PRIVATE -fsanitize=fuzzer)
	#target_link_libraries(${target} PRIVATE -fsanitize=fuzzer)
#endif()
rlcInstall(${target})

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
	add_subdirectory(test)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
	add_subdirectory(benchmark)
endif()
	

endmacro(rlcAddLibrary)


##############################
### rlcAddTestMacro   ###
##############################
macro(rlcAddTest target)
	include(GoogleTest)

	add_executable(${target}Test ${ARGN})
	add_executable(rlc::${target}Test ALIAS ${target}Test) 
	target_link_libraries(${target}Test PRIVATE gtest gtest_main ${target})
	target_include_directories(${target}Test PUBLIC include PRIVATE src)
	target_compile_features(${target}Test PUBLIC cxx_std_17)

	gtest_add_tests(TARGET     ${target}Test
					TEST_SUFFIX .noArgs
					TEST_LIST   noArgsTests
	)

endmacro(rlcAddTest)

##############################
###  rlcAddToolMacro  ###
##############################
macro(rlcAddTool target)

	add_executable(${target} src/Main.cpp)
	add_executable(rlc::${target} ALIAS ${target})

	target_link_libraries(${target} PUBLIC ${ARGN})
	target_compile_features(${target} PUBLIC cxx_std_17)

	include(GNUInstallDirs)
	INSTALL(TARGETS ${target} RUNTIME DESTINATION bin)

	if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
		add_subdirectory(test)
	endif()
	

endMacro(rlcAddTool)

##############################
###  rlcAddToolBench  ###
##############################
macro(rlcAddBenchmark target)
	ADD_EXECUTABLE(${target}Benchmark src/${target}Benchmark.cpp ${ARGN}) 
	ADD_EXECUTABLE(rlc::${target}Benchmark ALIAS ${target}Benchmark)

	TARGET_LINK_LIBRARIES(${target}Benchmark PRIVATE benchmark::benchmark rlc::${target})
	TARGET_INCLUDE_DIRECTORIES(${target}Benchmark PUBLIC include PRIVATE src)
	TARGET_COMPILE_FEATURES(${target}Benchmark PUBLIC cxx_std_17)

	INSTALL(TARGETS ${target}Benchmark 
	RUNTIME DESTINATION bechmark)
endMacro(rlcAddBenchmark)
