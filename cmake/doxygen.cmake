function(setup_doxygen)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXY_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc)
        configure_file(${CMAKE_SOURCE_DIR}/doxygen.conf.in ${DOXY_OUTPUT_DIR}/Doxyfile @ONLY)
        add_custom_target(doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_OUTPUT_DIR}/Doxyfile
            WORKING_DIRECTORY ${DOXY_OUTPUT_DIR}
            COMMENT "Generating API documentation with Doxygen"
        )
    else ()
        message(WARNING "No Doxygen executable found - documentation target unavailable.")
    endif ()
endfunction()
