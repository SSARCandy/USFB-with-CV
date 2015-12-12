cmake_minimum_required(VERSION 3.0)

project( USFB )

find_package( OpenCV REQUIRED )


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )


include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable( USFB
"${PROJECT_SOURCE_DIR}/src/Main.cpp" 
"${PROJECT_SOURCE_DIR}/src/Enemy.cpp" 
"${PROJECT_SOURCE_DIR}/src/RGBpixmap.cpp" 
"${PROJECT_SOURCE_DIR}/src/Shield.cpp"  
"${PROJECT_SOURCE_DIR}/include/Bullet.h" 
"${PROJECT_SOURCE_DIR}/include/Enemy.h" 
"${PROJECT_SOURCE_DIR}/include/glut.h" 
"${PROJECT_SOURCE_DIR}/include/glut32.lib" 
"${PROJECT_SOURCE_DIR}/include/RGBpixmap.h" 
"${PROJECT_SOURCE_DIR}/include/Shield.h" 
)

target_link_libraries( USFB ${OpenCV_LIBS} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )