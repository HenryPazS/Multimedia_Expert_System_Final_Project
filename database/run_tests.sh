#!/usr/bin/env bash
set -euo pipefail

# Run from the repository root, regardless of the current folder.
cd "$(dirname "$0")/.."

mkdir -p build

tinyxml_prefix="$(brew --prefix tinyxml2)"

common_flags=(
    -std=c++17
    -Wall
    -Wextra
    "-I${tinyxml_prefix}/include"
    "-L${tinyxml_prefix}/lib"
)

common_sources=(
    database/MediaDatabase.cpp
    database/MediaDescription.cpp
)

libraries=(
    -lsqlite3
    -ltinyxml2
)

echo "Building database test..."
clang++ "${common_flags[@]}" \
    "${common_sources[@]}" \
    database/test_database.cpp \
    "${libraries[@]}" \
    -o build/test_database

echo "Building description test..."
clang++ "${common_flags[@]}" \
    "${common_sources[@]}" \
    database/test_description.cpp \
    "${libraries[@]}" \
    -o build/test_description

echo "Building integration test..."
clang++ "${common_flags[@]}" \
    "${common_sources[@]}" \
    backend/PatientProfile.cpp \
    backend/InferenceEngine.cpp \
    database/test_integration.cpp \
    "${libraries[@]}" \
    -o build/test_integration

echo
echo "Running database test..."
./build/test_database

echo
echo "Running description test..."
./build/test_description

echo
echo "Running integration test..."
./build/test_integration

echo
echo "All test programs passed."