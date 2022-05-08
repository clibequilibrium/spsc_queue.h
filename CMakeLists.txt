cmake_minimum_required(VERSION 3.0)
project(spsc-queue)

add_executable(main main.c spsc_queue.h)