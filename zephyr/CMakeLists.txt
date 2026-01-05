if(CONFIG_CFL_SUPPORT)
    zephyr_library()

    zephyr_library_sources(
        ../src/cfl_log.c
        ../src/cfl_utilities.c
        ../src/services/cfl_service_danp.c # TODO check config for this file
    )

    zephyr_library_sources_ifdef(CONFIG_SHELL
        ../src/cfl_shell.c
    )

    zephyr_include_directories(
        ../include
        ../src
    )
endif()
