cmake_minimum_required(VERSION 3.27)
project(yellowstone)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_custom_target(compile_shaders
        COMMAND ${CMAKE_SOURCE_DIR}/src/shaders/compile.bat
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/shaders
        COMMENT "Compiling shaders using compile.bat"
    )
else()
    add_custom_target(compile_shaders
        COMMAND chmod +x ${CMAKE_SOURCE_DIR}/src/shaders/compile.sh
        COMMAND ${CMAKE_SOURCE_DIR}/src/shaders/compile.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/shaders
        COMMENT "Compiling shaders using compile.sh"
    )
endif()

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
)
add_executable(yellowstone ${PROJECT_SOURCES})

add_dependencies(yellowstone compile_shaders)

target_include_directories(yellowstone PRIVATE "${CMAKE_SOURCE_DIR}/src")

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

if(TARGET glm::glm)
	target_link_libraries(yellowstone PRIVATE glm::glm)
endif()

if(TARGET glfw)
	target_link_libraries(yellowstone PRIVATE glfw)
else()
	if(TARGET GLFW::GLFW)
		target_link_libraries(yellowstone PRIVATE GLFW::GLFW)
	endif()
endif()

if(TARGET Vulkan::Vulkan)
	target_link_libraries(yellowstone PRIVATE Vulkan::Vulkan)
endif()