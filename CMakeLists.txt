cmake_minimum_required(VERSION 3.3)
set(ads_VERSION "2.3.2")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
project(QtAdvancedDockingSystem VERSION ${ads_VERSION})
option(BUILD_STATIC "Build the static library" OFF)
option(BUILD_EXAMPLES "Build the examples" ON)
set(REQUIRED_QT_VERSION 5.0.0)
find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Gui ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
message(STATUS "Found Qt ${Qt5Core_VERSION}")
set(ads_INCLUDE ${ads_INCLUDE} ${Qt5Core_INCLUDE_DIRS})
set(ads_INCLUDE ${ads_INCLUDE} "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(ads_LIBS ${ads_LIBS} ${Qt5Core_LIBRARIES})
set(ads_COMPILE_DEFINE ${ads_COMPILE_DEFINE} ${Qt5Core_COMPILE_DEFINITIONS})
set(ads_LIBS ${ads_LIBS} ${Qt5Gui_LIBRARIES})
set(ads_INCLUDE ${ads_INCLUDE} ${Qt5Gui_INCLUDE_DIRS})
set(ads_COMPILE_DEFINE ${ads_COMPILE_DEFINE} ${Qt5Gui_COMPILE_DEFINITIONS})
set(ads_LIBS ${ads_LIBS} ${Qt5Widgets_LIBRARIES})
set(ads_INCLUDE ${ads_INCLUDE} ${Qt5Widgets_INCLUDE_DIRS})
set(ads_COMPILE_DEFINE ${ads_COMPILE_DEFINE} ${Qt5Widgets_COMPILE_DEFINITIONS})
if(BUILD_STATIC)
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()
set(ads_SRCS
    src/ads_globals.cpp
    src/DockAreaTabBar.cpp
    src/DockAreaTitleBar.cpp
    src/DockAreaWidget.cpp
    src/DockContainerWidget.cpp
    src/DockManager.cpp
    src/DockOverlay.cpp
    src/DockSplitter.cpp
    src/DockWidget.cpp
    src/DockWidgetTab.cpp
    src/ElidingLabel.cpp
    src/FloatingDockContainer.cpp
    src/ads.qrc
)
set(ads_INSTALL_INCLUDE 
    src/ads_globals.h
    src/DockAreaTabBar.h
    src/DockAreaTitleBar.h
    src/DockAreaWidget.h
    src/DockContainerWidget.h
    src/DockManager.h
    src/DockOverlay.h
    src/DockSplitter.h
    src/DockWidget.h
    src/DockWidgetTab.h
    src/ElidingLabel.h
    src/FloatingDockContainer.h
)
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(ads_PlatformDir "x64")
else()
    set(ads_PlatformDir "x86")
endif()
if(BUILD_STATIC)
    add_library(ads STATIC ${ads_SRCS})
    set(ads_COMPILE_DEFINE ${ads_COMPILE_DEFINE} ADS_STATIC)
else()
    add_library(ads SHARED ${ads_SRCS})
    set(ads_COMPILE_DEFINE ${ads_COMPILE_DEFINE} ADS_SHARED_EXPORT)
endif()
install(FILES ${ads_INSTALL_INCLUDE}
    DESTINATION include 
    COMPONENT headers
)
install(FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/gnu-lgpl-v2.1.md"
    DESTINATION license 
    COMPONENT license
)
install(TARGETS ads
    EXPORT adsBinary
    RUNTIME DESTINATION bin COMPONENT library
    LIBRARY DESTINATION lib COMPONENT library
    ARCHIVE DESTINATION lib COMPONENT library
)
target_include_directories(ads PUBLIC 
    $<BUILD_INTERFACE:${ads_INCLUDE}>
    $<INSTALL_INTERFACE:include>
    )
target_link_libraries(ads PUBLIC ${ads_LIBS})
target_compile_definitions(ads PRIVATE ${ads_COMPILE_DEFINE})
set_target_properties(ads PROPERTIES 
    VERSION ${ads_VERSION}
    EXPORT_NAME "Qt Advanced Docking System"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
if(BUILD_EXAMPLES)
    add_subdirectory(example)
    add_subdirectory(demo)
endif()

