cmake_minimum_required(VERSION 3.22)
project(lab1)

add_executable(gen1 gen1.cpp)
target_compile_features(gen1 PRIVATE cxx_std_23)

add_executable(gen2 gen2.cpp)
target_compile_features(gen2 PRIVATE cxx_std_23)

find_package(TBB REQUIRED)
find_package(DataFrame REQUIRED)

add_executable(merge merge.cpp)
target_compile_features(merge PRIVATE cxx_std_23)
target_link_libraries(merge PRIVATE TBB::tbb DataFrame::DataFrame)
