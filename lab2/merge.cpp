#include "DataFrame/DataFrame.h"
#include <iostream>
#include <fstream>

using myDataFrame = hmdf::StdDataFrame<unsigned long>;

int main()
{
    try
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        myDataFrame df1;
        df1.read("../in1.csv", hmdf::io_format::csv2);

        myDataFrame df2;
        df2.read("../in1.csv", hmdf::io_format::csv2);

        myDataFrame dfj = df1.join_by_column<myDataFrame, int, int, float>(df2, "id", hmdf::join_policy::inner_join);

        dfj.write<unsigned long, int, float>("../out.csv", hmdf::io_format::csv2, { .precision = 4});

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "time: " << duration.count() << std::endl;
    }
    catch(const std::exception &ex)
    {
        std::cerr << "Erorr: " << ex.what() << std::endl;
    }
    return 0;
}
