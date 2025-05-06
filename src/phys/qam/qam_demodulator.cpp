#include "phys/qam/qam_demodulator.hpp"

#include <stdexcept>
#include <cmath>
#include <limits>
#include "types/complex.hpp"

template<typename DTYPE>
typename qam_demodulator<DTYPE>::ptr qam_demodulator<DTYPE>::make() {
    return std::unique_ptr<qam_demodulator>(new qam_demodulator());
}

template<typename DTYPE>
void qam_demodulator<DTYPE>::set_mapper(std::shared_ptr<mapper_base> mapper_ptr) {
    mapper_m = mapper_ptr;
    
    if (!mapper_ptr) {
        throw std::invalid_argument("Mapper cannot be null");
    }
}

template<typename DTYPE>
void qam_demodulator<DTYPE>::run() {
    // pass
}

template<typename DTYPE>
qam_order qam_demodulator<DTYPE>::get_order() const {
    if (!mapper_m) {
        throw std::runtime_error("Mapper not set");
    }
    return mapper_m->get_order();
}

template<typename DTYPE>
uint32_t qam_demodulator<DTYPE>::get_bits_per_symbol() const {
    if (!mapper_m) {
        throw std::runtime_error("Mapper not set");
    }
    return mapper_m->get_bits_per_symbol();
}

template<typename DTYPE>
std::vector<byte> qam_demodulator<DTYPE>::demodulate(const complex<DTYPE>& symbols) {
    if (!mapper_m) {
        throw std::runtime_error("Mapper not set");
    }
    
    qam_order order = mapper_m->get_order();
    uint32_t bits_per_symbol = mapper_m->get_bits_per_symbol();
    
    size_t num_symbols = symbols.size() / 2;
    size_t total_bits = num_symbols * bits_per_symbol;
    size_t num_bytes = (total_bits + 7) / 8;
    
    std::vector<byte> bits(num_bytes, 0);
    
    for (size_t i = 0; i < num_symbols; i++) {
        complex_t<DTYPE> symbol = symbols[i];
        
        uint32_t bit_group = find_nearest_symbol(symbol, order);
        
        write_bits(bits, i * bits_per_symbol, bits_per_symbol, bit_group);
    }
    
    return bits;
}

template<typename DTYPE>
std::vector<byte> qam_demodulator<DTYPE>::demodulate_llr(const complex<DTYPE>& symbols, const channel<DTYPE>& channel) {
    if (!mapper_m) {
        throw std::runtime_error("Mapper not set");
    }
    
    DTYPE sigma = channel.get_quality();
    
    qam_order order = mapper_m->get_order();
    uint32_t bits_per_symbol = mapper_m->get_bits_per_symbol();
    
    size_t num_symbols = symbols.size() / 2;
    size_t total_bits = num_symbols * bits_per_symbol;
    size_t num_bytes = (total_bits + 7) / 8;
    
    std::vector<byte> bits(num_bytes, 0);
    
    for (size_t i = 0; i < num_symbols; i++) {
        complex_t<DTYPE> symbol = symbols[i];
        
        // Для каждого бита в символе вычисляем LLR
        for (size_t j = 0; j < bits_per_symbol; j++) {
            DTYPE llr = 0;
            
            switch (order) {
                case qam_order::QPSK:
                    llr = calculate_llr_qpsk(symbol, j, sigma);
                    break;
                case qam_order::QAM16:
                    llr = calculate_llr_qam16(symbol, j, sigma);
                    break;
                case qam_order::QAM64:
                    llr = calculate_llr_qam64(symbol, j, sigma);
                    break;
                default:
                    throw std::invalid_argument("Unsupported modulation order");
            }
            
            // Если LLR > 0, бит = 1, иначе бит = 0
            if (llr > 0) {
                size_t bit_pos = i * bits_per_symbol + j;
                size_t byte_pos = bit_pos / 8;
                size_t bit_offset = bit_pos % 8;
                
                if (byte_pos < bits.size()) {
                    bits[byte_pos] |= (1 << (7 - bit_offset));
                }
            }
        }
    }
    
    return bits;
}

template<typename DTYPE>
void qam_demodulator<DTYPE>::write_bits(std::vector<byte>& bits, size_t start_bit, size_t num_bits, uint32_t value) {
    for (size_t i = 0; i < num_bits; i++) {
        size_t bit_pos = start_bit + i;
        size_t byte_pos = bit_pos / 8;
        size_t bit_offset = bit_pos % 8;
        
        if (byte_pos >= bits.size()) {
            break;
        }
        
        bool bit = (value >> (num_bits - 1 - i)) & 0x1;
        if (bit) {
            bits[byte_pos] |= (1 << (7 - bit_offset));
        } else {
            bits[byte_pos] &= ~(1 << (7 - bit_offset));
        }
    }
}

template<typename DTYPE>
uint32_t qam_demodulator<DTYPE>::find_nearest_symbol(const complex_t<DTYPE>& symbol, qam_order order) {
    switch (order) {
        case qam_order::QPSK:
            return find_nearest_qpsk(symbol);
        case qam_order::QAM16:
            return find_nearest_qam16(symbol);
        case qam_order::QAM64:
            return find_nearest_qam64(symbol);
        default:
            throw std::invalid_argument("Unsupported modulation order");
    }
}

template<typename DTYPE>
uint32_t qam_demodulator<DTYPE>::find_nearest_qpsk(const complex_t<DTYPE>& symbol) {
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QPSK>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QPSK type");
    }
    
    const auto& constellation = mapper->get_constellation();
    
    uint32_t nearest_index = 0;
    DTYPE min_distance = std::numeric_limits<DTYPE>::max();
    
    // We go through all the points of the constellation and find the closest one
    for (const auto& [index, point] : constellation) {
        DTYPE distance = (symbol.i - point.i) * (symbol.i - point.i) + 
                         (symbol.q - point.q) * (symbol.q - point.q);
        
        if (distance < min_distance) {
            min_distance = distance;
            nearest_index = index;
        }
    }
    
    return nearest_index;
}

template<typename DTYPE>
uint32_t qam_demodulator<DTYPE>::find_nearest_qam16(const complex_t<DTYPE>& symbol) {
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QAM16>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QAM16 type");
    }
    
    const auto& constellation = mapper->get_constellation();
    
    uint32_t nearest_index = 0;
    DTYPE min_distance = std::numeric_limits<DTYPE>::max();
    
    for (const auto& [index, point] : constellation) {
        DTYPE distance = (symbol.i - point.i) * (symbol.i - point.i) + 
                         (symbol.q - point.q) * (symbol.q - point.q);
        
        if (distance < min_distance) {
            min_distance = distance;
            nearest_index = index;
        }
    }
    
    return nearest_index;
}

template<typename DTYPE>
uint32_t qam_demodulator<DTYPE>::find_nearest_qam64(const complex_t<DTYPE>& symbol) {
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QAM64>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QAM64 type");
    }
    
    const auto& constellation = mapper->get_constellation();
    
    uint32_t nearest_index = 0;
    DTYPE min_distance = std::numeric_limits<DTYPE>::max();
    
    for (const auto& [index, point] : constellation) {
        DTYPE distance = (symbol.i - point.i) * (symbol.i - point.i) + 
                         (symbol.q - point.q) * (symbol.q - point.q);
        
        if (distance < min_distance) {
            min_distance = distance;
            nearest_index = index;
        }
    }
    
    return nearest_index;
}

template<typename DTYPE>
DTYPE qam_demodulator<DTYPE>::calculate_llr_qpsk(const complex_t<DTYPE>& symbol, size_t bit_position, DTYPE sigma) {
    if (bit_position >= 2) {
        throw std::invalid_argument("Bit position out of range for QPSK");
    }
    
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QPSK>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QPSK type");
    }
    
    const auto& constellation = mapper->get_constellation();
    
    // We divide the constellation into two groups: where bit = 0 and where bit = 1
    std::vector<complex_t<DTYPE>> bit_0_points;
    std::vector<complex_t<DTYPE>> bit_1_points;
    
    for (const auto& [index, point] : constellation) {
        if ((index >> bit_position) & 1) {
            bit_1_points.push_back(point);
        } else {
            bit_0_points.push_back(point);
        }
    }
    
    DTYPE min_dist_0 = std::numeric_limits<DTYPE>::max();
    DTYPE min_dist_1 = std::numeric_limits<DTYPE>::max();
    
    for (const auto& point : bit_0_points) {
        DTYPE dist = (symbol.i - point.i) * (symbol.i - point.i) + 
                     (symbol.q - point.q) * (symbol.q - point.q);
        min_dist_0 = std::min(min_dist_0, dist);
    }
    
    for (const auto& point : bit_1_points) {
        DTYPE dist = (symbol.i - point.i) * (symbol.i - point.i) + 
                     (symbol.q - point.q) * (symbol.q - point.q);
        min_dist_1 = std::min(min_dist_1, dist);
    }
    
    // Calculate LLR = log(P(bit=1|symbol) / P(bit=0|symbol))
    // With Gaussian noise: LLR = (min_dist_1 - min_dist_0) / (2 * sigma^2)
    DTYPE sigma_squared = sigma * sigma;
    if (sigma_squared <= 0) {
        sigma_squared = 1e-10; // Preventing division by zero
    }
    
    return (min_dist_1 - min_dist_0) / (2 * sigma_squared);
}

template<typename DTYPE>
DTYPE qam_demodulator<DTYPE>::calculate_llr_qam16(const complex_t<DTYPE>& symbol, size_t bit_position, DTYPE sigma) {
    if (bit_position >= 4) {
        throw std::invalid_argument("Bit position out of range for QAM16");
    }
    
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QAM16>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QAM16 type");
    }
    
    const auto& constellation = mapper->get_constellation();
    
    std::vector<complex_t<DTYPE>> bit_0_points;
    std::vector<complex_t<DTYPE>> bit_1_points;
    
    for (const auto& [index, point] : constellation) {
        if ((index >> bit_position) & 1) {
            bit_1_points.push_back(point);
        } else {
            bit_0_points.push_back(point);
        }
    }
    
    DTYPE min_dist_0 = std::numeric_limits<DTYPE>::max();
    DTYPE min_dist_1 = std::numeric_limits<DTYPE>::max();
    
    for (const auto& point : bit_0_points) {
        DTYPE dist = (symbol.i - point.i) * (symbol.i - point.i) + 
                     (symbol.q - point.q) * (symbol.q - point.q);
        min_dist_0 = std::min(min_dist_0, dist);
    }
    
    for (const auto& point : bit_1_points) {
        DTYPE dist = (symbol.i - point.i) * (symbol.i - point.i) + 
                     (symbol.q - point.q) * (symbol.q - point.q);
        min_dist_1 = std::min(min_dist_1, dist);
    }
    
    // LLR
    DTYPE sigma_squared = sigma * sigma;
    if (sigma_squared <= 0) {
        sigma_squared = 1e-10; // Preventing division by zero
    }
    
    return (min_dist_0 - min_dist_1) / (2 * sigma_squared);
}

template<typename DTYPE>
DTYPE qam_demodulator<DTYPE>::calculate_llr_qam64(const complex_t<DTYPE>& symbol, size_t bit_position, DTYPE sigma) {
    if (bit_position >= 6) {
        throw std::invalid_argument("Bit position out of range for QAM64");
    }
    
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QAM64>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QAM64 type");
    }
    
    const auto& constellation = mapper->get_constellation();
    
    std::vector<complex_t<DTYPE>> bit_0_points;
    std::vector<complex_t<DTYPE>> bit_1_points;
    
    for (const auto& [index, point] : constellation) {
        if ((index >> bit_position) & 1) {
            bit_1_points.push_back(point);
        } else {
            bit_0_points.push_back(point);
        }
    }

    DTYPE min_dist_0 = std::numeric_limits<DTYPE>::max();
    DTYPE min_dist_1 = std::numeric_limits<DTYPE>::max();
    
    for (const auto& point : bit_0_points) {
        DTYPE dist = (symbol.i - point.i) * (symbol.i - point.i) + 
                     (symbol.q - point.q) * (symbol.q - point.q);
        min_dist_0 = std::min(min_dist_0, dist);
    }
    
    for (const auto& point : bit_1_points) {
        DTYPE dist = (symbol.i - point.i) * (symbol.i - point.i) + 
                     (symbol.q - point.q) * (symbol.q - point.q);
        min_dist_1 = std::min(min_dist_1, dist);
    }
    
    DTYPE sigma_squared = sigma * sigma;
    if (sigma_squared <= 0) {
        sigma_squared = 1e-10; // Preventing division by zero
    }
    
    return (min_dist_0 - min_dist_1) / (2 * sigma_squared);
}