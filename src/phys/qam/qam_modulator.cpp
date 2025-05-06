#include <stdexcept>
#include "phys/qam/qam_modulator.hpp"

template<typename DTYPE>
typename qam_modulator<DTYPE>::ptr qam_modulator<DTYPE>::make() {
    return std::unique_ptr<qam_modulator>(new qam_modulator());
}

template<typename DTYPE>
void qam_modulator<DTYPE>::set_mapper(std::shared_ptr<mapper_base> mapper_ptr) {
    mapper_m = mapper_ptr;
    
    if (!mapper_ptr) {
        throw std::invalid_argument("Mapper cannot be null");
    }
}

template<typename DTYPE>
void qam_modulator<DTYPE>::run() {
    //pass
}

template<typename DTYPE>
qam_order qam_modulator<DTYPE>::get_order() const {
    if (!mapper_m) {
        throw std::runtime_error("Mapper not set");
    }
    return mapper_m->get_order();
}

template<typename DTYPE>
uint32_t qam_modulator<DTYPE>::get_bits_per_symbol() const {
    if (!mapper_m) {
        throw std::runtime_error("Mapper not set");
    }
    return mapper_m->get_bits_per_symbol();
}

template<typename DTYPE>
complex<DTYPE> qam_modulator<DTYPE>::modulate(const std::vector<byte>& bits) {
    if (!mapper_m) {
        throw std::runtime_error("Mapper not set");
    }
    
    qam_order order = mapper_m->get_order();
    uint32_t bits_per_symbol = mapper_m->get_bits_per_symbol();
    
    size_t total_bits = bits.size() * 8;
    size_t num_symbols = total_bits / bits_per_symbol;
    if (total_bits % bits_per_symbol != 0) {
        num_symbols++;
    }
    
    auto symbols = complex<DTYPE>::make(num_symbols * 2);
    
    for (size_t i = 0; i < num_symbols; i++) {
        uint32_t bit_group = extract_bits(bits, i * bits_per_symbol, bits_per_symbol);
        
        complex_t<DTYPE> symbol = map_symbol(bit_group, order);
        
        symbols.store(symbol, i);
    }
    
    return symbols;
}

template<typename DTYPE>
uint32_t qam_modulator<DTYPE>::extract_bits(const std::vector<byte>& bits, size_t start_bit, size_t num_bits) {
    uint32_t result = 0;
    for (size_t i = 0; i < num_bits; i++) {
        size_t bit_pos = start_bit + i;
        size_t byte_pos = bit_pos / 8;
        size_t bit_offset = bit_pos % 8;
        
        if (byte_pos >= bits.size()) {
            break;
        }
        
        bool bit = (bits[byte_pos] >> (7 - bit_offset)) & 0x1;
        result = (result << 1) | (bit ? 1 : 0);
    }
    return result;
}

template<typename DTYPE>
complex_t<DTYPE> qam_modulator<DTYPE>::map_symbol(uint32_t bits, qam_order order) {
    switch (order) {
        case qam_order::QPSK:
            return map_qpsk(bits);
        case qam_order::QAM16:
            return map_qam16(bits);
        case qam_order::QAM64:
            return map_qam64(bits);
        default:
            throw std::invalid_argument("Unsupported modulation order");
    }
}

template<typename DTYPE>
complex_t<DTYPE> qam_modulator<DTYPE>::map_qpsk(uint32_t bits) {
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QPSK>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QPSK type");
    }
    
    const auto& constellation = mapper->get_constellation();
    auto it = constellation.find(bits & 0x3);
    if (it != constellation.end()) {
        return it->second;
    }
    return {0, 0};
}

template<typename DTYPE>
complex_t<DTYPE> qam_modulator<DTYPE>::map_qam16(uint32_t bits) {
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QAM16>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QAM16 type");
    }
    
    const auto& constellation = mapper->get_constellation();
    auto it = constellation.find(bits & 0xF);
    if (it != constellation.end()) {
        return it->second;
    }
    return {0, 0};
}

template<typename DTYPE>
complex_t<DTYPE> qam_modulator<DTYPE>::map_qam64(uint32_t bits) {
    auto mapper = std::dynamic_pointer_cast<qam_mapper<DTYPE, qam_order::QAM64>>(mapper_m);
    if (!mapper) {
        throw std::runtime_error("Failed to cast mapper to QAM64 type");
    }
    
    const auto& constellation = mapper->get_constellation();
    auto it = constellation.find(bits & 0x3F);
    if (it != constellation.end()) {
        return it->second;
    }
    return {0, 0};
}