cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    AdvancedDockingSystem
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)
include(cmake/link_qt.cmake)

set(BUILD_SHARED_LIBS ON)

# ---- Declare library ----

add_library(
    AdvancedDockingSystem
    include/AdvancedDockingSystem/
    include/AdvancedDockingSystem/AdvancedDockingSystem.hpp
    include/AdvancedDockingSystem/API.h
    include/AdvancedDockingSystem/ContainerWidget.h
    include/AdvancedDockingSystem/DropOverlay.h
    include/AdvancedDockingSystem/FloatingWidget.h
    include/AdvancedDockingSystem/Internal.h
    include/AdvancedDockingSystem/SectionContent.h
    include/AdvancedDockingSystem/SectionContentWidget.h
    include/AdvancedDockingSystem/SectionTitleWidget.h
    include/AdvancedDockingSystem/SectionWidget.h
    include/AdvancedDockingSystem/Serialization.h
    source/AdvancedDockingSystem.cpp
    source/API.cpp
    source/ContainerWidget.cpp
    source/DropOverlay.cpp
    source/FloatingWidget.cpp
    source/Internal.cpp
    source/SectionContent.cpp
    source/SectionContentWidget.cpp
    source/SectionTitleWidget.cpp
    source/SectionWidget.cpp
    source/Serialization.cpp
)
add_library(AdvancedDockingSystem::AdvancedDockingSystem ALIAS AdvancedDockingSystem)

target_link_libraries(AdvancedDockingSystem ${QT_LIBRARIES})

include(GenerateExportHeader)
generate_export_header(
    AdvancedDockingSystem
    BASE_NAME AdvancedDockingSystem
    EXPORT_FILE_NAME export/AdvancedDockingSystem/AdvancedDockingSystem_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(AdvancedDockingSystem PUBLIC ADVANCEDDOCKINGSYSTEM_STATIC_DEFINE)
endif()

set_target_properties(
    AdvancedDockingSystem PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME AdvancedDockingSystem
    OUTPUT_NAME AdvancedDockingSystem
)

target_include_directories(
    AdvancedDockingSystem ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    AdvancedDockingSystem SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(AdvancedDockingSystem PUBLIC cxx_std_17)

target_compile_definitions(
    AdvancedDockingSystem PUBLIC
    ADS_NAMESPACE_ENABLED=TRUE
    ADS_EXPORT=TRUE
)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----
add_subdirectory(example)

add_subdirectory(test)

