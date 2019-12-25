set(INTF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS
    include/astl/event.h
    include/astl/signal.h
    include/astl/slot_holder.h
    include/astl/recursive_event.h
    include/astl/final.h
)

add_library(core INTERFACE)

target_include_directories(core
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(core
    INTERFACE cxx_std_17 cxx_auto_type cxx_constexpr cxx_decltype cxx_defaulted_move_initializers cxx_delegating_constructors
        cxx_digit_separators cxx_explicit_conversions cxx_final cxx_inheriting_constructors cxx_inline_namespaces
        cxx_lambdas cxx_noexcept cxx_nullptr cxx_override cxx_range_for cxx_right_angle_brackets cxx_rvalue_references
        cxx_sizeof_member cxx_static_assert cxx_strong_enums cxx_uniform_initialization cxx_variable_templates
        cxx_variadic_templates cxx_template_template_parameters
)

target_compile_options(core
    INTERFACE -Wall -Wextra -pedantic -Werror
)

include(GNUInstallDirs)
install(TARGETS core
    EXPORT astl-exports
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    RENAME astlCore
)

install(FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astl
)

if (ASTL_GTESTS)
    add_subdirectory(gtest)
endif()


