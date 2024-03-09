  message(STATUS "Configuring test setup ${TEST_NAME}")

  add_executable(${TEST_NAME}
    ${SRCS}
  )

  target_include_directories(${TEST_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_link_libraries(${TEST_NAME}
    PRIVATE Catch2::Catch2WithMain
  )
