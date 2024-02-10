cmake_minimum_required(VERSION 3.9)

##############################
###        googletest      ###
##############################
SET(BUILD_GMOCK ON CACHE INTERNAL "")
SET(BUILD_GTEST ON CACHE INTERNAL "")
SET(INSTALL_GMOCK OFF CACHE INTERNAL "")
SET(INSTALL_GTEST OFF CACHE INTERNAL "")
SET(gmock_build_tests OFF CACHE INTERNAL "")
SET(gtest_build_samples OFF CACHE INTERNAL "")
SET(gtest_build_tests OFF CACHE INTERNAL "")
SET(gtest_disable_pthreads OFF CACHE INTERNAL "")
SET(gtest_hide_internal_symbols OFF CACHE INTERNAL "")
SET(gtest_force_shared_crt OFF CACHE INTERNAL "")
SET(GTEST_HAS_ABSL OFF CACHE INTERNAL "")

add_subdirectory(googletest)
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=memory -fsanitize-ignorelist=/home/massimo/Documents/example_rlc/rlc-infrastructure/rlc-debug/ignorelist.txt -fsanitize-recover=all")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined")
#set (CMAKE_CXX_FLAGS "-fsanitize=fuzzer-no-link -O1")
##############################
###        benchmarks      ###
##############################
SET(BENCHMARK_ENABLE_INSTALL OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_RELEASE OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_EXCEPTIONS OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
SET(BENCHMARK_BUILD_32_BITS OFF CACHE INTERNAL "")
SET(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_DOXYGEN OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_ASSEMBLY_TESTS OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_LTO OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_LIBPFM OFF CACHE INTERNAL "")
SET(BENCHMARK_INSTALL_DOCS OFF CACHE INTERNAL "")
SET(BENCHMARK_USE_LIBCXX OFF CACHE INTERNAL "")
SET(BENCHMARK_FORCE_WERROR OFF CACHE INTERNAL "")
SET(BENCHMARK_ENABLE_WERROR ON CACHE INTERNAL "")
SET(BENCHMARK_USE_BUNDLED_GTEST CACHE INTERNAL "")

add_subdirectory(benchmark)

##############################
###        Project         ###
##############################

project(rlc VERSION 1.0 LANGUAGES CXX)
enable_testing()
include("CPack")
include(CTest)
set(CMAKE_DEBUG_POSTFIX "-d")

##############################
###        Globals         ###
##############################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-unused-parameter -Wno-comment -Wno-unused-function -Wno-deprecated-this-capture")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-int-float-conversion")

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 10)
include(macros.cmake)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

find_package(MLIR REQUIRED CONFIG)

##############################
###       Subfolders       ###
##############################
add_subdirectory(lib)
add_subdirectory(tool)

##############################
###         Python         ###
##############################
find_package(Python COMPONENTS Interpreter REQUIRED)
add_test(NAME pytest
COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python
)


##############################
###        DOXYGEN         ###
##############################
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

##############################
###        COVERAGE        ###
##############################
if (${CMAKE_CXX_FLAGS} MATCHES ".*--coverage.*")
	find_program(LCOV NAMES lcov)
	find_program(GENHTML NAMES genhtml)

	add_custom_command(
		OUTPUT base.info
		COMMAND ${LCOV} --capture --initial --directory . --output-file ./base.info
		VERBATIM
		)

	add_custom_command(
		OUTPUT filtered.info
		COMMAND ${LCOV} --remove ./total.info "/usr/*" "*/googletest/*" "*/build/*" "*/benchmark/*" --output-file ./filtered.info
		DEPENDS total.info
		VERBATIM)

	add_custom_command(
		OUTPUT total.info
		COMMAND ${LCOV} --add-tracefile ./base.info --add-tracefile ./test.info --output-file ./total.info
		DEPENDS test.info base.info
		VERBATIM)

	add_custom_command(
		OUTPUT test.info
		COMMAND ${LCOV} --capture --directory . --output-file ./test.info
		VERBATIM)

	add_custom_target(genCoverage
		COMMAND ${GENHTML} ./filtered.info --output-directory ./Coverage
		DEPENDS filtered.info
		BYPRODUCTS ./Coverage
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generate coverage"
		VERBATIM )

	add_custom_target(coverage COMMAND rm test.info filtered.info base.info total.info)
	add_dependencies(coverage genCoverage)
	add_dependencies(coverage test)
elseif(TRUE)
	add_custom_target(coverage
		COMMAND echo could not calculate coverage run cmake with -DCMAKE_CXX_FLAGS=--coverage
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Coverage not aviable"
		VERBATIM)
endif()


##############################
###         stdlib         ###
##############################
install(DIRECTORY ./stdlib DESTINATION ${CMAKE_INSTALL_LIBDIR}/rlc)