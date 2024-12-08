function(rlc TARGET_NAME)
    set(options OPTIONAL SHARED COMPILE FUZ)
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
    if (${RLC_SHARED})
        list(APPEND ARG "--shared")
    endif()
    if (${RLC_FUZZ})
        list(APPEND ARG "--fuzzer")
    endif()

    if (NOT ${RLC_OUTPUT} STREQUAL "")
        add_custom_command(
            OUTPUT ${RLC_OUTPUT} 
            COMMAND rlc ${RLC_INPUT} -o ${RLC_OUTPUT} -O2 ${RLC_EXTRA_ARGS} ${ARG} -MD
            DEPENDS ${RLC_INPUT}
            DEPFILE ${RLC_OUTPUT}.dep
        )

        list(APPEND CREATED_FILES ${RLC_OUTPUT})
    endif()

    if (NOT ${RLC_GODOT} STREQUAL "")
        set(RLC_GODOT ${CMAKE_CURRENT_BINARY_DIR}/${RLC_GODOT})
        get_filename_component(RLC_GODOT_FILENAME ${RLC_GODOT} NAME)
        set(TMP_GODOT ${CMAKE_CURRENT_BINARY_DIR}/${RLC_GODOT_FILENAME}.tmp)
        add_custom_command(
            OUTPUT ${RLC_GODOT} 
            COMMAND rlc ${RLC_INPUT} -o ${TMP_GODOT} -O2 --godot ${RLC_EXTRA_ARGS} -MD
            COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${TMP_GODOT}  ${RLC_GODOT}
            DEPFILE ${RLC_GODOT}.dep
            DEPENDS ${RLC_INPUT}
        )

        add_custom_target(${TARGET_NAME}_GODOT DEPENDS ${RLC_GODOT})
        list(APPEND CREATED_FILES ${RLC_GODOT})
    endif()

    if (NOT ${RLC_HEADER} STREQUAL "")
        set(RLC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${RLC_HEADER})
        get_filename_component(RLC_HEADER_FILENAME ${RLC_HEADER} NAME)
        set(TMP_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${RLC_HEADER_FILENAME}.tmp)
        add_custom_command(
            OUTPUT ${RLC_HEADER} 
            COMMAND rlc ${RLC_INPUT} -o ${TMP_HEADER} -O2 --header ${RLC_EXTRA_ARGS} -MD
            COMMAND ${CMAKE_COMMAND} -E  copy_if_different ${TMP_HEADER}  ${RLC_HEADER}
            DEPENDS ${RLC_INPUT}
            DEPFILE ${RLC_HEADER}.dep
        )

        add_custom_target(${TARGET_NAME}_HEADER DEPENDS ${RLC_HEADER})
        list(APPEND CREATED_FILES ${RLC_HEADER})
    endif()

    if (NOT ${RLC_PYTHON} STREQUAL "")
        set(RLC_PYTHON ${CMAKE_CURRENT_BINARY_DIR}/${RLC_PYTHON})
        get_filename_component(RLC_PYTHON_FILENAME ${RLC_PYTHON} NAME)
        set(TMP_PYTHON ${CMAKE_CURRENT_BINARY_DIR}/${RLC_PTYHON_FILENAME}.tmp)

        add_custom_command(
            OUTPUT ${RLC_PYTHON} 
            COMMAND rlc ${RLC_INPUT} -o ${TMP_PYTHON} --python ${RLC_EXTRA_ARGS} -MD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TMP_PYTHON}  ${RLC_PYTHON}
            DEPENDS ${RLC_INPUT}
            DEPFILE ${RLC_PYTHON}.dep
        )
        add_custom_target(${TARGET_NAME}_PYTHON DEPENDS ${RLC_PYTHON})
        list(APPEND CREATED_FILES ${RLC_PYTHON})

        get_filename_component(RLC_PYTHON_DIR ${RLC_PYTHON} DIRECTORY)
        set(RLC_PYTHON_LIB ${RLC_PYTHON_DIR}/lib${CMAKE_SHARED_LIBRARY_SUFFIX})
        add_custom_command(
            OUTPUT ${RLC_PYTHON_LIB} 
            COMMAND rlc ${RLC_INPUT} -o ${RLC_PYTHON_LIB} --shared -O2 ${RLC_EXTRA_ARGS} -MD
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
