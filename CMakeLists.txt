cmake_minimum_required(VERSION 4.1.2)

project(BingKits VERSION 1.0 LANGUAGES C CXX)

add_subdirectory(third_party)

set(CMAKE_DEBUG_POSTFIX d)

foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}
        ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}
        ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}
        ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
endforeach()

if(MSVC)
    add_compile_options(/utf-8)
    add_definitions(-DUNICODE -D_UNICODE)  # 定义 UNICODE 宏
endif()


# 只允许使用 C++11 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    main.cc
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                           include)

target_link_libraries(${PROJECT_NAME} PRIVATE )

if(ENABLE_SQLITE)
    target_link_libraries(${PROJECT_NAME} PRIVATE sqlite)
endif()

add_custom_target(run
    COMMAND ${PROJECT_NAME} $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Running ${PROJECT_NAME}"
    VERBATIM
)
