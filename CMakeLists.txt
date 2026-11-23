cmake_minimum_required(VERSION 3.24)
project("Game-Demo" LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_subdirectory("Engine")

set(GAME_RULES_SOURCES
        "Source/Rules/BeatClock.cpp"
        "Source/Rules/BeatClock.h"
        "Source/Rules/LaneSelector.cpp"
        "Source/Rules/LaneSelector.h"
        "Source/Rules/Melody.cpp"
        "Source/Rules/Melody.h"
        "Source/Rules/ScoreBoard.cpp"
        "Source/Rules/ScoreBoard.h"
        "Source/Rules/ScoreRecords.cpp"
        "Source/Rules/ScoreRecords.h"
)

add_executable(${PROJECT_NAME}
        ${GAME_RULES_SOURCES}
        "Source/Actors/ActorRegistry.cpp"
        "Source/Actors/ActorRegistry.h"
        "Source/Actors/EffectActor.cpp"
        "Source/Actors/EffectActor.h"
        "Source/Actors/GameActor.h"
        "Source/Actors/ModelActor.h"
        "Source/Actors/ObstacleActor.cpp"
        "Source/Actors/ObstacleActor.h"
        "Source/Actors/RunnerActor.cpp"
        "Source/Actors/RunnerActor.h"
        "Source/Actors/ScrollActor.h"
        "Source/Actors/SpeakerActor.cpp"
        "Source/Actors/SpeakerActor.h"
        "Source/Actors/TargetActor.cpp"
        "Source/Actors/TargetActor.h"
        "Source/Cameras/RigCamera.cpp"
        "Source/Cameras/RigCamera.h"
        "Source/Effects/BurstEffect.cpp"
        "Source/Effects/BurstEffect.h"
        "Source/Effects/EmberField.cpp"
        "Source/Effects/EmberField.h"
        "Source/Effects/GlowEffect.cpp"
        "Source/Effects/GlowEffect.h"
        "Source/Effects/StreamEffect.cpp"
        "Source/Effects/StreamEffect.h"
        "Source/Scenes/RunScene.cpp"
        "Source/Scenes/RunScene.h"
        "Source/Scenes/SplashScene.cpp"
        "Source/Scenes/SplashScene.h"
        "Source/Screens/HudScreen.cpp"
        "Source/Screens/HudScreen.h"
        "Source/Screens/MenuScreen.cpp"
        "Source/Screens/MenuScreen.h"
        "Source/Screens/PauseScreen.cpp"
        "Source/Screens/PauseScreen.h"
        "Source/Screens/ScoresScreen.cpp"
        "Source/Screens/ScoresScreen.h"
        "Source/Screens/SplashScreen.cpp"
        "Source/Screens/SplashScreen.h"
        "Source/Screens/UiKit.cpp"
        "Source/Screens/UiKit.h"
        "Source/Sound/Timbres.h"
        "Source/Visuals/Catalog.h"
        "Source/Visuals/City.cpp"
        "Source/Visuals/City.h"
        "Source/Visuals/Palette.h"
        "Source/Visuals/ScreenFx.cpp"
        "Source/Visuals/ScreenFx.h"
        "Source/Config.h"
        "Source/NightmareGame.cpp"
        "Source/NightmareGame.h"
        "Source/Main.cpp"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE "Source")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
)

target_link_libraries(${PROJECT_NAME} PRIVATE "SpinoCore::Runtime")

spinocore_add_assets(${PROJECT_NAME})

if(TARGET GTest::gtest_main)
    add_executable(Game-Tests
            ${GAME_RULES_SOURCES}
            "Tests/BeatClockTests.cpp"
            "Tests/LaneSelectorTests.cpp"
            "Tests/MelodyTests.cpp"
            "Tests/ScoreBoardTests.cpp"
            "Tests/ScoreRecordsTests.cpp"
    )
    target_compile_features(Game-Tests PRIVATE cxx_std_20)
    target_include_directories(Game-Tests PRIVATE "Source")
    set_target_properties(Game-Tests PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_options(Game-Tests PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>)
    target_link_libraries(Game-Tests PRIVATE GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(Game-Tests)
endif()
