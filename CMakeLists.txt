cmake_minimum_required(VERSION 3.16) # 指定 CMake 最低版本 3.16
project(myOpenGL) # 定義專案名稱

set(CMAKE_BUILD_TYPE "Release") # 建構類型 Release。編譯時會進行優化以提高性能。
set(CMAKE_CXX_FLAGS "-std=c++17 -O3") # C++ 17 標準、O3 最高級別編譯器優化

# 自訂的 CMake 模組位置
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakeModules)

find_package(OpenCV 3.0 REQUIRED) # 加載 OpenCV 函式庫
find_package(Threads REQUIRED) # 加載 pthread 函式庫
find_package(Eigen3 3.1.0 REQUIRED) # 加載 Eigen3 函式庫

#引用 OpenGL 函式庫
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

#引用 OpenGL 頭文件目錄
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${glm_INCLUDE_DIRS})

include_directories(${OpenCV_INCLUDE_DIRS}) # 加載 OpenCV 頭文件目錄
include_directories(${EIGEN3_INCLUDE_DIR}) # 加載 Eigen3 頭文件目錄

set( # 編譯器連結第三方函式庫
    THIRD_PARTY_LIBS ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT}
    ${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so
    ${PROJECT_SOURCE_DIR}/Thirdparty/g2o/lib/libg2o.so
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${glm_LIBRARIES}
)

# 生成的可執行文件檔將放置在 bin 資料夾
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 生成的庫文件將放置在 lib 資料夾
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 設定本專案之頭文件目錄位置
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR})

# 設定 CMake 處理該些目錄下的 CMakeLists.txt 文件
add_subdirectory(src)
add_subdirectory(test)