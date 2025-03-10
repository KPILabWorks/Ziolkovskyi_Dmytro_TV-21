#include <print>
#include <vector>
#include <string>
#include <fstream>
#include "frequency_analysis.hpp"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::println(std::cerr, "Erorr paramtr");
        return 1;
    }

    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::println(std::cerr, "Erorr open file");
        return 1;
    }

    std::vector<std::pair<std::string, int>> word_count = fkl::frequency_analysis(file);

    for(const auto &[word, count] : word_count)
    {
        std::println("{}: {}", word, count);
    }

    return 0;
}
