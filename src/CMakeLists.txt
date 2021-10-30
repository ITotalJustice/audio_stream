cmake_minimum_required(VERSION 3.18.0)

add_library(stream
    stream.cpp
    decoders/flac/flac.cpp
    decoders/mp3/mp3.cpp
    decoders/ogg/ogg.cpp
    decoders/wav/wav.cpp
    resamplers/SDL_audioEX.c
)

target_include_directories(stream PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(stream PROPERTIES CXX_STANDARD 20)


# (C)flags for each compiler
# Werror is disabled atm because it's very annoying early into a project
list(APPEND gcc_flags
    -Wall
    -Wextra
    # -Werror
    -Wfatal-errors
    # -Wpedantic
    -Wshadow
    -Wdouble-promotion
    -Wformat=2
    -Wformat-overflow=2
    -Wformat-truncation=1
    -Wundef
    -Wmissing-include-dirs
    -fstrict-aliasing
    -Wstrict-aliasing=3
    -Wstrict-overflow=5
    -Walloca
    -Wduplicated-branches
    -Wduplicated-cond
    -Wwrite-strings
    -Wdate-time
    -Wlogical-op
    -Wpacked
    -Wredundant-decls
    -Wnested-externs
    -Wcast-qual
    -Wcast-align=strict
    -Wunused-macros
    -Wbad-function-cast
)

list(APPEND clang_flags
    -Wall
    -Wextra
    # -Werror
    -Wfatal-errors
    # -Wpedantic
    -Wshadow
    -Wdouble-promotion
    -Wformat=2
    -Wundef
    -Wmissing-include-dirs
    -Wstrict-aliasing=2
    -Wstrict-overflow=5
    -Walloca
    -Wwrite-strings
    -Wdate-time
    -Wpacked
    -Wredundant-decls
    -Wnested-externs
    -Wcast-qual
    -Wcast-align
    -Wunused-macros
    -Wreserved-id-macro
    -Wbad-function-cast
    -Wbitwise-op-parentheses
    -Wbitfield-enum-conversion
    -Wno-missing-braces # false positive

    # can try enabling this just to see what breaks (still compiles as of clang 11)
    # be sure to enable (well, disable) the below flags else a *lot* of "errors"
    # -Weverything
    # -Wno-conversion
    # -Wno-sign-conversion
    # -Wno-missing-prototypes
    # -Wno-padded
)

list(APPEND tcc_flags
    -Wall
    # -Werror
    -Wwrite-strings
    -Wunsupported
)

list(APPEND msvc_flags
    /W4
    # /WX
)

include(CheckCCompilerFlag)
include(CheckLinkerFlag)

function(add_compiler_flag flag scope)
    # check_c_compiler_flag() adds a macro to the test.c file such that
    # it'll look like # will be shown as "#define COMPILER_SUPPORTS_-Wdouble-promotion"
    # which is not a valid macro! because of this, we have to remove all invalid characters
    string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" check_flag ${flag})
    check_c_compiler_flag(${flag} CHECK_FLAG_${check_flag})

    if (CHECK_FLAG_${check_flag})
        target_compile_options(stream ${scope} ${flag})
    endif()
endfunction()

function(add_linker_flag flag scope)
    string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" check_flag ${flag})
    check_linker_flag("C" ${flag} CHECK_FLAG_${check_flag})

    if (CHECK_FLAG_${check_flag})
        target_link_options(stream ${scope} ${flag})
        message("has linker flag ${flag}")
    else()
        message("does NOT have linker flag ${flag}")
    endif()
endfunction()

function(add_compiler_flags flag_list scope)
    foreach (flag IN LISTS ${flag_list})
       add_compiler_flag(${flag} ${scope})
    endforeach()
endfunction()

function(add_linker_flags flag_list scope)
    foreach (flag IN LISTS ${flag_list})
       add_linker_flag(${flag} ${scope})
    endforeach()
endfunction()

# enable warning flags for each compiler
# only gcc, clang, tcc and msvc are supported so far
if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compiler_flags(clang_flags PRIVATE)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compiler_flags(gcc_flags PRIVATE)
elseif (CMAKE_C_COMPILER_ID STREQUAL "TinyCC")
    add_compiler_flags(tcc_flags PRIVATE)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compiler_flags(msvc_flags PRIVATE)
endif()
