project(unit_test_uart)

add_subdirectory(mock)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC inc)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/servo_controller.c)
