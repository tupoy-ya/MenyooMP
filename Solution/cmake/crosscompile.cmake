option(CROSSCOMPILE "Enables crosscompiling you still need to set USE_GCC or USE_CLANG after this.")

if(CROSSCOMPILE)
    message(STATUS "Crosscompile is enabled. Using Clang and uasm.")

    set(CMAKE_SYSTEM_NAME "Windows")
    set(CMAKE_ASM_MASM_COMPILER "uasm")

    set(CMAKE_C_COMPILER_TARGET x86_64-windows-msvc)
    set(CMAKE_CXX_COMPILER_TARGET x86_64-windows-msvc)
    set(CMAKE_RC_COMPILER llvm-rc)

    add_compile_options("$<$<COMPILE_LANGUAGE:ASM_MASM>:-win64${ASM_MASM_FLAGS}>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-fuse-ld=lld${CXX_FLAGS}>")
    add_compile_definitions(CROSSCOMPILING)

    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
endif() 
