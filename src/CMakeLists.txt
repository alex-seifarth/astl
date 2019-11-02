set(LIB_NAME astl)

set(SRCS
    ../library.cpp
)

add_library(${LIB_NAME} SHARED ${SRCS})

include(GenerateExportHeader)
generate_export_header(${LIB_NAME}
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/${LIB_NAME}/Export.h)

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include
    PRIVATE .
)

target_compile_options(${LIB_NAME}
    PRIVATE -Wall -Wextra -pedantic -Werror
)

target_compile_features(${LIB_NAME}
    PUBLIC cxx_std_17 cxx_auto_type cxx_constexpr cxx_decltype cxx_defaulted_move_initializers cxx_delegating_constructors
           cxx_digit_separators cxx_explicit_conversions cxx_final cxx_inheriting_constructors cxx_inline_namespaces
           cxx_lambdas cxx_noexcept cxx_nullptr cxx_override cxx_range_for cxx_right_angle_brackets cxx_rvalue_references
           cxx_sizeof_member cxx_static_assert cxx_strong_enums cxx_uniform_initialization cxx_variable_templates
           cxx_variadic_templates cxx_template_template_parameters
)

