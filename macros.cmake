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
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
	PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
	${LLVM_INCLUDE_DIRS}
		)

target_compile_features(${target} PUBLIC cxx_std_20)
target_compile_definitions(${target} PUBLIC ${LLVM_DEFINITIONS})

rlcInstall(${target})

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
	add_subdirectory(test)
endif()


if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
	add_subdirectory(benchmark)
endif()
	

endmacro(rlcAddLibrary)


##############################
###   rlcAddLitTestMacro   ###
##############################
macro(rlcAddLitTest target)
   find_program(LIT NAMES "lit")

   configure_lit_site_cfg(
     ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
     ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
     MAIN_CONFIG
     ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py)

   set(${target}_lit_dependencies
     rlc
     rlc-opt)

   add_test(
     NAME lit-${target}
     COMMAND ${LIT} ${CMAKE_CURRENT_BINARY_DIR} 
   )

   set_tests_properties(lit-${target} PROPERTIES DEPENDS "${${target}_lit_dependencies}")

endMacro(rlcAddLitTest)

##############################
###    rlcAddTestMacro     ###
##############################
macro(rlcAddTest target)
	include(GoogleTest)

	add_executable(${target}Test ${ARGN})
	add_executable(rlc::${target}Test ALIAS ${target}Test) 
	target_link_libraries(${target}Test PRIVATE gtest gtest_main ${target})
	target_include_directories(${target}Test PUBLIC include PRIVATE src)
	target_compile_features(${target}Test PUBLIC cxx_std_20)

	gtest_add_tests(TARGET     ${target}Test
					TEST_SUFFIX .noArgs
					TEST_LIST   noArgsTests
	)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in)
        rlcAddLitTest(${target})
    endif()

endmacro(rlcAddTest)

##############################
###  rlcAddToolMacro  ###
##############################
macro(rlcAddTool target)

	add_executable(${target} src/Main.cpp)
	add_executable(rlc::${target} ALIAS ${target})

	target_link_libraries(${target} PUBLIC ${ARGN})
	target_compile_features(${target} PUBLIC cxx_std_20)

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
	ADD_EXECUTABLE(${target}Benchmark src/${target}Benchmark.cpp) 
	ADD_EXECUTABLE(rlc::${target}Benchmark ALIAS ${target}Benchmark)

	include(FetchContent)
	FetchContent_Declare(
  		abseil
  		GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  		GIT_TAG        20240722.0
	)
	FetchContent_MakeAvailable(abseil)

	TARGET_LINK_LIBRARIES(${target}Benchmark PRIVATE benchmark::benchmark)
	TARGET_LINK_LIBRARIES(${target}Benchmark PRIVATE absl::flat_hash_map)
	TARGET_LINK_LIBRARIES(${target}Benchmark PRIVATE ${ARGN})
	TARGET_INCLUDE_DIRECTORIES(${target}Benchmark PUBLIC include PRIVATE src)
	TARGET_COMPILE_FEATURES(${target}Benchmark PUBLIC cxx_std_20)

	INSTALL(TARGETS ${target}Benchmark 
	RUNTIME DESTINATION bechmark)
endMacro(rlcAddBenchmark)
