cmake_minimum_required(VERSION 3.22)
project(lab1)

add_executable(gen1 gen1.cpp)
add_executable(gen2 gen2.cpp)

find_package(TBB REQUIRED)
find_package(DataFrame REQUIRED)

add_executable(marge marge.cpp)
target_compile_features(marge PRIVATE cxx_std_23)
target_link_libraries(marge PRIVATE TBB::tbb DataFrame::DataFrame)
