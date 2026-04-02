add_library(shared STATIC
    protocol/messages.cpp
)

target_include_directories(shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shared PUBLIC sfml-network sfml-system)
