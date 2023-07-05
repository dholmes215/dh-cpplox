add_executable(cpplox main.cpp)

target_link_libraries(
  cpplox
  PRIVATE dh_cpplox::dh_cpplox_options
          dh_cpplox::dh_cpplox_warnings)

target_link_system_libraries(
  cpplox
  PRIVATE
          lox_lib
          fmt::fmt
          Microsoft.GSL::GSL
          lefticus::tools)

target_include_directories(cpplox PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
