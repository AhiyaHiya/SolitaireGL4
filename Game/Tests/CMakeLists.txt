add_executable(cards_tests
    cards_tests.cpp
    "${game_base_directory}/Game/cards.cpp"
)

#gtest_add_tests(TARGET cards_tests)

target_include_directories(cards_tests
    PRIVATE
        "${game_base_directory}/Game"
)

target_link_libraries(cards_tests
    PRIVATE
        glad
        gtest
        gtest_main
        stb

        nlohmann_json::nlohmann_json
)

target_link_options(cards_tests
    PRIVATE
        "-Wl,-rpath,/usr/local/gcc-15/lib64"
)
