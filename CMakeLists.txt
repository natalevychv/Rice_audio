cmake_minimum_required(VERSION 3.26)
project(rice_audio)

set(CMAKE_CXX_STANDARD 17)

add_executable(rice_audio main.cpp
        AudioFile.h
        encoders-decoders.h
        writers-readers-converters.h
        stats.h)
