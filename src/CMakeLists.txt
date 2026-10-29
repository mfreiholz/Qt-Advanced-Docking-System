cmake_minimum_required(VERSION 3.16)

project(
    QtAdvancedDockingSystem
    VERSION ${QtADS_VERSION}
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# CMake modules
# ---------------------------------------------------------------------------

# Provides standard installation directory variables such as
# CMAKE_INSTALL_BINDIR, CMAKE_INSTALL_LIBDIR, and CMAKE_INSTALL_INCLUDEDIR.
include(GNUInstallDirs)

# Provides ADS-specific versioning functions, including generation of the
# version header and Windows version resources.
include(Versioning)

# ---------------------------------------------------------------------------
# Qt dependencies
# ---------------------------------------------------------------------------

# If QT_VERSION_MAJOR has already been specified by the parent project,
# restrict Qt discovery to that major version. Otherwise, prefer Qt 6 and
# fall back to Qt 5.
if(DEFINED QT_VERSION_MAJOR AND QT_VERSION_MAJOR)
    find_package(
        QT
        NAMES Qt${QT_VERSION_MAJOR}
        COMPONENTS Core
        REQUIRED
    )
else()
    find_package(
        QT
        NAMES Qt6 Qt5
        COMPONENTS Core
        REQUIRED
    )
endif()

message(STATUS "ADS: Solution configured to use Qt version ${QT_VERSION_MAJOR}")

# Qt modules required by ADS.
set(ads_DEP_LIBS
    Core
    Gui
    Widgets
)

# Qt 6.9+ provides the required private GUI headers through the GuiPrivate
# component. Request it explicitly on supported Unix platforms.
if(UNIX AND NOT APPLE
   AND QT_VERSION_MAJOR STREQUAL "6"
   AND QT_VERSION_MINOR GREATER_EQUAL 9)
    list(APPEND ads_DEP_LIBS GuiPrivate)
endif()

find_package(
    Qt${QT_VERSION_MAJOR}
    COMPONENTS ${ads_DEP_LIBS}
    REQUIRED
)

# ---------------------------------------------------------------------------
# Version information
# ---------------------------------------------------------------------------

# Generate the ADS version header from the current project version.
generate_ads_version_header()

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------

set(ads_SRCS
    ads_globals.cpp
    DockAreaTabBar.cpp
    DockAreaTitleBar.cpp
    DockAreaWidget.cpp
    DockContainerWidget.cpp
    DockManager.cpp
    DockOverlay.cpp
    DockSplitter.cpp
    DockWidget.cpp
    DockWidgetTab.cpp
    DockingStateReader.cpp
    DockFocusController.cpp
    ElidingLabel.cpp
    FloatingDockContainer.cpp
    FloatingDragPreview.cpp
    IconProvider.cpp
    DockComponentsFactory.cpp
    AutoHideSideBar.cpp
    AutoHideTab.cpp
    AutoHideDockContainer.cpp
    PushButton.cpp
    ResizeHandle.cpp
    ads.qrc
)

set(ads_HEADERS
    ads_globals.h
    DockAreaTabBar.h
    DockAreaTitleBar.h
    DockAreaTitleBar_p.h
    DockAreaWidget.h
    DockContainerWidget.h
    DockManager.h
    DockOverlay.h
    DockSplitter.h
    DockWidget.h
    DockWidgetTab.h
    DockingStateReader.h
    DockFocusController.h
    ElidingLabel.h
    FloatingDockContainer.h
    FloatingDragPreview.h
    IconProvider.h
    DockComponentsFactory.h
    AutoHideSideBar.h
    AutoHideTab.h
    AutoHideDockContainer.h
    PushButton.h
    ResizeHandle.h
)

# Linux and other Unix platforms use a custom floating-window title bar.
if(UNIX AND NOT APPLE)
    list(PREPEND ads_SRCS
        linux/FloatingWidgetTitleBar.cpp
    )

    list(PREPEND ads_HEADERS
        linux/FloatingWidgetTitleBar.h
    )
endif()

# ---------------------------------------------------------------------------
# Library target
# ---------------------------------------------------------------------------

# Include the Qt major version in the library name so Qt 5 and Qt 6 builds
# can coexist on the same system.
set(library_name_base "qtadvanceddocking")
set(library_name "${library_name_base}-qt${QT_VERSION_MAJOR}")

if(BUILD_STATIC)
    add_library(
        ${library_name}
        STATIC
        ${ads_SRCS}
        ${ads_HEADERS}
    )

    # ADS_STATIC is part of the public usage requirements because consumers
    # need to know that ADS is linked statically.
    target_compile_definitions(
        ${library_name}
        PUBLIC ADS_STATIC
    )

    # Keep static and shared library filenames distinguishable.
    set_target_properties(
        ${library_name}
        PROPERTIES OUTPUT_NAME "${library_name}_static"
    )
else()
    add_library(
        ${library_name}
        SHARED
        ${ads_SRCS}
        ${ads_HEADERS}
    )

    # ADS_SHARED_EXPORT enables symbol export while building the shared
    # library itself. Consumers do not need this definition.
    target_compile_definitions(
        ${library_name}
        PRIVATE ADS_SHARED_EXPORT
    )

    # Add Windows version information to the DLL.
    if(WIN32)
        add_windows_version_resources(${library_name})
    endif()
endif()

# Provide namespaced aliases for use inside the build tree.
#
# ads::qtadvanceddocking-qt5 / ads::qtadvanceddocking-qt6 provide the
# version-specific target, while ads::qtadvanceddocking provides a common
# version-independent target name.
add_library(ads::${library_name} ALIAS ${library_name})
add_library(ads::${library_name_base} ALIAS ${library_name})

# ---------------------------------------------------------------------------
# Compiler configuration
# ---------------------------------------------------------------------------

# Tell MSVC that source and execution character sets use UTF-8.
target_compile_options(
    ${library_name}
    PRIVATE
        "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
)

# Qt 5 builds require C++14, while Qt 6 builds require C++17.
if(QT_VERSION_MAJOR STREQUAL "5")
    set_target_properties(
        ${library_name}
        PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED ON
    )
elseif(QT_VERSION_MAJOR STREQUAL "6")
    set_target_properties(
        ${library_name}
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
endif()

# Disable compiler-specific language extensions such as GNU C++ extensions.
set_target_properties(
    ${library_name}
    PROPERTIES
        CXX_EXTENSIONS OFF
)

# ---------------------------------------------------------------------------
# Include directories
# ---------------------------------------------------------------------------

target_include_directories(
    ${library_name}
    PUBLIC
        # Public headers when ADS is used directly from the build tree.
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>

        # Generated headers when ADS is used directly from the build tree.
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>

        # Public headers after installation.
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${library_name}>
)

# ---------------------------------------------------------------------------
# Qt private GUI headers
#
# The Linux implementation requires access to private Qt GUI headers for
# Qt 5 and Qt 6.9+. Keep these include directories private because they are
# an implementation detail of QtAdvancedDockingSystem and must not become
# part of its public interface.
# ---------------------------------------------------------------------------
if(UNIX AND NOT APPLE
   AND (QT_VERSION_MAJOR STREQUAL "5"
        OR (QT_VERSION_MAJOR STREQUAL "6"
            AND QT_VERSION_MINOR GREATER_EQUAL 9)))
    target_include_directories(${library_name}
        PRIVATE
            ${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS}
    )
endif()

# ---------------------------------------------------------------------------
# Qt libraries
# ---------------------------------------------------------------------------

target_link_libraries(
    ${library_name}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
)

# ADS uses private Qt GUI APIs on Unix systems other than macOS,
# specifically qpa/qplatformnativeinterface.h.
if(UNIX AND NOT APPLE AND QT_VERSION_MAJOR STREQUAL "6")
    target_link_libraries(
        ${library_name}
        PRIVATE
            Qt6::GuiPrivate
    )
endif()

# ---------------------------------------------------------------------------
# Platform-specific dependencies
# ---------------------------------------------------------------------------

if(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        find_package(X11 REQUIRED)

        target_link_libraries(
            ${library_name}
            PUBLIC X11::xcb
        )
    else()
        target_link_libraries(
            ${library_name}
            PUBLIC xcb
        )
    endif()
endif()

# ---------------------------------------------------------------------------
# Qt code generation and output configuration
# ---------------------------------------------------------------------------

set_target_properties(
    ${library_name}
    PROPERTIES
        # Process QObject-based classes automatically with moc.
        AUTOMOC ON

        # Compile Qt resource files automatically.
        AUTORCC ON

        # Shared-library ABI version information.
        VERSION ${QtADS_VERSION}
        SOVERSION ${QtADS_VERSION_MAJOR}

        # Name used when this target is exported.
        EXPORT_NAME ${library_name}

        # Append "d" to Debug library filenames.
        DEBUG_POSTFIX "d"

        # Keep generated binaries separated by platform.
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)

# ---------------------------------------------------------------------------
# Package configuration
# ---------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

# Generate a package version file used by find_package().
#
# Versions with the same major version are considered compatible.
write_basic_package_version_file(
    "${library_name}ConfigVersion.cmake"
    VERSION ${QtADS_VERSION}
    COMPATIBILITY SameMajorVersion
)

# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------

# Install public ADS headers.
install(
    FILES
        ${ads_HEADERS}
        "${CMAKE_CURRENT_BINARY_DIR}/ads_version.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${library_name}
    COMPONENT headers
)

# Install license files.
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
        "${CMAKE_CURRENT_SOURCE_DIR}/../gnu-lgpl-v2.1.md"
    DESTINATION share/ads/license
    COMPONENT license
)

# Install the ADS library and add it to the exported target set.
install(
    TARGETS ${library_name}
    EXPORT adsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Export the installed target as ads::<target>.
install(
    EXPORT adsTargets
    FILE adsTargets.cmake
    NAMESPACE ads::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}
)

# Install the package configuration file using the Qt-major-specific
# package name.
install(
    FILES qtadvanceddockingConfig.cmake
    RENAME ${library_name}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}
)

# Install the generated package version file.
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${library_name}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}
)