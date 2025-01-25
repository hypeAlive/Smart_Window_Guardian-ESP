# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(requires "")
idf_build_get_property(target IDF_TARGET)

message(STATUS "Including project.cmake: ${IDF_PATH}/tools/cmake/project.cmake")
FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
idf_component_register(
        SRCS ${app_sources}
        INCLUDE_DIRS "."
        REQUIRES ${requires}
        EMBED_TXTFILES cert.pem)
target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
