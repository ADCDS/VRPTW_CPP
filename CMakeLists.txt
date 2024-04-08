# Set the minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.10)

# Define the project and specify the language used
project(vrptw VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the optimization flag
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

# Add pthread as a link option
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add all the source files involved in the build
add_executable(vrptw
        main.cpp
        Genetic.cpp
        Hillclimbing.cpp
        Point.cpp
        Solution.cpp
        SubCandidate.cpp
        Utils.cpp
        Vehicle.cpp
        VehicleState.cpp
        )

# Link the executable with the pthread library
target_link_libraries(vrptw PRIVATE Threads::Threads)
