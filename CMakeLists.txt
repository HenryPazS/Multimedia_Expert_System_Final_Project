cmake_minimum_required(VERSION 3.16)

project(ChildDistractionSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Use installed SQLite / TinyXML-2 when present (Mac, Linux); otherwise download
# and build them (handy on Windows).
find_package(SQLite3 QUIET)
if(TARGET SQLite::SQLite3 AND NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 ALIAS SQLite::SQLite3)  # older CMake target name
endif()
if(NOT TARGET SQLite3::SQLite3)
    FetchContent_Declare(sqlite_src
        URL https://www.sqlite.org/2025/sqlite-amalgamation-3490100.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(sqlite_src)
    add_library(sqlite3_local STATIC ${sqlite_src_SOURCE_DIR}/sqlite3.c)
    target_include_directories(sqlite3_local PUBLIC ${sqlite_src_SOURCE_DIR})
    add_library(SQLite3::SQLite3 ALIAS sqlite3_local)
endif()

find_package(tinyxml2 CONFIG QUIET)
if(NOT TARGET tinyxml2::tinyxml2)
    FetchContent_Declare(tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG 11.0.0)
    set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(tinyxml2)
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

add_library(consultation_session
    backend/ConsultationSession.cpp
)

target_link_libraries(consultation_session
    PUBLIC inference_engine media_database
)

# HTTP server that connects the frontend to the backend.
find_package(Threads REQUIRED)

# Header-only libraries, downloaded at configure time (needs internet once).
FetchContent_Declare(httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.67.0)
FetchContent_Declare(nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(httplib nlohmann_json)

add_executable(server backend/server.cpp)
target_include_directories(server PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/backend")
target_link_libraries(server PRIVATE
    consultation_session Threads::Threads
    httplib::httplib nlohmann_json::nlohmann_json)
if(WIN32)
    target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0A00)
    target_link_libraries(server PRIVATE ws2_32)
endif()

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

    add_executable(test_consultation
        tests/test_consultation.cpp
    )
    target_link_libraries(test_consultation PRIVATE consultation_session)

    add_test(NAME database_test COMMAND test_database)
    add_test(NAME consultation_test COMMAND test_consultation)

    # End-to-end: starts the real server and drives the HTTP API.
    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_FOUND)
        add_test(NAME api_test
            COMMAND ${Python3_EXECUTABLE} tests/api_test.py $<TARGET_FILE:server>)
        set_tests_properties(api_test PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    add_test(NAME description_test COMMAND test_description)
    add_test(NAME integration_test COMMAND test_integration)

    # Database and media paths are relative to the repository root.
    set_tests_properties(
        database_test description_test integration_test consultation_test
        PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()