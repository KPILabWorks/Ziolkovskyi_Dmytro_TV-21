#include <print>
#include <fstream>
#include <random>
#include <chrono>

int main()
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    std::ofstream file("in1.csv");
    if(!file.is_open())
    {
        std::println("Erorr open file");
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 150000); //3

    std::println(file, "INDEX:5000000:<ulong>,num:5000000:<int>,id:5000000:<int>");

    for(int i = 0; i < 5000000; i++) //20
    {
        std::println(file, "{0},{0},{1}", i, dist(gen));
    }

    file.close();

    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::println("{}", duration.count());

    return 0;
}
