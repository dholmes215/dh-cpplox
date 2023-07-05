include(GenerateExportHeader)


add_library(lox_lib chunk.cpp debug.cpp memory.cpp value.cpp vm.cpp)



add_library(dh_cpplox::lox_lib ALIAS lox_lib)

target_link_libraries(lox_lib PRIVATE dh_cpplox_options dh_cpplox_warnings fmt::fmt)

target_include_directories(lox_lib ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                           $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(lox_lib PUBLIC cxx_std_20)

set_target_properties(
  lox_lib
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(lox_lib EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/dh_cpplox/lox_lib.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(lox_lib PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()
