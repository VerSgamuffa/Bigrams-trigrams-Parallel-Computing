cmake_minimum_required(VERSION 3.21)
project(BigramsTrigrams)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp -g")

set(SOURCE_FILES
        main.cpp
        WordNgrams.cpp
        WordNgrams.h
        CharacterNgrams.cpp
        CharacterNgrams.h
        Utils.cpp
        Utils.h
        plot_ngrams.py
)

add_executable(BigramsTrigrams ${SOURCE_FILES})

