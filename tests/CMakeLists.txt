add_executable(test_protocol   test_protocol.cpp)
target_link_libraries(test_protocol PRIVATE shared)
add_test(NAME test_protocol COMMAND test_protocol)

add_executable(test_game_state test_game_state.cpp)
target_link_libraries(test_game_state PRIVATE game_logic)
add_test(NAME test_game_state COMMAND test_game_state)
