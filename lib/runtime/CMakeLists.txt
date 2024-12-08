
if (WIN32)

    rlcAddLibrary(runtime STATIC src/Parse.c)
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
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/runtime.lib ORIGINAL)
    add_custom_command(
      TARGET runtime POST_BUILD
      COMMAND xcopy /y ${ORIGINAL} ${UNPATCHED}*

      COMMAND lib /OUT:$<TARGET_FILE:rlc::runtime>  ${UNPATCHED} ${CMAKE_BINARY_DIR}/${RLC_MSVCRT} ${CMAKE_BINARY_DIR}/${RLC_KERNEL32} ${CMAKE_BINARY_DIR}/${RLC_VCRUNTIME} ${CMAKE_BINARY_DIR}/${RLC_UCRT}
      DEPENDS $<TARGET_FILE:rlc::runtime>
    )

else()
    rlcAddLibrary(runtime STATIC src/Runtime.c)
    set_property(TARGET runtime PROPERTY C_STANDARD 11)
endif()
INSTALL(FILES src/Runtime.c DESTINATION include/rlc/runtime)

find_package (Python3 COMPONENTS Interpreter Development)
add_library(pyrlc STATIC ${CMAKE_SOURCE_DIR}/python/native/call_python.c)
target_include_directories(pyrlc PUBLIC ${Python3_INCLUDE_DIRS})
target_link_libraries(pyrlc ${PYTHON_LIBRARIES})
add_library(rlc::pyrlc ALIAS pyrlc)
INSTALL(TARGETS pyrlc DESTINATION ${CMAKE_INSTALL_LIBDIR})
