
function(astl_enable_gtest)
    find_package(GTest REQUIRED)
    enable_testing()
    add_subdirectory(${CMAKE_SOURCE_DIR}/gtests)
endfunction()
