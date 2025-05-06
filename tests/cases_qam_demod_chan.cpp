#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <bitset>
#include <iomanip>

#include "phys/qam/mapper.hpp"
#include "phys/qam/qam_modulator.hpp"
#include "phys/qam/qam_demodulator.hpp"
#include "phys/chan.hpp"
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

// Вспомогательная функция для сравнения битов
bool compare_bits(const std::vector<byte>& bits1, const std::vector<byte>& bits2) {
    if (bits1.size() != bits2.size()) {
        std::cout << "Error: Bit array size mismatch. Got " << bits1.size() 
                  << ", expected " << bits2.size() << std::endl;
        return false;
    }
    
    bool all_match = true;
    for (size_t i = 0; i < bits1.size(); i++) {
        if (bits1[i] != bits2[i]) {
            std::cout << "Bit mismatch at byte " << i << ": Got " 
                      << std::bitset<8>(bits1[i]) << ", expected " 
                      << std::bitset<8>(bits2[i]) << std::endl;
            all_match = false;
        }
    }
    
    return all_match;
}

void test_qam_channel() {
    std::cout << "=== Testing QAM Channel ===" << std::endl;
    
    auto qpsk_mapper = qam_mapper<float, qam_order::QPSK>::make();
    auto qam16_mapper = qam_mapper<float, qam_order::QAM16>::make();
    auto qam64_mapper = qam_mapper<float, qam_order::QAM64>::make();
    
    auto qpsk_modulator = qam_modulator<float>::make();
    auto qam16_modulator = qam_modulator<float>::make();
    auto qam64_modulator = qam_modulator<float>::make();
    
    auto qpsk_demodulator = qam_demodulator<float>::make();
    auto qam16_demodulator = qam_demodulator<float>::make();
    auto qam64_demodulator = qam_demodulator<float>::make();
    
    qpsk_modulator->set_mapper(qpsk_mapper);
    qam16_modulator->set_mapper(qam16_mapper);
    qam64_modulator->set_mapper(qam64_mapper);
    
    qpsk_demodulator->set_mapper(qpsk_mapper);
    qam16_demodulator->set_mapper(qam16_mapper);
    qam64_demodulator->set_mapper(qam64_mapper);
    
    std::vector<byte> test_bits_qpsk_qam16 = {
        0b10101010, 0b11001100, 0b00110011, 0b01010101,
        0b11110000, 0b00001111, 0b10101010, 0b11001100
    };
    
    std::vector<byte> test_bits_qam64 = {
        0b10101010, 0b11001100, 0b00110011, 0b01010101,
        0b11110000, 0b00001111
    };
    
    std::cout << "Input data QPSK & QAM16: ";
    print_bits(test_bits_qpsk_qam16);
    
    std::cout << "Input data QAM64: ";
    print_bits(test_bits_qam64);

    auto qpsk_symbols = qpsk_modulator->modulate(test_bits_qpsk_qam16);
    auto qam16_symbols = qam16_modulator->modulate(test_bits_qpsk_qam16);
    auto qam64_symbols = qam64_modulator->modulate(test_bits_qam64);
    
    std::cout << "\n--- Channel Test with No Noise (sigma = 0) ---" << std::endl;
    
    channel<float> perfect_channel(0.0);
    
    auto qpsk_noisy_symbols_0 = perfect_channel.transmit(qpsk_symbols);
    auto qam16_noisy_symbols_0 = perfect_channel.transmit(qam16_symbols);
    auto qam64_noisy_symbols_0 = perfect_channel.transmit(qam64_symbols);
    
    auto qpsk_demod_bits_0 = qpsk_demodulator->demodulate(qpsk_noisy_symbols_0);
    auto qam16_demod_bits_0 = qam16_demodulator->demodulate(qam16_noisy_symbols_0);
    auto qam64_demod_bits_0 = qam64_demodulator->demodulate(qam64_noisy_symbols_0);
    
    bool qpsk_test_0_passed = compare_bits(test_bits_qpsk_qam16, qpsk_demod_bits_0);
    bool qam16_test_0_passed = compare_bits(test_bits_qpsk_qam16, qam16_demod_bits_0);
    bool qam64_test_0_passed = compare_bits(test_bits_qam64, qam64_demod_bits_0);
    
    std::cout << "QPSK Perfect Channel Test: " << (qpsk_test_0_passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "QAM16 Perfect Channel Test: " << (qam16_test_0_passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "QAM64 Perfect Channel Test: " << (qam64_test_0_passed ? "PASSED" : "FAILED") << std::endl;
    
    // Тест 2: Канал с шумом (sigma = 0.2)
    std::cout << "\n--- Channel Test with Noise (sigma = 0.2) ---" << std::endl;
    
    channel<float> noisy_channel(0.2);
    
    auto qpsk_noisy_symbols = noisy_channel.transmit(qpsk_symbols);
    auto qam16_noisy_symbols = noisy_channel.transmit(qam16_symbols);
    auto qam64_noisy_symbols = noisy_channel.transmit(qam64_symbols);
    
    std::cout << "QPSK Noisy Symbols:" << std::endl;
    print_symbols(qpsk_noisy_symbols);
    
    auto qpsk_demod_bits = qpsk_demodulator->demodulate(qpsk_noisy_symbols);
    auto qam16_demod_bits = qam16_demodulator->demodulate(qam16_noisy_symbols);
    auto qam64_demod_bits = qam64_demodulator->demodulate(qam64_noisy_symbols);
    
    // Проверяем результаты и считаем BER (Bit Error Rate)
    size_t qpsk_errors = 0;
    size_t qam16_errors = 0;
    size_t qam64_errors = 0;
    
    for (size_t i = 0; i < test_bits_qpsk_qam16.size(); i++) {
        byte xor_result = test_bits_qpsk_qam16[i] ^ qpsk_demod_bits[i];
        for (int j = 0; j < 8; j++) {
            if ((xor_result >> j) & 1) {
                qpsk_errors++;
            }
        }
        
        xor_result = test_bits_qpsk_qam16[i] ^ qam16_demod_bits[i];
        for (int j = 0; j < 8; j++) {
            if ((xor_result >> j) & 1) {
                qam16_errors++;
            }
        }
    }
    
    for (size_t i = 0; i < test_bits_qam64.size(); i++) {
        byte xor_result = test_bits_qam64[i] ^ qam64_demod_bits[i];
        for (int j = 0; j < 8; j++) {
            if ((xor_result >> j) & 1) {
                qam64_errors++;
            }
        }
    }
    
    size_t qpsk_total_bits = test_bits_qpsk_qam16.size() * 8;
    size_t qam64_total_bits = test_bits_qam64.size() * 8;
    
    float qpsk_ber = static_cast<float>(qpsk_errors) / qpsk_total_bits;
    float qam16_ber = static_cast<float>(qam16_errors) / qpsk_total_bits;
    float qam64_ber = static_cast<float>(qam64_errors) / qam64_total_bits;
    
    std::cout << "QPSK Demodulated with noise: ";
    print_bits(qpsk_demod_bits);
    std::cout << "QPSK BER: " << qpsk_ber << " (" << qpsk_errors << "/" << qpsk_total_bits << " bits)" << std::endl;
    
    std::cout << "QAM16 Demodulated with noise: ";
    print_bits(qam16_demod_bits);
    std::cout << "QAM16 BER: " << qam16_ber << " (" << qam16_errors << "/" << qpsk_total_bits << " bits)" << std::endl;
    
    std::cout << "QAM64 Demodulated with noise: ";
    print_bits(qam64_demod_bits);
    std::cout << "QAM64 BER: " << qam64_ber << " (" << qam64_errors << "/" << qam64_total_bits << " bits)" << std::endl;
    
    std::cout << "\n--- Saving Noisy Constellations to Files ---" << std::endl;
    bool qpsk_noisy_saved = file_io::save_and_plot_modulated_symbols(qpsk_noisy_symbols, "qpsk_noisy", "QPSK Noisy", true);
    bool qam16_noisy_saved = file_io::save_and_plot_modulated_symbols(qam16_noisy_symbols, "qam16_noisy", "QAM16 Noisy", true);
    bool qam64_noisy_saved = file_io::save_and_plot_modulated_symbols(qam64_noisy_symbols, "qam64_noisy", "QAM64 Noisy", true);
    
    std::cout << "QPSK Noisy Constellation Saved: " << (qpsk_noisy_saved ? "YES" : "NO") << std::endl;
    std::cout << "QAM16 Noisy Constellation Saved: " << (qam16_noisy_saved ? "YES" : "NO") << std::endl;
    std::cout << "QAM64 Noisy Constellation Saved: " << (qam64_noisy_saved ? "YES" : "NO") << std::endl;
    
    std::cout << "\n=== QAM Channel Tests Completed ===" << std::endl;
}

int main() {
    test_qam_channel();
    return 0;
}
