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

void test_qam_modem() {
    std::cout << "=== Testing QAM Modulation/Demodulation ===" << std::endl;
    
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
    
    std::vector<byte> test_bits_qpsk_and_qam16 = {0b10101010, 0b11001100};
    std::vector<byte> test_bits_qam64 = {0b10101010, 0b11001100, 0b10011110};
    
    std::cout << "Input data QPSK & QAM16: ";
    print_bits(test_bits_qpsk_and_qam16);

    std::cout << "Input data QAM64: ";
    print_bits(test_bits_qam64);

    std::cout << "\n--- QPSK Modulation/Demodulation Test ---" << std::endl;
    auto qpsk_symbols = qpsk_modulator->modulate(test_bits_qpsk_and_qam16);
    print_symbols(qpsk_symbols);
    
    auto qpsk_demod_bits = qpsk_demodulator->demodulate(qpsk_symbols);
    std::cout << "Demodulated data: ";
    print_bits(qpsk_demod_bits);
    
    bool qpsk_test_passed = compare_bits(test_bits_qpsk_and_qam16, qpsk_demod_bits);
    std::cout << "QPSK Test: " << (qpsk_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "\n--- QAM16 Modulation/Demodulation Test ---" << std::endl;
    auto qam16_symbols = qam16_modulator->modulate(test_bits_qpsk_and_qam16);
    print_symbols(qam16_symbols);
    
    auto qam16_demod_bits = qam16_demodulator->demodulate(qam16_symbols);
    std::cout << "Demodulated data: ";
    print_bits(qam16_demod_bits);
    
    bool qam16_test_passed = compare_bits(test_bits_qpsk_and_qam16, qam16_demod_bits);
    std::cout << "QAM16 Test: " << (qam16_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "\n--- QAM64 Modulation/Demodulation Test ---" << std::endl;
    auto qam64_symbols = qam64_modulator->modulate(test_bits_qam64);
    print_symbols(qam64_symbols);
    
    auto qam64_demod_bits = qam64_demodulator->demodulate(qam64_symbols);
    std::cout << "Demodulated data: ";
    print_bits(qam64_demod_bits);
    
    bool qam64_test_passed = compare_bits(test_bits_qam64, qam64_demod_bits);
    std::cout << "QAM64 Test: " << (qam64_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "\n--- Test with Longer Bit Sequence ---" << std::endl;
    std::vector<byte> long_test_bits_qpsk_qam16 = {
        0b10101010, 0b11001100, 0b00110011, 0b01010101,
        0b11110000, 0b00001111, 0b10101010, 0b11001100
    };

    std::vector<byte> long_test_bits_qam64 = {
        0b10101010, 0b11001100, 0b00110011, 0b01010101,
        0b11110000, 0b00001111
    };
    
    std::cout << "Input data QPSK & QAM16: ";
    print_bits(long_test_bits_qpsk_qam16);
    
    std::cout << "Input data QAM64: ";
    print_bits(long_test_bits_qam64);

    // QPSK
    auto long_qpsk_symbols          = qpsk_modulator->modulate(long_test_bits_qpsk_qam16);
    auto long_qpsk_demod_bits       = qpsk_demodulator->demodulate(long_qpsk_symbols);
    bool long_qpsk_test_passed      = compare_bits(long_test_bits_qpsk_qam16, long_qpsk_demod_bits);
    std::cout << "QPSK Long Test: " << (long_qpsk_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    // QAM16
    auto long_qam16_symbols         = qam16_modulator->modulate(long_test_bits_qpsk_qam16);
    auto long_qam16_demod_bits      = qam16_demodulator->demodulate(long_qam16_symbols);
    bool long_qam16_test_passed     = compare_bits(long_test_bits_qpsk_qam16, long_qam16_demod_bits);
    std::cout << "QAM16 Long Test: " << (long_qam16_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    // QAM64
    auto long_qam64_symbols         = qam64_modulator->modulate(long_test_bits_qam64);
    auto long_qam64_demod_bits      = qam64_demodulator->demodulate(long_qam64_symbols);
    bool long_qam64_test_passed     = compare_bits(long_test_bits_qam64, long_qam64_demod_bits);
    std::cout << "QAM64 Long Test: " << (long_qam64_test_passed ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "\n=== QAM Modulation/Demodulation Tests Completed ===" << std::endl;
}

int main() {
    test_qam_modem();
    return 0;
}
