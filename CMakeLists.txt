cmake_minimum_required(VERSION 3.12)
project(astl VERSION 0.0.1 LANGUAGES CXX C)

include(cmake/gtest.cmake)
include(cmake/doxygen.cmake)

astl_setup_doxygen()
astl_setup_gtest()

set(ASTL_COMPONENTS
    core
    rlp
)

message(STATUS " * ASTL version         ${PROJECT_VERSION}")
message(STATUS " * CXX Compiler         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " * ASTL_GTEST           ${ASTL_GTESTS}")
message(STATUS " * ASTL_CTEST           ${ASTL_CTEST}")
message(STATUS " * Doxygen              ${DOXYGEN_FOUND} (v${DOXYGEN_VERSION})")
message(STATUS " * Components           ${ASTL_COMPONENTS}")

add_subdirectory(src)

set(ASTL_INTERFACE_COMPILE_FEATURES
    cxx_std_17 cxx_auto_type cxx_constexpr cxx_decltype cxx_defaulted_move_initializers cxx_delegating_constructors
    cxx_digit_separators cxx_explicit_conversions cxx_final cxx_inheriting_constructors cxx_inline_namespaces
    cxx_lambdas cxx_noexcept cxx_nullptr cxx_override cxx_range_for cxx_right_angle_brackets cxx_rvalue_references
    cxx_sizeof_member cxx_static_assert cxx_strong_enums cxx_uniform_initialization cxx_variable_templates
    cxx_variadic_templates cxx_template_template_parameters
)

set(ASTL_INTERFACE_COMPILE_OPTIONS
    -Wall -Wextra -pedantic -Werror
)

set(INSTALL_TARGET_EXPORT astl)

foreach(component ${ASTL_COMPONENTS})
    add_subdirectory(${component})
endforeach()
add_custom_target(astl-lib DEPENDS ${ASTL_COMPONENTS})

if (ASTL_GTESTS)
    set(ASTL_TESTS_LIST)
    foreach(comp ${ASTL_COMPONENTS})
        set(test_name "${comp}-tests")
        if(TARGET ${test_name})
            list(APPEND ASTL_TESTS_LIST "${test_name}")
        endif()
    endforeach()
    message(STATUS " * GTESTS               ${ASTL_TESTS_LIST}")
    add_custom_target(astl-tests DEPENDS ${ASTL_TESTS_LIST})
endif()

include(GNUInstallDirs)
install(EXPORT ${INSTALL_TARGET_EXPORT}
    NAMESPACE astl::
    DESTINATION ${INSTALL_CMAKE_DIR} ${CMAKE_INSTALL_LIBDIR}/cmake/astl
)
