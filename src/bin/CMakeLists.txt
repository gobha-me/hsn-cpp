add_executable(${PROJECT_NAME}
  main.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE argparse
  PRIVATE fmt::fmt-header-only
)
