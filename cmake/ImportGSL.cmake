
# Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
# Copyright (c) 2023 by Brightskies inc,
# All rights reserved.
# ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).

# @file ImportGSL.cmake
# @version 1.0.0
# @author Sameh Abdulah
# @date 2023-03-16


message("")
message("---------------------------------------- GSL")
message(STATUS "Checking for GSL")
include(macros/BuildGSL)

if (NOT TARGET GSL_FOUND)
    include(FindPkgConfig)
    find_package(PkgConfig QUIET)
    find_package(GSL QUIET)

    if (GSL_FOUND)
        message("   Found GSL: ${GSL_LIBDIR}")
    else ()
        message("   Can't find GSL, Installing it instead ..")
        BuildGSL(GSL "https://gitlab.inria.fr/solverstack/chameleon.git" "v1.1.0")
        find_package(GSL REQUIRED)
    endif ()
else()
    message("   GSL already included")
endif()

list(APPEND LIBS  ${GSL_LIBRARIES})

message(STATUS "GSL done")