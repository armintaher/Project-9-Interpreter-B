cmake_minimum_required(VERSION 3.8)
project(Project_9_Interpreter_B)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        dataStructures.h
        helpers.h
        Input.cpp
        main.cpp
        Parse.h
        runBlip.cpp
        test1.blip
        test2.blip
        test3.blip
        test4.blip
        test5.blip
        test6.blip
        test7.blip
        test8.blip
        test9.blip String.h)

add_executable(Project_9_Interpreter_B ${SOURCE_FILES})