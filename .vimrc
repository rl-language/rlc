let CMAKE_TYPE = 0
let BUILD_DIRECTORY = "../rlc-debug"
let CPPCOMPILER = "g++" 
let CCOMPILER = "gcc"
let BUILD_TYPE = "Debug"
let EXTRA_CONFIG = ""
let GENERATOR = "Ninja"
let TARGET = "dialect"
let RUN_ARGS = ""
let TEST = g:TARGET . "Test"
let LINKER = "gold"
let PYTHON  = "python3"
let PLOTTER = "./plot.py"

let g:ale_enabled = 0

let DBGLLVM_LOC = "/home/massimo/Documents/example_rlc/rlc-infrastructure/llvm-install-debug/lib/cmake/llvm/"
let RELLLVM_LOC = "/home/massimo/Documents/example_rlc/rlc-infrastructure/llvm-install-release/lib/cmake/llvm/"
let s:path = expand('<sfile>:p:h')

let g:ycm_clangd_binary_path = exepath("/home/massimo/Documents/example_rlc/rlc-infrastructure/llvm-install-release")


let g:ctrlp_custom_ignore = '\v[\/](docs|release|build|gli|glbinding|benchmark|googletest|boostGraph|glfw|glm)|(\.(swp|ico|git|svn|lock))$'

function! s:updateCmake()

	call AQAppend(":lcd " . g:BUILD_DIRECTORY)
	let l:t = AQAppend(s:getBuildCommand())
	call AQAppend(":lcd " . s:path)
	call AQAppendOpen(0, l:t)
	call AQAppendCond("!cp " . g:BUILD_DIRECTORY . "/compile_commands.json " . s:path, 1, l:t)
endfunction

function! s:setType(val, cmakeBuildBir, cCompiler, cppCompiler, buildType, extra, generator)
	let g:CMAKE_TYPE = a:val
	let g:BUILD_DIRECTORY = a:cmakeBuildBir
	let g:CCOMPILER = a:cCompiler
	let g:CPPCOMPILER = a:cppCompiler
	let g:BUILD_TYPE = a:buildType
	let g:EXTRA_CONFIG = a:extra
	let g:GENERATOR = a:generator

	call s:updateCmake()
endfunction

function! s:getBuildCommand()
	let s:command =  "!cmake -DCMAKE_BUILD_TYPE=" . g:BUILD_TYPE .  " -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_INSTALL_PREFIX=./install" . " -DCMAKE_C_COMPILER=" . g:CCOMPILER . " -DCMAKE_CXX_COMPILER=" . g:CPPCOMPILER . " -G " . g:GENERATOR . " -DCMAKE_EXE_LINKER_FLAGS='-fuse-ld=" . g:LINKER ."' " . g:EXTRA_CONFIG . " -S " . s:path
	return s:command
endfunction

function! s:Rebuild()
	call AQAppend("!rm -r ./" . g:BUILD_DIRECTORY)
	call AQAppend("!mkdir ./" . g:BUILD_DIRECTORY)
	call s:updateCmake()
endfunction

function! s:RunLIT(file)
	let l:t = s:silentBuild("rlc")
	call AQAppendOpen(0, l:t)
	call AQAppendCond("call ParseClangOutput()", 0, l:t)

	let l:t2 = AQAppendCond("!lit " . g:BUILD_DIRECTORY . "/tool/rlc/test --verbose --filter=".a:file, 1, l:t)
	call AQAppendOpen(-1, l:t2)
endfunction

function! s:RunCompileAndExecute(file)
	let l:t = s:SilentRun("rlc", "tool/rlc/rlc", a:file . " -o /tmp/compiled")
	call AQAppendOpenError(0, l:t[0])
	call AQAppendOpenError(0, l:t[1])
	call AQAppendCond("call ParseClangOutput()", 0, l:t[1])
	let l:t2 = AQAppendCond("!/tmp/compiled", 1, l:t[1])
endfunction

function! s:RunTest(param, executible, args)
	let l:t = s:SilentRun(a:param, a:executible, a:args)
	call AQAppendOpen(0, l:t[0])
	call AQAppendCond("call ParseClangOutput()", 0, l:t[0])

	call AQAppendOpen(-1, l:t[1])
	call AQAppendCond("call RunOnBuffer()", -1, l:t[1])
	call AQAppendCond("call ApplyTestSyntax()", -1, l:t[1])

	call AQAppend("setlocal nomodified")
endfunction

function! s:silentBuild(target)
	let s:build = "!cmake --build " . g:BUILD_DIRECTORY . " --target " . a:target 
	call AQAppendOpen(0)
	return AQAppend(s:build)
endfunction

function! s:SilentRun(target, executible, args)
	let s:exec = "!./" . g:BUILD_DIRECTORY . "/" . a:executible . " " . a:args
	let l:ret = s:silentBuild(a:target)
	return [l:ret, AQAppendCond(s:exec, 1, l:ret)]
endfunction

function! s:Run(param, executible, args)
	let l:r = s:SilentRun(a:param, a:executible, a:args)
	call AQAppendOpen(0, l:r[0])
	call AQAppendCond("call ParseClangOutput()", 0, l:r[0])

	call AQAppendOpen(-1, l:r[1])
	call AQAppendCond("setlocal nomodified")

	call AQAppendOpenError(0, l:r[1])
	call AQAppendCond("call AsanParseBuffer()", 0, l:r[1])

	call AQAppendCond("setlocal nomodified")
endfunction

function! s:RunD(target, executible, args)
	let s:exec = "./" . g:BUILD_DIRECTORY . "/" . a:executible . " " . a:args

	let l:t = s:silentBuild(a:target)
	call AQAppendCond("Termdebug -r --args " . s:exec, 1, l:t)

endfunction

function! s:goToTest(name)
	execute "vimgrep " . a:name . " ../" . g:TARGET . "/test/src/*.cpp"	. " **/" . g:TARGET . "/test/src/*.cpp"
endfunction

function! s:setTarget(name)
	let g:TARGET = a:name
endfunction

function! s:setTargetArgs(args)
	let g:RUN_ARGS = a:args
endfunction

function! s:genDoc()
	let l:ret = s:silentBuild("doc")
	call AQAppendCond('QTBSearch ./build/doc_doxygen/html/index.html', 1, l:ret)
endfunction

function! s:genCoverage()
	let l:ret = s:silentBuild("all")
	let l:ret = s:silentBuild("coverage")
	call AQAppendCond('QTBSearch ./build/Coverage/index.html', 1, l:ret)
endfunction

function! s:getFlags(otherArgs, extraCMargs)
	let l:ret = '-DCMAKE_CXX_FLAGS="' . a:otherArgs . '" '
	let l:ret = l:ret . a:extraCMargs 
	return l:ret
endfunction

function! s:runBenchmark()
	let l:t = s:silentBuild(g:TARGET."Benchmark")
	call AQAppendOpen(0, l:t)
	call AQAppendCond("call ParseClangOutput()", 0, l:t[0])

	let outfile = g:BUILD_DIRECTORY . "/lib/" . g:TARGET . "/benchmark/lastResult.csv"
	let command = "execute '!./" . g:BUILD_DIRECTORY ."/lib/" . g:TARGET . "/benchmark/" . g:TARGET . "Benchmark --benchmark_format=csv > ". l:outfile . "'"
	let command2 = "execute '!" . g:PYTHON . " " . g:PLOTTER . " < " . l:outfile . "'"
	silent call AQAppendCond(l:command, 1, l:t)
	call AQAppendCond(l:command2, 1, l:t)
endfunction

let EXTRA_CONFIG = s:getFlags('--coverage', "-DLLVM_DIR=".g:DBGLLVM_LOC)

command! -nargs=0 CMDEBUG call s:setType(0, "../rlc-debug", "gcc", "g++", "Debug", s:getFlags('--coverage ', "-DBUILD_SHARED_LIBS=ON -DLLVM_DIR=".g:DBGLLVM_LOC . " -DMLIR_DIR=".g:DBGLLVM_LOC . "/../mlir"), "Ninja")
command! -nargs=0 CMRELEASE call s:setType(1, "../rlc-release", "clang", "clang++", "Release", s:getFlags('', "-DBUILD_SHARED_LIBS=OFF -DLLVM_DIR=".g:RELLLVM_LOC . " -DMLIR_DIR=".g:RELLLVM_LOC . "/../mlir"), "Ninja")
command! -nargs=0 CMASAN call s:setType(2, "../rlc-debug", "clang", "clang++", "Debug", s:getFlags(' -fsanitize=address -fno-omit-frame-pointer',"-DLLVM_DIR=".g:DBGLLVM_LOC ), "Ninja")
command! -nargs=0 CMFUZZER call s:setType(5, "../rlc-debug", "clang", "clang++", "Debug", s:getFlags('', "-DBUILD_FUZZER=ON"), "Ninja")
command! -nargs=0 CMTSAN call s:setType(3, "../rlc-debug", "clang", "clang++", "Debug", s:getFlags('-fsanitize=thread -O1', "-DLLVM_DIR=".g:DBGLLVM_LOC), "Ninja")
command! -nargs=0 CMMSAN call s:setType(4, "../rlc-debug", "clang", "clang++", "Debug", s:getFlags('-fsanitize=memory -fPIE -fno-omit-frame-pointer -fsanitize-blacklist=../msan_ignores.txt', "-DLLVM_DIR=".g:DBGLLVM_LOC), "Ninja")

command! -nargs=0 REBUILD call s:Rebuild()
command! -nargs=0 TALL call s:RunTest(g:TARGET . "Test", "lib/".g:TARGET . "/test/" . g:TEST, "")
command! -nargs=0 TSUIT call s:RunTest(g:TARGET . "Test", "lib/".g:TARGET . "/test/" . g:TEST, GTestOption(1))
command! -nargs=0 TONE call s:RunTest(g:TARGET . "Test", "lib/".g:TARGET . "/test/" . g:TEST, GTestOption(0))
command! -nargs=0 LIT call s:RunLIT(expand("%:t"))
command! -nargs=0 RUN call s:RunCompileAndExecute(expand("%:p"))
command! -nargs=0 DTALL call s:RunD(g:TARGET . "Test", "lib/".g:TARGET . "/test/" . g:TEST, "")
command! -nargs=0 DTSUIT call s:RunD(g:TARGET . "Test", "lib/".g:TARGET . "/test/" . g:TEST, GTestOption(1))
command! -nargs=0 DTONE call s:RunD(g:TARGET . "Test", "lib/".g:TARGET . "/test/" . g:TEST, GTestOption(0))
command! -nargs=0 DRUN call s:RunD("rlc", "tool/rlc/rlc", expand("%:p") . " -o /tmp/compiled")
command! -nargs=0 GOTOTEST call s:goToTest(expand("<cword>"))
command! -nargs=1 SETTARGET call s:setTarget(<f-args>)
command! -nargs=1 SETARGS call s:setTargetArgs(<f-args>)
command! -nargs=0 BENCHMARK call s:runBenchmark()
command! -nargs=0 DOC call s:genDoc()
command! -nargs=0 COVERAGE call s:genCoverage()

nnoremap <leader><leader>gt :vsp<cr>:GOTOTEST<cr>
nnoremap <leader><leader>b :REBUILD<cr>
nnoremap <leader><leader>r :RUN<cr>
nnoremap <leader><leader>l :LIT<cr>
nnoremap <leader><leader>dr :DRUN<cr>
nnoremap <leader><leader>ta :TALL<cr>
nnoremap <leader><leader>dta :DTALL<cr>
nnoremap <leader><leader>ts :TSUIT<cr>
nnoremap <leader><leader>dts :DTSUIT<cr>
nnoremap <leader><leader>to :TONE<cr>
nnoremap <leader><leader>dto :DTONE<cr>
nnoremap <leader><leader>tc :BENCHMARK<cr>
map <leader><leader>L i<
map <leader><leader>G i>

command! -nargs=1 Rename call s:clangRename(<f-args>)
exe "hi clangOutError ctermfg="g:ColorStatement
	exe "hi clangOutNote ctermfg="g:ColorNumber
	exe "hi clangOutFile ctermfg="g:ColorType
	exe "hi clangOutFile ctermfg="g:ColorString

function! ParseClangOutput()
	syntax match clangOutError '\s\+error:\s\+'
	syntax match clangOutNote '\s\+note:\s\+'
	syntax match clangOutNote '\s\+warning:\s\+'
	syntax match clangOutFile '\S\+:\d\+:\d\+:'
endfunction

function! s:clangRename(newName)
	let s:offset = line2byte(line(".")) + col(".") - 2
	let command = "!clang-rename -offset=" . s:offset . " -i -new-name=" . a:newName . " " . expand('%:t')
	call AQAppend(command)
	call AQAppendCond("checktime", 1)
endfunction

