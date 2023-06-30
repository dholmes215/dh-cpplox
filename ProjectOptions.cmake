include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(dh_cpplox_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(dh_cpplox_setup_options)
  option(dh_cpplox_ENABLE_HARDENING "Enable hardening" ON)
  option(dh_cpplox_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    dh_cpplox_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    dh_cpplox_ENABLE_HARDENING
    OFF)

  dh_cpplox_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR dh_cpplox_PACKAGING_MAINTAINER_MODE)
    option(dh_cpplox_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(dh_cpplox_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(dh_cpplox_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(dh_cpplox_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(dh_cpplox_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(dh_cpplox_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(dh_cpplox_ENABLE_PCH "Enable precompiled headers" OFF)
    option(dh_cpplox_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(dh_cpplox_ENABLE_IPO "Enable IPO/LTO" ON)
    option(dh_cpplox_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(dh_cpplox_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(dh_cpplox_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(dh_cpplox_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(dh_cpplox_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(dh_cpplox_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(dh_cpplox_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(dh_cpplox_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(dh_cpplox_ENABLE_PCH "Enable precompiled headers" OFF)
    option(dh_cpplox_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      dh_cpplox_ENABLE_IPO
      dh_cpplox_WARNINGS_AS_ERRORS
      dh_cpplox_ENABLE_USER_LINKER
      dh_cpplox_ENABLE_SANITIZER_ADDRESS
      dh_cpplox_ENABLE_SANITIZER_LEAK
      dh_cpplox_ENABLE_SANITIZER_UNDEFINED
      dh_cpplox_ENABLE_SANITIZER_THREAD
      dh_cpplox_ENABLE_SANITIZER_MEMORY
      dh_cpplox_ENABLE_UNITY_BUILD
      dh_cpplox_ENABLE_CLANG_TIDY
      dh_cpplox_ENABLE_CPPCHECK
      dh_cpplox_ENABLE_COVERAGE
      dh_cpplox_ENABLE_PCH
      dh_cpplox_ENABLE_CACHE)
  endif()

  dh_cpplox_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (dh_cpplox_ENABLE_SANITIZER_ADDRESS OR dh_cpplox_ENABLE_SANITIZER_THREAD OR dh_cpplox_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(dh_cpplox_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(dh_cpplox_global_options)
  if(dh_cpplox_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    dh_cpplox_enable_ipo()
  endif()

  dh_cpplox_supports_sanitizers()

  if(dh_cpplox_ENABLE_HARDENING AND dh_cpplox_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR dh_cpplox_ENABLE_SANITIZER_UNDEFINED
       OR dh_cpplox_ENABLE_SANITIZER_ADDRESS
       OR dh_cpplox_ENABLE_SANITIZER_THREAD
       OR dh_cpplox_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${dh_cpplox_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${dh_cpplox_ENABLE_SANITIZER_UNDEFINED}")
    dh_cpplox_enable_hardening(dh_cpplox_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(dh_cpplox_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(dh_cpplox_warnings INTERFACE)
  add_library(dh_cpplox_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  dh_cpplox_set_project_warnings(
    dh_cpplox_warnings
    ${dh_cpplox_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(dh_cpplox_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(dh_cpplox_options)
  endif()

  include(cmake/Sanitizers.cmake)
  dh_cpplox_enable_sanitizers(
    dh_cpplox_options
    ${dh_cpplox_ENABLE_SANITIZER_ADDRESS}
    ${dh_cpplox_ENABLE_SANITIZER_LEAK}
    ${dh_cpplox_ENABLE_SANITIZER_UNDEFINED}
    ${dh_cpplox_ENABLE_SANITIZER_THREAD}
    ${dh_cpplox_ENABLE_SANITIZER_MEMORY})

  set_target_properties(dh_cpplox_options PROPERTIES UNITY_BUILD ${dh_cpplox_ENABLE_UNITY_BUILD})

  if(dh_cpplox_ENABLE_PCH)
    target_precompile_headers(
      dh_cpplox_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(dh_cpplox_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    dh_cpplox_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(dh_cpplox_ENABLE_CLANG_TIDY)
    dh_cpplox_enable_clang_tidy(dh_cpplox_options ${dh_cpplox_WARNINGS_AS_ERRORS})
  endif()

  if(dh_cpplox_ENABLE_CPPCHECK)
    dh_cpplox_enable_cppcheck(${dh_cpplox_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(dh_cpplox_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    dh_cpplox_enable_coverage(dh_cpplox_options)
  endif()

  if(dh_cpplox_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(dh_cpplox_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(dh_cpplox_ENABLE_HARDENING AND NOT dh_cpplox_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR dh_cpplox_ENABLE_SANITIZER_UNDEFINED
       OR dh_cpplox_ENABLE_SANITIZER_ADDRESS
       OR dh_cpplox_ENABLE_SANITIZER_THREAD
       OR dh_cpplox_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    dh_cpplox_enable_hardening(dh_cpplox_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
