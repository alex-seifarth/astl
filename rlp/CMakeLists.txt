include(rlp_platform_checks.cmake)

rlp_setup_mechanism()
string(TOLOWER ${ASTL_RLP_AIO_MECHANISM} mechanism_dir)

set(COMPONENT rlp)

set(SRCS
    arch/${mechanism_dir}/runloop_impl.cpp
    src/runloop.cpp
)

add_library(${COMPONENT} SHARED "${SRCS}")

target_link_libraries(${COMPONENT}
    PUBLIC core
)

target_include_directories(${COMPONENT}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        arch/${mechanism_dir}
)

target_compile_features(${COMPONENT}
    INTERFACE ${ASTL_INTERFACE_COMPILE_FEATURES}
)

target_compile_options(${COMPONENT}
    INTERFACE ${ASTL_INTERFACE_COMPILE_OPTIONS}
)

include(GenerateExportHeader)
generate_export_header(${COMPONENT}
    BASE_NAME ASTL_RLP
    EXPORT_FILE_NAME astl/rlp_export.h
)


include(GNUInstallDirs)
install(TARGETS ${COMPONENT}
        EXPORT ${INSTALL_TARGET_EXPORT}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astl
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ./include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astl-v${PROJECT_VERSION_MAJOR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/astl/rlp_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astl-v${PROJECT_VERSION_MAJOR}/astl)

if (ASTL_GTESTS)
    add_subdirectory(gtest)
endif()

