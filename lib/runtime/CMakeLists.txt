rlcRuntime(runtime ${CMAKE_CURRENT_SOURCE_DIR}/src/Runtime.c)
rlcRuntime(fuzzer ${CMAKE_CURRENT_SOURCE_DIR}/src/fuzz_target.c)

find_package (Python3 COMPONENTS Interpreter Development)
find_package(PythonLibs REQUIRED)
rlcRuntime(pyrlc ${CMAKE_SOURCE_DIR}/python/native/call_python.c -I ${Python3_INCLUDE_DIRS})


INSTALL(FILES src/Runtime.c DESTINATION include/rlc/runtime)

if (WIN32)

    rlcAddLibrary(runtime STATIC src/Runtime.c)
    set_property(TARGET runtime PROPERTY C_STANDARD 11)

    if(NOT DEFINED RLC_VCRUNTIME)
      message(FATAL_ERROR "specify vcruntime.lib location with RLC_VCRUNTIME")
    endif(NOT DEFINED RLC_VCRUNTIME)
    if(NOT DEFINED RLC_UCRT)
	    message(FATAL_ERROR "specify ucrt.lib location with RLC_UCRT")
    endif(NOT DEFINED RLC_UCRT)
    if(NOT DEFINED RLC_MSVCRT)
	    message(FATAL_ERROR "specify ucrt.lib location with RLC_MSVCRT")
    endif(NOT DEFINED RLC_MSVCRT)
    if(NOT DEFINED RLC_KERNEL32)
	    message(FATAL_ERROR "specify ucrt.lib location with RLC_KERNEL32")
    endif(NOT DEFINED RLC_KERNEL32)

    file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/unpatched.lib UNPATCHED)
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/runtime$<$<CONFIG:Debug>:-d>.lib ORIGINAL)
    add_custom_command(
      TARGET runtime POST_BUILD
      COMMAND xcopy /y ${ORIGINAL} ${UNPATCHED}*

      COMMAND lib /OUT:$<TARGET_FILE:rlc::runtime>  ${UNPATCHED} ${CMAKE_BINARY_DIR}/${RLC_MSVCRT} ${CMAKE_BINARY_DIR}/${RLC_KERNEL32} ${CMAKE_BINARY_DIR}/${RLC_VCRUNTIME} ${CMAKE_BINARY_DIR}/${RLC_UCRT}
      DEPENDS $<TARGET_FILE:rlc::runtime>
    )

file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/pyunpatched.lib PYUNPATCHED)
file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/pyrlc$<$<CONFIG:Debug>:-d>.lib PYORIGINAL)
add_custom_command(
      TARGET pyrlc POST_BUILD
      COMMAND xcopy /y ${PYORIGINAL} ${PYUNPATCHED}*

      COMMAND lib /OUT:$<TARGET_FILE:rlc::pyrlc>  ${PYUNPATCHED} C:/Users/mofio/AppData/Local/Programs/Python/Python311/libs/python3.lib
      DEPENDS $<TARGET_FILE:rlc::pyrlc>
)
endif()
