add_executable(arkanoid_client
    main.cpp
    client_network.cpp
    renderer.cpp
)

target_include_directories(arkanoid_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(arkanoid_client PRIVATE
    shared game_logic
    sfml-graphics sfml-window sfml-network sfml-system)
