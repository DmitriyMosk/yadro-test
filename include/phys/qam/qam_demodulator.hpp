#pragma once

#include <memory>
#include <vector>

#include "phys/qam/qam.hpp"
#include "phys/qam/mapper.hpp"
#include "phys/chan.hpp"
#include "types/def.hpp"
#include "types/complex.hpp"

template<typename DTYPE>
class channel;

/**
 * @class qam_demodulator
 * @brief Class implementing QAM demodulator functionality
 * @tparam DTYPE Data type for components of complex number
 */
template<typename DTYPE>
class qam_demodulator : public i_qam_modem {
public:
    using ptr = std::unique_ptr<qam_demodulator>;
    
    /**
     * @brief Default constructor
     */
    explicit qam_demodulator() = default;
    
    /**
     * @brief Destructor
     */
    ~qam_demodulator() = default;
    
    /**
     * @brief Creates a QAM demodulator instance
     * @return Smart pointer to demodulator
     */
    static ptr make();
    
    /**
     * @brief Sets the mapper for demodulation
     * @param mapper_ptr Pointer to mapper
     */
    void set_mapper(std::shared_ptr<mapper_base> mapper_ptr) override;
    
    /**
     * @brief Starts the demodulation process
     */
    void run() override;
    
    /**
     * @brief Gets the current modulation order from the installed mapper
     * @return Modulation order
     */
    qam_order get_order() const;
    
    /**
     * @brief Gets the number of bits per symbol for the current modulation
     * @return Number of bits per symbol
     */
    uint32_t get_bits_per_symbol() const;
    
    /**
     * @brief Demodulates IQ symbols into a bit sequence
     * @param symbols Input IQ symbols
     * @return Bit sequence
     */
    std::vector<byte> demodulate(const complex<DTYPE>& symbols);
    
    /**
     * @brief Demodulates IQ symbols into a bit sequence using LLR
     * @param symbols Input IQ symbols
     * @param channel Channel object to get quality information from
     * @return Bit sequence
     */
    std::vector<byte> demodulate_llr(const complex<DTYPE>& symbols, const channel<DTYPE>& channel);

private:
    /**
     * @brief Writes a group of bits to a byte array
     * @param bits Array of bytes
     * @param start_bit Start bit
     * @param num_bits Number of bits
     * @param value Value to write
     */
    void write_bits(std::vector<byte>& bits, size_t start_bit, size_t num_bits, uint32_t value);
    
    /**
     * @brief Finds the closest constellation point
     * @param symbol Symbol
     * @param order Modulation order
     * @return Index of closest point
     */
    uint32_t find_nearest_symbol(const complex_t<DTYPE>& symbol, qam_order order);
    
    /**
     * @brief Finds the closest constellation point for QPSK
     * @param symbol Symbol
     * @return Index of closest point
     */
    uint32_t find_nearest_qpsk(const complex_t<DTYPE>& symbol);
    
    /**
     * @brief Finds the closest constellation point for QAM16
     * @param symbol Symbol
     * @return Index of closest point
     */
    uint32_t find_nearest_qam16(const complex_t<DTYPE>& symbol);
    
    /**
     * @brief Finds the closest constellation point for QAM64
     * @param symbol Symbol
     * @return Index of closest point
     */
    uint32_t find_nearest_qam64(const complex_t<DTYPE>& symbol);
    
    /**
     * @brief Computes the LLR for a bit in a QPSK symbol
     * @param symbol Symbol
     * @param bit_position Bit position (0 or 1)
     * @param sigma Noise standard deviation
     * @return LLR value
     */
    DTYPE calculate_llr_qpsk(const complex_t<DTYPE>& symbol, size_t bit_position, DTYPE sigma);
    
    /**
     * @brief Calculates the LLR for a bit in a QAM16 symbol
     * @param symbol Symbol
     * @param bit_position Bit position (0-3)
     * @param sigma Noise standard deviation
     * @return LLR value
     */
    DTYPE calculate_llr_qam16(const complex_t<DTYPE>& symbol, size_t bit_position, DTYPE sigma);
    
    /**
     * @brief Calculates the LLR for a bit in a QAM64 symbol
     * @param symbol Symbol
     * @param bit_position Bit position (0-5)
     * @param sigma Noise standard deviation
     * @return LLR value
     */
    DTYPE calculate_llr_qam64(const complex_t<DTYPE>& symbol, size_t bit_position, DTYPE sigma);
    
    std::shared_ptr<mapper_base> mapper_m;
};

// i know...
QAM_MODEM_TEMPLATES(qam_demodulator)
