function(rlc TARGET_NAME)
    set(options OPTIONAL SHARED COMPILE FUZ OPTIMIZE DEBUG_INFO)
    set(oneValueArgs DESTINATION INPUT OUTPUT HEADER PYTHON GODOT)
    set(multiValueArgs EXTRA_ARGS)
    cmake_parse_arguments(RLC
        "${options}" "${oneValueArgs}" "${multiValueArgs}"
        ${ARGN}
    )
    set(CREATED_FILES "")
    set(RLC_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/${RLC_INPUT})

    set(ARG "")
    if (${RLC_COMPILE})
        list(APPEND ARG "--compile")
    endif()
    if (${RLC_OPTIMIZE})
        list(APPEND ARG "-O2")
    endif()
    if (${RLC_DEBUG_INFO})
        list(APPEND ARG "-g")
    endif()
    if (${RLC_SHARED})
        list(APPEND ARG "--shared")
    endif()
    if (${RLC_FUZZ})
        list(APPEND ARG "--fuzzer")
    endif()

    if (NOT ${RLC_OUTPUT} STREQUAL "")
        add_custom_command(
            OUTPUT ${RLC_OUTPUT} ${RLC_OUTPUT}.dep
            COMMAND rlc ${RLC_INPUT} -o ${RLC_OUTPUT} -O2 ${RLC_EXTRA_ARGS} ${ARG} -MD
            DEPENDS ${RLC_INPUT}
            DEPFILE ${RLC_OUTPUT}.dep
        )

        list(APPEND CREATED_FILES ${RLC_OUTPUT})
    endif()

    if (NOT ${RLC_GODOT} STREQUAL "")
        add_custom_command(
            OUTPUT ${RLC_GODOT} ${RLC_GODOT}.dep
            COMMAND rlc ${RLC_INPUT} -o ${RLC_GODOT} --godot ${RLC_EXTRA_ARGS} -MD
            DEPFILE ${RLC_GODOT}.dep
            DEPENDS ${RLC_INPUT}
        )

        add_custom_target(${TARGET_NAME}_GODOT DEPENDS ${RLC_GODOT})
        list(APPEND CREATED_FILES ${RLC_GODOT})
    endif()

    if (NOT ${RLC_HEADER} STREQUAL "")
        add_custom_command(
            OUTPUT ${RLC_HEADER} ${RLC_HEADER}.dep
            COMMAND rlc ${RLC_INPUT} -o ${RLC_HEADER} --header ${RLC_EXTRA_ARGS} -MD
            DEPENDS ${RLC_INPUT}
            DEPFILE ${RLC_HEADER}.dep
        )

        add_custom_target(${TARGET_NAME}_HEADER DEPENDS ${RLC_HEADER})
        list(APPEND CREATED_FILES ${RLC_HEADER})
    endif()

    if (NOT ${RLC_PYTHON} STREQUAL "")
        add_custom_command(
            OUTPUT ${RLC_PYTHON} ${RLC_PYTHON}.dep
            COMMAND rlc ${RLC_INPUT} -o ${RLC_PYTHON} --python ${RLC_EXTRA_ARGS} -MD
            DEPENDS ${RLC_INPUT}
            DEPFILE ${RLC_PYTHON}.dep
        )
        add_custom_target(${TARGET_NAME}_PYTHON DEPENDS ${RLC_PYTHON})
        list(APPEND CREATED_FILES ${RLC_PYTHON})

        set(RLC_PYTHON_LIB lib${CMAKE_SHARED_LIBRARY_SUFFIX})
        add_custom_command(
            OUTPUT ${RLC_PYTHON_LIB} ${RLC_PYTHON_LIB}.dep
            COMMAND rlc ${RLC_INPUT} -o ${RLC_PYTHON_LIB} --shared ${RLC_EXTRA_ARGS} -MD
            DEPENDS ${RLC_INPUT}
            DEPFILE ${RLC_PYTHON_LIB}.dep
        )
        add_custom_target(${TARGET_NAME}_PYTHON_LIB DEPENDS ${RLC_PYTHON_LIB})
        list(APPEND CREATED_FILES ${RLC_PYTHON_LIB})
    endif()

    add_custom_target(${TARGET_NAME} DEPENDS ${CREATED_FILES})
endfunction(rlc)

set(RLC_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../include)
get_filename_component(RLC_INCLUDE_DIR ${RLC_INCLUDE_DIR} ABSOLUTE)
