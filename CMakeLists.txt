cmake_minimum_required(VERSION 3.21)
project(uebung_3)

set(CMAKE_CXX_STANDARD 14)

add_link_options(-lgtest -lgtest_main)

include_directories(Performance)
include_directories(RangeQuery)
include_directories(Google_tests)


add_executable(uebung_3
        Performance/main.cpp
        RangeQuery/RangeTree.hpp
        RangeQuery/Stopwatch.h
        Google_tests/UnitTest.cpp RangeQuery/Point.h RangeQuery/RangeQuery.h)
