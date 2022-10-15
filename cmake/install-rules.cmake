if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/AdvancedDockingSystem CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package AdvancedDockingSystem)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT AdvancedDockingSystem_Development
)

install(
    TARGETS AdvancedDockingSystem
    EXPORT AdvancedDockingSystemTargets
    RUNTIME #
    COMPONENT AdvancedDockingSystem_Runtime
    LIBRARY #
    COMPONENT AdvancedDockingSystem_Runtime
    NAMELINK_COMPONENT AdvancedDockingSystem_Development
    ARCHIVE #
    COMPONENT AdvancedDockingSystem_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    AdvancedDockingSystem_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(AdvancedDockingSystem_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${AdvancedDockingSystem_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT AdvancedDockingSystem_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${AdvancedDockingSystem_INSTALL_CMAKEDIR}"
    COMPONENT AdvancedDockingSystem_Development
)

install(
    EXPORT AdvancedDockingSystemTargets
    NAMESPACE AdvancedDockingSystem::
    DESTINATION "${AdvancedDockingSystem_INSTALL_CMAKEDIR}"
    COMPONENT AdvancedDockingSystem_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
