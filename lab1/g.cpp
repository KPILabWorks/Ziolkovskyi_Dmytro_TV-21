#include <print>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>

std::vector<std::pair<std::string, int>> frequency_analysis(std::istream& file)
{
    std::unordered_map<std::string, int> word_count;
    std::string word;

    while(file >> word)
    {
        word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c)
        {
            return std::ispunct(c);
        }), word.end());

        if(!word.empty())
        {  
            ++word_count[word];
        }
    }

    std::vector<std::pair<std::string, int>> vec(word_count.begin(), word_count.end());

    std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {return a.second > b.second;});

    vec.resize(5);

    return vec;
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::println("Erorr paramtr");
        return 1;
    }

    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::println("Erorr open file");
        return 1;
    }

    std::vector<std::pair<std::string, int>> word_count = frequency_analysis(file);

    for(const auto &[key, value] : word_count)
    {
        std::println("{}: {}", key, value);
    }

    return 0;
}
