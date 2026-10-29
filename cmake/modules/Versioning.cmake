# ============================================================
# Versioning.cmake
#
# Version header and Windows resource generation utilities.
# ============================================================

# Directory containing this CMake module.
#
# CMAKE_CURRENT_FUNCTION_LIST_DIR is only available since CMake 3.17.
# Store CMAKE_CURRENT_LIST_DIR when this module is included so the
# functions below remain compatible with CMake 3.16.
set(_ADS_VERSIONING_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

# ------------------------------------------------------------
# Generates the ADS version header.
# ------------------------------------------------------------
function(generate_ads_version_header)
    if(NOT DEFINED QtADS_VERSION_MAJOR OR
       NOT DEFINED QtADS_VERSION_MINOR OR
       NOT DEFINED QtADS_VERSION_PATCH)
        message(FATAL_ERROR
            "ADS major, minor, or patch version variables are not defined."
        )
    endif()

    configure_file(
        "${_ADS_VERSIONING_MODULE_DIR}/ads_version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/ads_version.h"
        @ONLY
    )
endfunction()


# ------------------------------------------------------------
# Adds Windows version information resources to a target.
# ------------------------------------------------------------
function(add_windows_version_resources target)
    if(NOT WIN32)
        return()
    endif()

    if(NOT TARGET "${target}")
        message(FATAL_ERROR
            "add_windows_version_resources: Target '${target}' does not exist."
        )
    endif()

    set(_rc_in
        "${_ADS_VERSIONING_MODULE_DIR}/FileVersionInfo.rc.in"
    )

    if(NOT EXISTS "${_rc_in}")
        message(FATAL_ERROR
            "Windows version resource template not found: ${_rc_in}"
        )
    endif()

    set(_rc_out
        "${CMAKE_CURRENT_BINARY_DIR}/${target}_version.rc"
    )

    configure_file(
        "${_rc_in}"
        "${_rc_out}"
        @ONLY
    )

    target_sources(
        "${target}"
        PRIVATE
            "${_rc_out}"
    )
endfunction()
