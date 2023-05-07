cmake_minimum_required(VERSION 3.14)

project(valsim LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package( OpenCV REQUIRED )

add_executable(valsim
  main.cpp
  remoteobjectdetector.cpp
  remoteobjectdetector.h
  objectdetectorinterface.cpp
  objectdetectorinterface.h
  valsim.cpp
  worker.cpp
)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(valsim ${OpenCV_LIBS})

target_include_directories(valsim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/bpf_tools/rpclib/include)
target_link_directories(valsim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/commonpp/build
    ${CMAKE_CURRENT_SOURCE_DIR}/bpf_tools/build
    ${CMAKE_CURRENT_SOURCE_DIR}/bpf_tools/rpclib/build
)

target_link_libraries(valsim Qt${QT_VERSION_MAJOR}::Core distlib rpc commonpp)

install(TARGETS valsim
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
