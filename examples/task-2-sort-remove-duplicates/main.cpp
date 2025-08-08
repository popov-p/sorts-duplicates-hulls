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

    for (size_t i = 0; i < count; i++) {
        float a = dist(gen);
        float b = dist(gen);
        float c = dist(gen);
        fout << std::fixed << std::setprecision(6) << a << " " << b << " " << c << "\n";
    }
    fout.close();
}

bool validate_no_duplicates(const char* filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Ошибка открытия файла для проверки " << filename << std::endl;
        return false;
    }

    std::unordered_set<std::tuple<float, float, float>, TupleHash> seen;
    float a, b, c;
    while (fin >> a >> b >> c) {
        auto triple = std::make_tuple(a, b, c);
        if (seen.find(triple) != seen.end()) {
            std::cerr << "Найден дубликат: "
                      << a << " " << b << " " << c << std::endl;
            return false;
        }
        seen.insert(triple);
    }
    return true;
}

int main() {
    const char* input_filename = "test_data.txt";
    const char* output_filename = "result_data.txt";
    size_t num_lines = 1000000;

    auto start_gen = std::chrono::high_resolution_clock::now();
    generate_random_data(input_filename, num_lines);
    auto end_gen = std::chrono::high_resolution_clock::now();
    std::cout << "Сгенерировано случайных данных: " << num_lines << std::endl;
    std::cout << "Время генерации данных: "
              << std::chrono::duration<double>(end_gen - start_gen).count()
              << " секунд\n";

    auto start_proc = std::chrono::high_resolution_clock::now();
    std::ifstream fin(input_filename);
    if (!fin.is_open()) {
        std::cerr << "Ошибка открытия файла: " << input_filename << std::endl;
        return 1;
    }

    std::unordered_set<std::tuple<float, float, float>, TupleHash> seen;
    std::vector<std::tuple<float, float, float>> unique_lines;

    float a, b, c;
    while (fin >> a >> b >> c) {
        auto triple = std::make_tuple(a, b, c);
        if (seen.find(triple) == seen.end()) {
            seen.insert(triple);
            unique_lines.push_back(triple);
        }
    }
    fin.close();
    auto end_proc = std::chrono::high_resolution_clock::now();

    auto start_out = std::chrono::high_resolution_clock::now();
    std::ofstream fout(output_filename);
    if (!fout.is_open()) {
        std::cerr << "Ошибка создания файла " << output_filename << std::endl;
        return 1;
    }
    for (const auto& triple : unique_lines) {
        fout << std::fixed << std::setprecision(6)
             << std::get<0>(triple) << " "
             << std::get<1>(triple) << " "
             << std::get<2>(triple) << "\n";
    }
    fout.close();
    auto end_out = std::chrono::high_resolution_clock::now();

    std::cout << "Обработано уникальных строк: " << unique_lines.size()
              << ", результат сохранён в " << output_filename << std::endl;
    std::cout << "Время фильтрации уникальных: "
              << std::chrono::duration<double>(end_proc - start_proc).count()
              << " секунд\n";
    std::cout << "Время записи в файл: "
              << std::chrono::duration<double>(end_out - start_out).count()
              << " секунд\n";

    auto start_val = std::chrono::high_resolution_clock::now();
    bool valid = validate_no_duplicates(output_filename);
    auto end_val = std::chrono::high_resolution_clock::now();

    if (valid) {
        std::cout << "Проверка пройдена: дубликатов нет." << std::endl;
    } else {
        std::cout << "Внимание! Найдены дубликаты в выходном файле!" << std::endl;
    }
    std::cout << "Время валидации: "
              << std::chrono::duration<double>(end_val - start_val).count()
              << " секунд\n";

    return 0;
}

