#include "tuple_hash.h"

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <tuple>
#include <iomanip>
#include <random>
#include <chrono>

void generate_random_data(const char* filename, size_t count) {
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        std::cerr << "Ошибка создания файла " << filename << std::endl;
        exit(1);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1000.f, 1000.f);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < count; i++) {
        float a = dist(gen);
        float b = dist(gen);
        float c = dist(gen);
        fout << std::fixed << std::setprecision(6) << a << " " << b << " " << c << "\n";
    }

    auto end = std::chrono::high_resolution_clock::now();
    fout.close();

    std::cout << "Сгенерировано " << count << " случайных записей." << std::endl;
    std::cout << "Время генерации данных: "
              << std::chrono::duration<double>(end - start).count()
              << " секунд\n";
}

void process_unique_data(const char* input_filename, const char* output_filename) {
    std::ifstream fin(input_filename);
    if (!fin.is_open()) {
        std::cerr << "Ошибка открытия файла: " << input_filename << std::endl;
        exit(1);
    }

    std::ofstream fout(output_filename);
    if (!fout.is_open()) {
        std::cerr << "Ошибка создания файла " << output_filename << std::endl;
        exit(1);
    }

    std::unordered_set<std::tuple<float, float, float>, TupleHash> seen;
    float a, b, c;

    size_t total_lines = 0;
    size_t unique_lines = 0;

    auto start = std::chrono::high_resolution_clock::now();

    while (fin >> a >> b >> c) {
        ++total_lines;
        auto triple = std::make_tuple(a, b, c);
        if (seen.insert(triple).second) {
            fout << std::fixed << std::setprecision(6)
                 << a << " " << b << " " << c << "\n";
            ++unique_lines;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    fin.close();
    fout.close();

    std::cout << "Всего прочитано строк: " << total_lines << std::endl;
    std::cout << "Уникальных строк: " << unique_lines << std::endl;
    std::cout << "Время обработки (чтение + фильтрация + запись): "
              << std::chrono::duration<double>(end - start).count()
              << " секунд\n";
}

int main() {
    const char* input_filename = "test_data.txt";
    const char* output_filename = "result_data.txt";
    size_t num_lines = 10000000;

    generate_random_data(input_filename, num_lines);

    process_unique_data(input_filename, output_filename);

    return 0;
}
