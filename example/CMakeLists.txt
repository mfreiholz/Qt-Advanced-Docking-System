cmake_minimum_required(VERSION 3.14)

project(AdvancedDockingSystemExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/link_qt.cmake)


if(PROJECT_IS_TOP_LEVEL)
  find_package(AdvancedDockingSystem REQUIRED)
endif()

add_custom_target(run-examples)

add_executable(
  demo
  icontitlewidget.cpp
  icontitlewidget.h
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  dialogs/SectionContentListModel.cpp
  dialogs/SectionContentListModel.h
  dialogs/SectionContentListWidget.cpp
  dialogs/SectionContentListWidget.h
  dialogs/SectionContentListWidget.ui
)
target_link_libraries(demo PRIVATE AdvancedDockingSystem::AdvancedDockingSystem ${QT_LIBRARIES} )
target_compile_features(demo PRIVATE cxx_std_17)