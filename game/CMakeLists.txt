add_library(game_logic STATIC
    game_state.cpp
    physics.cpp
)

target_include_directories(game_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(game_logic PUBLIC shared)
