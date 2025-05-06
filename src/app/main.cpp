#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "phys/qam/mapper.hpp"
#include "phys/qam/qam_demodulator.hpp"
#include "phys/qam/qam_modulator.hpp"
#include "phys/chan.hpp"
#include "file/csv_writer.hpp"
#include "types/def.hpp" 

#define SYMBOL_DTYPE float

/**
 * @brief Генерирует случайный вектор байтов заданной длины
 * @param length Длина вектора в байтах
 * @return Вектор случайных байтов
 */
std::vector<byte> generate_random_bytes(size_t length) {
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<unsigned int> dist(0, 255);
    
    std::vector<byte> random_bytes(length);
    
    for (size_t i = 0; i < length; ++i) {
        random_bytes[i] = static_cast<byte>(dist(rng));
    }
    
    return random_bytes;
}

/**
 * @brief Подсчитывает количество ошибок в битах между двумя векторами
 * @param bits1 Первый вектор байтов
 * @param bits2 Второй вектор байтов
 * @return Количество ошибок в битах
 */
size_t count_bit_errors(const std::vector<byte>& bits1, const std::vector<byte>& bits2) {
    size_t errors = 0;
    size_t min_size = std::min(bits1.size(), bits2.size());
    
    for (size_t i = 0; i < min_size; ++i) {
        byte xor_result = bits1[i] ^ bits2[i];
        for (int j = 0; j < 8; ++j) {
            if ((xor_result >> j) & 1) {
                errors++;
            }
        }
    }
    
    return errors;
}

int main(int argc, char* argv[]) { 
    /**
     * SIM Settings
     */
    // {sigma_start, sigma_end, sigma_step}
    const std::tuple<double,double,double> sigma = {0, 10, 0.02};

    // modulations filename
    const char* fnames[] = {
        "ber_sigma_qpsk.csv",
        "ber_sigma_qam16.csv",
        "ber_sigma_qam64.csv"
    };

    // monter-carlo iterations
    const int iterations_per_modulations = 100000;
    
    auto qpsk_mapper            = qam_mapper<SYMBOL_DTYPE, qam_order::QPSK>::make();
    auto qam16_mapper           = qam_mapper<SYMBOL_DTYPE, qam_order::QAM16>::make();
    auto qam64_mapper           = qam_mapper<SYMBOL_DTYPE, qam_order::QAM64>::make();

    auto qpsk_modulator         = qam_modulator<SYMBOL_DTYPE>::make();
    auto qam16_modulator        = qam_modulator<SYMBOL_DTYPE>::make();
    auto qam64_modulator        = qam_modulator<SYMBOL_DTYPE>::make();

    auto qpsk_demodulator       = qam_demodulator<SYMBOL_DTYPE>::make();
    auto qam16_demodulator      = qam_demodulator<SYMBOL_DTYPE>::make();
    auto qam64_demodulator      = qam_demodulator<SYMBOL_DTYPE>::make();

    qpsk_modulator->set_mapper(qpsk_mapper);
    qam16_modulator->set_mapper(qam16_mapper);
    qam64_modulator->set_mapper(qam64_mapper);
    
    qpsk_demodulator->set_mapper(qpsk_mapper);
    qam16_demodulator->set_mapper(qam16_mapper);
    qam64_demodulator->set_mapper(qam64_mapper);
    
    csv_writer writer;

    // i = 0 (QPSK)
    // i = 1 (QAM16)
    // i = 2 (QAM64)
    for (int i = 0; i < 3; ++i) { 
        writer.set_file_name(fnames[i]);
        writer.set_headers("sigma,ber");

        int test_sequence_len = (i == 0 || i == 1) ? 64 : 128;
        
        std::cout << "Testing " << (i == 0 ? "QPSK" : (i == 1 ? "QAM16" : "QAM64")) << std::endl;
        
        for (double sigma_iter = std::get<0>(sigma); sigma_iter < std::get<1>(sigma); sigma_iter += std::get<2>(sigma)) { 
            double result_ber = 0.0; 
            size_t total_errors = 0;
            size_t total_bits = 0;
            
            channel<SYMBOL_DTYPE> chan(sigma_iter);

            for (int iter = 0; iter < iterations_per_modulations; ++iter) {
                std::vector<byte> test_sequence = generate_random_bytes(test_sequence_len);
                
                complex<SYMBOL_DTYPE> modulated_symbols;
                if (i == 0) {
                    modulated_symbols = qpsk_modulator->modulate(test_sequence);
                } else if (i == 1) {
                    modulated_symbols = qam16_modulator->modulate(test_sequence);
                } else {
                    modulated_symbols = qam64_modulator->modulate(test_sequence);
                }

                complex<SYMBOL_DTYPE> noisy_symbols = chan.transmit(modulated_symbols);
                
                std::vector<byte> demodulated_bits;
                if (i == 0) {
                    demodulated_bits = qpsk_demodulator->demodulate(noisy_symbols);
                } else if (i == 1) {
                    demodulated_bits = qam16_demodulator->demodulate(noisy_symbols);
                } else {
                    demodulated_bits = qam64_demodulator->demodulate(noisy_symbols);
                }
                
                size_t errors = count_bit_errors(test_sequence, demodulated_bits);
                total_errors += errors;
                total_bits += test_sequence_len * 8;
            }
            
            result_ber = static_cast<double>(total_errors) / total_bits;
            
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << sigma_iter << ","
                << std::fixed << std::setprecision(15) << result_ber;
            
            writer.push_data(oss.str());
            
            std::cout << "Sigma: " << std::fixed << std::setprecision(2) << sigma_iter 
                      << ", BER: " << std::fixed << std::setprecision(15) << result_ber << std::endl;
        }
        
        std::cout << "Completed testing " << (i == 0 ? "QPSK" : (i == 1 ? "QAM16" : "QAM64")) << std::endl;
        std::cout << "Results saved to " << fnames[i] << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }

    return EXIT_SUCCESS;
}
