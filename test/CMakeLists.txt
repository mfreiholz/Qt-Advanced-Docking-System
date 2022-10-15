cmake_minimum_required(VERSION 3.14)

project(AdvancedDockingSystemTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)
# Link Qt
set(QT_LIBRARIES_NAMES
  Core
  Gui
  Widgets
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_LIBRARIES_NAMES} )
find_package(Qt5Test REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_LIBRARIES_NAMES})
include_directories(%{Qt:QT_INSTALL_PREFIX})

list(APPEND QT_LIBRARIES
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::Widgets
)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(AdvancedDockingSystem REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(
  AdvancedDockingSystem_test
  source/main.cpp
  source/TestCore.cpp
  source/TestCore.h
)
target_link_libraries(AdvancedDockingSystem_test PRIVATE AdvancedDockingSystem::AdvancedDockingSystem  ${QT_LIBRARIES} Qt5::Test)
target_compile_features(AdvancedDockingSystem_test PRIVATE cxx_std_17)

add_test(NAME AdvancedDockingSystem_test COMMAND AdvancedDockingSystem_test)
windows_set_path(AdvancedDockingSystem_test AdvancedDockingSystem::AdvancedDockingSystem)

# ---- End-of-file commands ----

add_folders(Test)
