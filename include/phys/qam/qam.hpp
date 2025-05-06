#pragma once

#include <map>
#include <functional>
#include <memory>

#include "types/def.hpp"

/** 
 *  TASK: 
 *  1. Write a C++ class implementing QAM modulator functionality (QPSK, QAM16, QAM64)
 *  2. Write a C++ class implementing QAM demodulator functionality (QPSK, QAM16, QAM64)
 */

/**
 * @enum qam_order
 * @brief Supported modulation orders
 */
enum class qam_order {
    QPSK    = 4,    //< 2 bits per symbol
    QAM16   = 16,   //< 4 bits per symbol 
    QAM64   = 64,   //< 6 bits per symbol
}; 

/**
 * @class mapper_base
 * @brief Base class for all QAM mappers
 */
class mapper_base {
public:
    virtual ~mapper_base() = default;
    
    /**
     * @brief Gets the modulation order
     * @return Modulation order
     */
    virtual qam_order get_order() const = 0;
    
    /**
     * @brief Gets the number of bits per symbol
     * @return Number of bits per symbol
     */
    virtual uint32_t get_bits_per_symbol() const = 0;
};

/**
 * @interface i_qam_modem
 * @brief Basic QAM modulator/demodulator interface 
 */
class i_qam_modem { 
public:
    virtual ~i_qam_modem() = default; 

    /**
     * @brief Sets the mapper for modulation/demodulation
     * @param mapper_ptr Pointer to the mapper
     */
    virtual void set_mapper(std::shared_ptr<mapper_base> mapper_ptr) = 0;
    
    /**
     * @brief Runs the modulation/demodulation process
     * @todo
     */
    virtual void run() = 0;
};
