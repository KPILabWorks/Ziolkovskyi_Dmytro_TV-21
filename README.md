## Лабараторна робота №1
`lab1` — лабараторна робота №1, містить малу бібліотеку з однієї функції та приклад використання.
Яка знаходить 5 найбільвживаних слів у потоці вводу.
Примає `std::istream&` — посилання на потік вводу.
Повертає `std::vector<std::pair<std::string, int>>` — вектор пар слів та їх кілкість у тексті.

Команда для компіляці:
`g++-14 exemple.cpp -std=c++23 -o exemple`

Приклад потребує С++23 із-за функції `std::println()`

## Лабараторна робота №2
`lab2` — лабараторна робота №2, містить два генератори csv файлів та програму об'єднання цих фійлів у третій

Команди для компіляці:
mkdir build
cd build
cmake .. -DCMAKE_CXX_COMPILER=g++-14
make
