# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置

cmake_minimum_required (VERSION 3.8)

project ("Pie Dreams and Hog Schemes")

#设置c++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#设置源文件目录
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

#设置sfml库位置
set(SFML_DIR ${CMAKE_SOURCE_DIR}/to/SFML-3.0.0/lib/cmake/SFML)
#寻找sfml
find_package(SFML 3.0.0 REQUIRED COMPONENTS Audio System )

add_executable(main ${SRC})

#添加头文件路径
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

#添加easyx库文件路径
target_link_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/lib)

#链接easyx库
target_link_libraries(main PRIVATE EasyXa)
#链接windows gdi
target_link_libraries(main PRIVATE Msimg32)

#链接sfml库
target_link_libraries(main PRIVATE SFML::Audio SFML::System)

#target_compile_definitions(main PRIVATE UNICODE _UNICODE)
   
