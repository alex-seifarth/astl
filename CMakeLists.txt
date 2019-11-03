cmake_minimum_required(VERSION 3.14)
project(astl VERSION 0.0.1 LANGUAGES CXX)

include(cmake/gtest.cmake)
include(cmake/doxygen.cmake)

astl_setup_doxygen()
astl_setup_gtest()

set(ASTL_COMPONENTS
    core
)

message(STATUS " * ASTL version         ${PROJECT_VERSION}")
message(STATUS " * CXX Compiler         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " * ASTL_GTEST           ${ASTL_GTESTS}")
message(STATUS " * ASTL_CTEST           ${ASTL_CTEST}")
message(STATUS " * Doxygen              ${DOXYGEN_FOUND} (v${DOXYGEN_VERSION})")
message(STATUS " * Components           ${ASTL_COMPONENTS}")

add_subdirectory(src)

foreach (component ${ASTL_COMPONENTS})
    add_subdirectory(${component})
endforeach()
add_custom_target(astl-lib DEPENDS ${ASTL_COMPONENTS})

if (ASTL_GTESTS)
    set(ASTL_TESTS ${ASTL_COMPONENTS})
    list(TRANSFORM ASTL_TESTS APPEND -tests)
    add_custom_target(astl-tests DEPENDS ${ASTL_TESTS})
endif()

include(GNUInstallDirs)
install(EXPORT astl-exports
    NAMESPACE astl
    DESTINATION ${INSTALL_CMAKE_DIR} ${CMAKE_INSTALL_LIBDIR}/cmake/astl
)
