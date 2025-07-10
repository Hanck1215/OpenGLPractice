# 建立一個共享庫（動態連結庫），名稱為 myOpenGL
add_library(
    myOpenGL SHARED
    niftiReader.cpp
    shaderReader.cpp
    cubeModel.cpp
    displayer.cpp
    niftiModel.cpp
)

# 將第三方庫連結到 myOpenGL 共享庫上，確保編譯和連結時能找到所需的外部依賴
target_link_libraries(myOpenGL ${THIRD_PARTY_LIBS})