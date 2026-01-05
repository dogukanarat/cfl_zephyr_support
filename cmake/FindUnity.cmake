find_package(PkgConfig)
pkg_check_modules(PKGUnity libunity)

find_path(Find_Unity_INCLUDE_DIRS unity.h
          PATHS ${Unity_INCLUDE_DIRS_HINT}
                ${PKGUnity_INCLUDE_DIRS}
                /usr/include
                /usr/local/include
                /usr/include/unity
                /usr/local/include/unity)

find_library(Find_Unity_LIBRARIES NAMES unity
             PATHS ${Unity_LIBRARIES_HINT}
                   ${PKGUnity_LIBRARY_DIRS}
                   /usr/lib
                   /usr/local/lib
                   /usr/lib/unity
                   /usr/local/lib/unity)

if(Find_Unity_LIBRARIES AND Find_Unity_INCLUDE_DIRS)
    set(Unity_FOUND ON)
    #message(STATUS "Found Unity")
    #message(STATUS "Unity libraries: ${Find_Unity_LIBRARIES}")
    #message(STATUS "Unity include directories: ${Find_Unity_INCLUDE_DIRS}")
endif()

if(NOT TARGET unity)
    add_library(unity UNKNOWN IMPORTED)
    set_target_properties(unity PROPERTIES
        IMPORTED_LOCATION ${Find_Unity_LIBRARIES}
        INTERFACE_INCLUDE_DIRECTORIES ${Find_Unity_INCLUDE_DIRS})
endif()
