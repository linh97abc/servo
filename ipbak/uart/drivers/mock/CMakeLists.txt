add_executable (${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/uart_fd_mock.c)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sys_init.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/fifo_uart_mock.cpp)