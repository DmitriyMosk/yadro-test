#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <bitset>
#include <iomanip>

#include "phys/qam/mapper.hpp"
#include "phys/qam/qam_modulator.hpp"
#include "file/file.hpp"

void print_bits(const std::vector<byte>& bits) {
    std::cout << "Bits: ";
    for (byte b : bits) {
        std::cout << std::bitset<8>(b) << " ";
    }
    std::cout << std::endl;
}

template<typename DTYPE>
void print_symbols(const complex<DTYPE>& symbols) {
    std::cout << "Symbols: " << std::endl;
    for (size_t i = 0; i < symbols.size() / 2; i++) {
        auto symbol = symbols[i];
        std::cout << "  " << i << ": (" << std::fixed << std::setprecision(2) 
                  << symbol.i << ", " << symbol.q << ")" << std::endl;
    }
}

template<typename DTYPE>
bool compare_symbols(const complex<DTYPE>& symbols, const std::vector<std::pair<DTYPE, DTYPE>>& reference) {
    if (symbols.size() / 2 != reference.size()) {
        std::cout << "Error: Symbol count mismatch. Got " << symbols.size() / 2 
                  << ", expected " << reference.size() << std::endl;
        return false;
    }
    
    bool all_match = true;
    for (size_t i = 0; i < reference.size(); i++) {
        auto symbol = symbols[i];
        auto ref = reference[i];
        
        // Используем небольшой допуск для сравнения с плавающей точкой
        const DTYPE epsilon = 1e-5;
        bool match = (std::abs(symbol.i - ref.first) < epsilon && 
                      std::abs(symbol.q - ref.second) < epsilon);
        
        if (!match) {
            std::cout << "Symbol mismatch at index " << i << ": Got (" 
                      << symbol.i << ", " << symbol.q << "), expected (" 
                      << ref.first << ", " << ref.second << ")" << std::endl;
            all_match = false;
        }
    }
    
    return all_match;
}

void test_qam_modulator() {
    std::cout << "=== Testing QAM Modulator ===" << std::endl;
    
    auto qpsk_mapper = qam_mapper<float, qam_order::QPSK>::make();
    auto qam16_mapper = qam_mapper<float, qam_order::QAM16>::make();
    auto qam64_mapper = qam_mapper<float, qam_order::QAM64>::make();
    
    auto qpsk_modulator = qam_modulator<float>::make();
    auto qam16_modulator = qam_modulator<float>::make();
    auto qam64_modulator = qam_modulator<float>::make();
    
    qpsk_modulator->set_mapper(qpsk_mapper);
    qam16_modulator->set_mapper(qam16_mapper);
    qam64_modulator->set_mapper(qam64_mapper);
    
    // Тестовые данные
    std::vector<byte> test_bits = {0b10101010, 0b11001100};
    
    std::cout << "Input data: ";
    print_bits(test_bits);
    
    // Тест QPSK модуляции
    std::cout << "\n--- QPSK Modulation Test ---" << std::endl;
    auto qpsk_symbols = qpsk_modulator->modulate(test_bits);
    print_symbols(qpsk_symbols);
    
    // Референсные значения для QPSK
    // Для входных битов 10101010 11001100 ожидаемые символы:
    // 10 -> (1, -1)
    // 10 -> (1, -1)
    // 10 -> (1, -1)
    // 11 -> (1, 1)
    // 00 -> (-1, -1)
    // 11 -> (1, 1)
    // 00 -> (-1, -1)
    // 0  -> (0, 0) (последний символ может быть неполным)
    std::vector<std::pair<float, float>> qpsk_reference = {
        {1.0f, -1.0f},  // 10
        {1.0f, -1.0f},  // 10
        {1.0f, -1.0f},  // 10
        {1.0f, -1.0f},  // 10
        {1.0f, 1.0f},   // 11
        {-1.0f, -1.0f}, // 00
        {1.0f, 1.0f},   // 11
        {-1.0f, -1.0f}  // 00
    };
    
    bool qpsk_test_passed = compare_symbols(qpsk_symbols, qpsk_reference);
    std::cout << "QPSK Test: " << (qpsk_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Тест QAM16 модуляции
    std::cout << "\n--- QAM16 Modulation Test ---" << std::endl;
    auto qam16_symbols = qam16_modulator->modulate(test_bits);
    print_symbols(qam16_symbols);
    
    // Референсные значения для QAM16
    // Для входных битов 10101010 11001100 ожидаемые символы:
    // 1010 -> зависит от конкретной реализации маппера
    // 1011 -> зависит от конкретной реализации маппера
    // 0011 -> зависит от конкретной реализации маппера
    // 00   -> зависит от конкретной реализации маппера (может быть неполным)
    // Примечание: для QAM16 референсные значения зависят от конкретной реализации маппера
    // Здесь приведены примерные значения, которые нужно заменить на реальные
    std::vector<std::pair<float, float>> qam16_reference = {
        {3.0f, 3.0f},   // 1010
        {3.0f, 3.0f},   // 1010
        {1.0f, 1.0f},   // 1100
        {1.0f, 1.0f}    // 1100
    };

    bool qam16_test_passed = compare_symbols(qam16_symbols, qam16_reference);
    std::cout << "QAM16 Test: " << (qam16_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Тест QAM64 модуляции
    std::cout << "\n--- QAM64 Modulation Test ---" << std::endl;
    auto qam64_symbols = qam64_modulator->modulate(test_bits);
    print_symbols(qam64_symbols);
    
    // Референсные значения для QAM64
    // Для входных битов 10101010 11001100 ожидаемые символы:
    // 101010 -> зависит от конкретной реализации маппера
    // 101100 -> зависит от конкретной реализации маппера
    // 1100   -> зависит от конкретной реализации маппера (может быть неполным)
    // Примечание: для QAM64 референсные значения зависят от конкретной реализации маппера
    // Здесь приведены примерные значения, которые нужно заменить на реальные
    std::vector<std::pair<float, float>> qam64_reference = {
        {7.0f, 7.0f},     // 101010
        {-5.0f, 7.0f},    // 101100
        {-5.0f, -3.0f}    // 1100xx (неполный символ)
    };
    
    bool qam64_test_passed = compare_symbols(qam64_symbols, qam64_reference);
    std::cout << "QAM64 Test: " << (qam64_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "\n--- Saving Modulated Symbols to Files ---" << std::endl;
    bool qpsk_mod_saved = file_io::save_and_plot_modulated_symbols(qpsk_symbols, "qpsk_modulated", "QPSK", true);
    bool qam16_mod_saved = file_io::save_and_plot_modulated_symbols(qam16_symbols, "qam16_modulated", "QAM16", true);
    bool qam64_mod_saved = file_io::save_and_plot_modulated_symbols(qam64_symbols, "qam64_modulated", "QAM64", true);
    
    std::cout << "QPSK Modulated Symbols Saved: " << (qpsk_mod_saved ? "YES" : "NO") << std::endl;
    std::cout << "QAM16 Modulated Symbols Saved: " << (qam16_mod_saved ? "YES" : "NO") << std::endl;
    std::cout << "QAM64 Modulated Symbols Saved: " << (qam64_mod_saved ? "YES" : "NO") << std::endl;
    
    std::cout << "\n--- Saving Constellations to Files ---" << std::endl;
    bool qpsk_saved = file_io::save_and_plot_constellation(qpsk_mapper, "qpsk_constellation", true);
    bool qam16_saved = file_io::save_and_plot_constellation(qam16_mapper, "qam16_constellation", true);
    bool qam64_saved = file_io::save_and_plot_constellation(qam64_mapper, "qam64_constellation", true);
    
    std::cout << "QPSK Constellation Saved: " << (qpsk_saved ? "YES" : "NO") << std::endl;
    std::cout << "QAM16 Constellation Saved: " << (qam16_saved ? "YES" : "NO") << std::endl;
    std::cout << "QAM64 Constellation Saved: " << (qam64_saved ? "YES" : "NO") << std::endl;
    
    if (qpsk_saved && qam16_saved && qam64_saved) {
        std::cout << "\nTo view the constellations, run the following commands:" << std::endl;
        std::cout << "gnuplot qpsk_constellation.plt" << std::endl;
        std::cout << "gnuplot qam16_constellation.plt" << std::endl;
        std::cout << "gnuplot qam64_constellation.plt" << std::endl;
    }
    
    std::cout << "\n=== QAM Modulator Tests Completed ===" << std::endl;
}

int main() {
    test_qam_modulator();
    return 0;
}
