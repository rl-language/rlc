rlcAddLibrary(runtime STATIC src/Parse.c)
set_property(TARGET runtime PROPERTY C_STANDARD 11)

if (WIN32)
    find_library(KERNEL32 NAMES Kernel32)
    find_library(VCRUNTIME NAMES vcruntime)
    find_library(MSVCRT NAMES msvcrt)
    find_library(UCRT NAMES ucrt)

    file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/runtime.lib RUNTIME_PATH)
    file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/unpatched.lib ORIGINAL_PATH)
    add_custom_command(
      TARGET runtime POST_BUILD
      COMMAND xcopy /y ${RUNTIME_PATH} ${ORIGINAL_PATH}*
      COMMAND lib /OUT:${RUNTIME_PATH} ${ORIGINAL_PATH} ${KERNEL32} ${VCRUNTIME} ${UCRT} ${MSVCRT}
    )
endif()
