cmake_minimum_required(VERSION 3.21)
project(uebung_3)

set(CMAKE_CXX_STANDARD 14)

include_directories(Performance)
include_directories(RangeQuery)
include_directories(UnitTest)
include_directories(UnitTest2)

add_executable(uebung_3
        Performance/main.cpp
        RangeQuery/RangeTree.hpp
        RangeQuery/Stopwatch.h
        UnitTest/pch.cpp
        UnitTest/pch.h
        UnitTest/UnitTest.cpp
        UnitTest2/UnitTest.cpp RangeQuery/Point.h)
