#ifndef H_FREQUENCY_ANALISIS_H
#define H_FREQUENCY_ANALISIS_H

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

namespace fkl
{

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

} // namespace fkl

#endif // H_FREQUENCY_ANALISIS_H