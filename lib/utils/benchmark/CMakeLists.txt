rlcAddBenchmark(utils)
rlcAddBenchmark(dict)

# Data structure benchmark macro - no learn.rl needed
macro(makeDataStructureBenchmark name file)
    set(INS ${file})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX}
        COMMAND rlc::rlc ${INS} -o ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} --compile -O2 -i ${CMAKE_SOURCE_DIR}/stdlib
        DEPENDS rlc::rlc ${INS})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.h
        COMMAND rlc::rlc ${INS} -o ${CMAKE_CURRENT_BINARY_DIR}/${name}.h --header -O2 -i ${CMAKE_SOURCE_DIR}/stdlib
        DEPENDS rlc::rlc ${INS})
    add_custom_target(${name}_lib ALL DEPENDS 
        ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)

    rlcAddBenchmark(${name} ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} rlc::runtime)
    target_include_directories(${name}Benchmark PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    add_dependencies(${name}Benchmark ${name}_lib)
endMacro(makeDataStructureBenchmark)

# Generate dictionary benchmark
makeDataStructureBenchmark(DictIntBench ${CMAKE_SOURCE_DIR}/tool/rlc/test/dict_int_bench.rl)
makeDataStructureBenchmark(DictLargeKeyBench ${CMAKE_SOURCE_DIR}/tool/rlc/test/dict_largekey_bench.rl)
# Game benchmarks
macro(makeProgramBenchmark name file)
    set(INS ${file} ${CMAKE_SOURCE_DIR}/stdlib/learn.rl)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX}
        COMMAND rlc::rlc ${INS} -o ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} --compile -O2 -i ${CMAKE_SOURCE_DIR}/stdlib
        DEPENDS rlc::rlc ${INS})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.h
        COMMAND rlc::rlc ${INS} -o ${CMAKE_CURRENT_BINARY_DIR}/${name}.h --header -O2 -i ${CMAKE_SOURCE_DIR}/stdlib
        DEPENDS rlc::rlc ${INS})
    add_custom_target(${name}_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)

    rlcAddBenchmark(${name} ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_STATIC_LIBRARY_SUFFIX} rlc::runtime)
    target_include_directories(${name}Benchmark PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    add_dependencies(${name}Benchmark ${name}_lib)
endMacro(makeProgramBenchmark)

makeProgramBenchmark(TicTacToe ${CMAKE_SOURCE_DIR}/tool/rlc/test/tic_tac_toe.rl)
makeProgramBenchmark(Hanabi ${CMAKE_SOURCE_DIR}/tool/rlc/test/examples/hanabi.rl)
makeProgramBenchmark(Battleship ${CMAKE_SOURCE_DIR}/tool/rlc/test/examples/battleship.rl)
makeProgramBenchmark(Checkers ${CMAKE_SOURCE_DIR}/tool/rlc/test/examples/checkers.rl)
