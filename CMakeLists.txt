cmake_minimum_required(VERSION 3.16)

project(QTEdgeWebView VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

find_library(
  WebView2LibraryLib #变量名，用于存储库的完整路径
  NAMES WebView2Library # DLL文件的名称，不含"lib"前缀和".dll"后缀
  PATHS
  "./third_party/WebView2Library/lib" #库文件所在的路径
  )

# 如果找不到库，你可以给出警告或错误
if(NOT WebView2LibraryLib)
  message(FATAL_ERROR "无法找到第三方库")
endif()

find_path(
  WebView2LibraryInclude #变量名，用于存储头文件路径
  NAMES EdgeWebView2.h #头文件的名称
  PATHS
  "./third_party/WebView2Library/include" #头文件所在的路径
)

if(NOT WebView2LibraryInclude)
  message(FATAL_ERROR "无法找到第三方库的头文件")
endif()

qt_add_executable(appQTEdgeWebView
    main.cpp
)

qt_add_qml_module(appQTEdgeWebView
    URI QTEdgeWebView
    VERSION 1.0
    QML_FILES main.qml
    SOURCES
    SOURCES qmlwebview2.h qmlwebview2.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appQTEdgeWebView PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appQTEdgeWebView
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appQTEdgeWebView
    PRIVATE Qt6::Quick)
target_link_libraries(appQTEdgeWebView PRIVATE Qt6::Core
    shell32.lib
    advapi32.lib
    user32.lib
    ole32.lib)
target_link_libraries(appQTEdgeWebView PRIVATE Qt6::Core Qt6::Quick)

# 将头文件路径添加到你的目标
target_include_directories(
  appQTEdgeWebView #你的目标名，例如你的可执行文件
  PUBLIC
  ${WebView2LibraryInclude}
)


# 将库文件链接到你的目标
target_link_libraries(
  appQTEdgeWebView
  PUBLIC
  ${WebView2LibraryLib}
)

include(GNUInstallDirs)
install(TARGETS appQTEdgeWebView
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
