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
SET(GTEST_HAS_RTTI OFF CACHE INTERNAL "")
SET(GTEST_INTERNAL_HAS_ANY OFF CACHE INTERNAL "")
SET(GTEST_LINKED_AS_SHARED_LIBRARY OFF CACHE INTERNAL "")
SET(gtest_force_shared_crt ON CACHE INTERNAL "")

add_subdirectory(googletest)
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=memory -fsanitize-ignorelist=/home/massimo/Documents/example_rlc/rlc-infrastructure/rlc-debug/ignorelist.txt -fsanitize-recover=all")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=undefined")
#set (CMAKE_CXX_FLAGS "-fsanitize=fuzzer-no-link -O0 -g")
#set (CMAKE_CXX_FLAGS "-g")
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


##############################
###        Project         ###
##############################

project(rlc VERSION 1.0 LANGUAGES CXX C)
enable_testing()
include("CPack")
include(CTest)
set(CMAKE_DEBUG_POSTFIX "-d")
set(CXX_STANDARD 20)

add_subdirectory(benchmark)

##############################
###        Globals         ###
##############################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-unused-parameter -Wno-comment -Wno-unused-function -Wno-deprecated-this-capture -Wno-dangling-assignment-gsl")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-implicit-int-float-conversion -Wno-reserved-macro-identifier -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-old-style-cast -Wno-sign-conversion -Wshadow-field-in-constructor -Wno-language-extension-token -Wno-deprecated-declarations -Wno-invalid-offsetof")

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})
set(LIBEXT ${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SHAREDEXT ${CMAKE_SHARED_LIBRARY_SUFFIX})

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 10)
include(macros.cmake)

find_package(LLVM REQUIRED CONFIG)
find_package(LLD REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

find_package(Clang REQUIRED CONFIG)

find_package(MLIR REQUIRED CONFIG)
set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})

##############################
###       Subfolders       ###
##############################
add_subdirectory(lib)
add_subdirectory(tool)

##############################
###         Python         ###
##############################
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
add_test(NAME pytest
COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python
)

if (UNIX AND NOT APPLE)
add_custom_target(pip_package
    COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/dist && ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/setup.py sdist bdist_wheel && cp -r ${CMAKE_SOURCE_DIR}/python/dist ${CMAKE_CURRENT_BINARY_DIR}/ && rm -r ${CMAKE_SOURCE_DIR}/python/dist ${CMAKE_SOURCE_DIR}/python/build
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/python
    DEPENDS install)
endif()
if (APPLE)
    add_custom_target(pip_package
        COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/dist && ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/setup.py sdist bdist_wheel --plat-name=macosx_11_0_arm64 && cp -r ${CMAKE_SOURCE_DIR}/python/dist ${CMAKE_CURRENT_BINARY_DIR}/ && rm -r ${CMAKE_SOURCE_DIR}/python/dist ${CMAKE_SOURCE_DIR}/python/build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/python
        DEPENDS install)
endif()

##############################
###       STDLIBDOC        ###
##############################
file(GLOB_RECURSE STDLIBFILES "${CMAKE_SOURCE_DIR}/stdlib/*.rl")
foreach(RLFILE ${STDLIBFILES})
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}" "${RLFILE}")
    string(REPLACE ".rl" ".md" MD_FILE "${REL_PATH}")
    set(MD_FILE_PATH "${CMAKE_BINARY_DIR}/${MD_FILE}")

    add_custom_command(
        OUTPUT ${MD_FILE_PATH}
        COMMAND rlc-doc ${RLFILE} -o ${MD_FILE_PATH}  --create-dirs=true
        DEPENDS rlc-doc ${RLFILE})
    list(APPEND MD_FILES "${MD_FILE_PATH}")
endforeach()

add_custom_target(stdlibdoc ALL DEPENDS ${MD_FILES})


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

##############################
###     cmake config       ###
##############################
configure_package_config_file(
    rlcConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/rlcConfig.cmake
    INSTALL_DESTINATION share/rlc/cmake
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rlcConfig.cmake
        DESTINATION share/rlc/cmake)
