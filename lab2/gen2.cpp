#include <print>
#include <fstream>
#include <random>
#include <chrono>

int main()
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    std::ofstream file("in2.csv");
    if(!file.is_open())
    {
        std::println("Erorr open file");
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 10000);

    std::println(file, "INDEX:150000:<ulong>,id:150000:<int>,value:150000:<float>");

    for(int i = 0; i < 150000; i++) //3
    {
        std::println(file, "{0},{0},{1}", i, dist(gen) / 100.0f);
    }

    file.close();

    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::println("{}", duration.count());

    return 0;
}
