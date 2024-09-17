# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lvgl)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

file(GLOB ui_image_sources src/ui/images/*.c)
target_sources(app PRIVATE ${ui_image_sources})

file(GLOB ui_sources src/ui/*.c)
target_sources(app PRIVATE ${ui_sources})

file(GLOB advertisement_sources src/advertisement/*.c)
target_sources(app PRIVATE ${advertisement_sources})

file(GLOB threads_sources src/threads/*.c)
target_sources(app PRIVATE ${threads_sources})
