#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>

#include "phys/qam/mapper.hpp"
#include "phys/qam/qam_demodulator.hpp"
#include "phys/qam/qam_modulator.hpp"
#include "phys/chan.hpp"
#include "file/csv_writer.hpp"
#include "types/def.hpp" 

#define SYMBOL_DTYPE float

// Mutex for thread-safe console output
std::mutex cout_mutex;

std::vector<byte> generate_random_bytes(size_t length) {
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    
    std::vector<byte> random_bytes(length);
    
    for (size_t i = 0; i < length; ++i) {
        random_bytes[i] = dist(rng);
    }
    
    return random_bytes;
}


// i love xor
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

void process_modulation(int modulation_index, double sigma_start, double sigma_end, double sigma_step, 
                        int iterations, const std::string& filename) {
    std::string modulation_name;
    int test_sequence_len;
    
    if (modulation_index == 0) {
        modulation_name = "QPSK";
        test_sequence_len = 64;
    } else if (modulation_index == 1) {
        modulation_name = "QAM16";
        test_sequence_len = 64;
    } else {
        modulation_name = "QAM64";
        test_sequence_len = 64;
    }
    
    //?????
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Starting testing for " << modulation_name << std::endl;
    }
    
    std::shared_ptr<mapper_base> mapper;
    if (modulation_index == 0) {
        mapper = qam_mapper<SYMBOL_DTYPE, qam_order::QPSK>::make();
    } else if (modulation_index == 1) {
        mapper = qam_mapper<SYMBOL_DTYPE, qam_order::QAM16>::make();
    } else {
        mapper = qam_mapper<SYMBOL_DTYPE, qam_order::QAM64>::make();
    }
    
    
    auto modulator = qam_modulator<SYMBOL_DTYPE>::make();
    auto demodulator = qam_demodulator<SYMBOL_DTYPE>::make();
    
    modulator->set_mapper(mapper);
    demodulator->set_mapper(mapper);
    
    csv_writer writer;
    writer.set_file_name(filename);
    writer.set_headers("sigma,ber");
    
    for (double sigma_iter = sigma_start; sigma_iter < sigma_end; sigma_iter += sigma_step) {
        double result_ber = 0.0; 
        size_t total_errors = 0;
        size_t total_bits = 0;
        
        channel<SYMBOL_DTYPE> chan(sigma_iter);
        
        // Monte-Carlo iterations
        for (int iter = 0; iter < iterations; ++iter) {
            std::vector<byte> test_sequence = generate_random_bytes(test_sequence_len);
            
            complex<SYMBOL_DTYPE> modulated_symbols = modulator->modulate(test_sequence);
            
            complex<SYMBOL_DTYPE> noisy_symbols = chan.transmit(modulated_symbols);
            
            std::vector<byte> demodulated_bits = demodulator->demodulate(noisy_symbols);
            
            size_t errors = count_bit_errors(test_sequence, demodulated_bits);
            total_errors += errors;
            total_bits += test_sequence_len * 8;
        }
        
        result_ber = static_cast<double>(total_errors) / total_bits;

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << sigma_iter << ","
            << std::fixed << std::setprecision(15) << result_ber;
        
        writer.push_data(oss.str());
        
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << modulation_name << " - Sigma: " << std::fixed << std::setprecision(2) << sigma_iter 
                      << ", BER: " << std::fixed << std::setprecision(15) << result_ber << std::endl;
        }
    }
    
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Completed testing " << modulation_name << std::endl;
        std::cout << "Results saved to " << filename << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }
}

int main(int argc, char* argv[]) { 
    /**
     * SIM Settings
     */
    // {sigma_start, sigma_end, sigma_step}
    const std::tuple<double,double,double> sigma = {0, 10, 0.05};

    // modulations filename
    const std::string fnames[] = {
        "ber_sigma_qpsk.csv",
        "ber_sigma_qam16.csv",
        "ber_sigma_qam64.csv"
    };

    // monte-carlo iterations
    const int iterations_per_modulations = 100000;
    
    // Create threads for each modulation type
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(process_modulation, i, 
                             std::get<0>(sigma), std::get<1>(sigma), std::get<2>(sigma),
                             iterations_per_modulations, fnames[i]);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "All modulation tests completed successfully!" << std::endl;
    
    return EXIT_SUCCESS;
}
