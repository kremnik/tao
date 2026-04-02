add_executable(arkanoid_server
    main.cpp
    server.cpp
    match.cpp
    leaderboard.cpp
)

target_include_directories(arkanoid_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(arkanoid_server PRIVATE shared game_logic sfml-network sfml-system)
