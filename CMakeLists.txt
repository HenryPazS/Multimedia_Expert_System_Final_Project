cmake_minimum_required(VERSION 3.16)

project(ChildDistractionSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)

# Support both older and newer CMake SQLite target names.
if(NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 ALIAS SQLite::SQLite3)
endif()

add_library(media_database
    database/MediaDatabase.cpp
    database/MediaDescription.cpp
)

target_include_directories(media_database
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/database"
)

target_link_libraries(media_database
    PUBLIC
        SQLite3::SQLite3
        tinyxml2::tinyxml2
)

add_library(inference_engine
    backend/PatientProfile.cpp
    backend/InferenceEngine.cpp
)

target_include_directories(inference_engine
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/backend"
)

include(CTest)

if(BUILD_TESTING)
    add_executable(test_database
        database/test_database.cpp
    )
    target_link_libraries(test_database PRIVATE media_database)

    add_executable(test_description
        database/test_description.cpp
    )
    target_link_libraries(test_description PRIVATE media_database)

    add_executable(test_integration
        database/test_integration.cpp
    )
    target_link_libraries(test_integration
        PRIVATE media_database inference_engine
    )

    add_test(NAME database_test COMMAND test_database)
    add_test(NAME description_test COMMAND test_description)
    add_test(NAME integration_test COMMAND test_integration)

    # Database and media paths are relative to the repository root.
    set_tests_properties(
        database_test description_test integration_test
        PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()