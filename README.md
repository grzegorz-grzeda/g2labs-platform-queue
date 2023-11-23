# g2labs-platform-queue

Static queue handling at platform level.

This is a [G2EPM](https://github.com/grzegorz-grzeda/g2epm) library.

This library relies on the [g2labs-platform](https://github.com/grzegorz-grzeda/g2labs-platform) library.

## Target platforms:
- [x] native (compiled for host)
- [x] ESP32 (S3) (requires the ESP-IDF to be installed and exported)
- [ ] AVR
- [ ] STM32 

## How to compile and link it?

Just include this directory in your CMake project.

Example `CMakeLists.txt` content:
```
...

add_subdirectory(<PATH TO THIS LIBRARY>)
target_link_libraries(${PROJECT_NAME} PRIVATE g2labs-platform-queue)

...
```

# Copyright
This library was written by G2Labs Grzegorz Grzęda, and is distributed under MIT Licence. Check the `LICENSE` file for more details.