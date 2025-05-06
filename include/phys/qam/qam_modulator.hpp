#pragma once

#include <memory>
#include <vector>

#include "phys/qam/qam.hpp"
#include "phys/qam/mapper.hpp"
#include "types/def.hpp"
#include "types/complex.hpp"

/**
 * @class qam_modulator
 * @brief Class implementing QAM modulation functionality
 * @tparam DTYPE Data type for complex number components
 */
template<typename DTYPE>
class qam_modulator : public i_qam_modem {
public:
    using ptr = std::unique_ptr<qam_modulator>;
    
    /**
     * @brief Default constructor
     */
    explicit qam_modulator() = default;
    
    /**
     * @brief Destructor
     */
    ~qam_modulator() = default;
    
    /**
     * @brief Creates an instance of the QAM modulator
     * @return Smart pointer to the modulator
     */
    static ptr make();
    
    /**
     * @brief Sets the mapper for modulation
     * @param mapper_ptr Pointer to the mapper
     */
    void set_mapper(std::shared_ptr<mapper_base> mapper_ptr) override;
    
    /**
     * @brief Starts the modulation process
     */
    void run() override;
    
    /**
     * @brief Gets the current modulation order from the set mapper
     * @return Modulation order
     */
    qam_order get_order() const;
    
    /**
     * @brief Gets the number of bits per symbol for the current modulation
     * @return Number of bits per symbol
     */
    uint32_t get_bits_per_symbol() const;
    
    /**
     * @brief Modulates a bit sequence into IQ symbols
     * @param bits Input bit sequence
     * @return Container with IQ symbols
     */
    complex<DTYPE> modulate(const std::vector<byte>& bits);

private:
    /**
     * @brief Extracts a group of bits from a byte array
     * @param bits Byte array
     * @param start_bit Starting bit
     * @param num_bits Number of bits
     * @return Extracted group of bits as an integer
     */
    uint32_t extract_bits(const std::vector<byte>& bits, size_t start_bit, size_t num_bits);
    
    /**
     * @brief Maps a group of bits to an IQ symbol
     * @param bits Group of bits
     * @param order Modulation order
     * @return IQ symbol
     */
    complex_t<DTYPE> map_symbol(uint32_t bits, qam_order order);
    
    /**
     * @brief Maps a group of bits to an IQ symbol for QPSK
     * @param bits Group of bits
     * @return IQ symbol
     */
    complex_t<DTYPE> map_qpsk(uint32_t bits);
    
    /**
     * @brief Maps a group of bits to an IQ symbol for QAM16
     * @param bits Group of bits
     * @return IQ symbol
     */
    complex_t<DTYPE> map_qam16(uint32_t bits);
    
    /**
     * @brief Maps a group of bits to an IQ symbol for QAM64
     * @param bits Group of bits
     * @return IQ symbol
     */
    complex_t<DTYPE> map_qam64(uint32_t bits);
    
    std::shared_ptr<mapper_base> mapper_m;
};

QAM_MODEM_TEMPLATES(qam_modulator)
