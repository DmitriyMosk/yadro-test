#pragma once

#include <memory> 
#include <cstddef>
#include <optional>
#include <span>

#include "types/def.hpp"

/**
 * @brief Class for representing complex numbers.
 *        Simple complex value support.
 * @tparam DTYPE See COMPLEX_TEMPLATES in def.hpp
 */
template<typename DTYPE> 
class complex_t { 
public: 
    // Example usage: 
    //          complex_t<int> value(10, 10);
    //          complex_t<int> value = {10, 10}; 
    // value.i -> <i_value>
    // value.q -> <q_value>
    complex_t(DTYPE i_, DTYPE q_) : i(i_), q(q_) {}
    complex_t() : i(0), q(0) {}
    
    // void* operator new(size_t) = delete;
    // void* operator new[](size_t) = delete;

    const bool operator==(const complex_t& z_other) const { 
        return (i == z_other.i && q == z_other.q);
    }

    // In-phase component
    DTYPE i; 
    // Quadrature component
    DTYPE q; 
};
    
template<typename DTYPE>
class complex { 
public: 
    complex() = default;
    
    explicit complex(size_t length) { 
        init(length);
    }

    complex(const complex& other) { 
        init(other.size_m);
        std::copy(other.arr_m.get(), other.arr_m.get() + size_m, arr_m.get());
    }

    /**
     * @brief Complex container constructor
     */
    static complex<DTYPE> make(size_t length) { 
        return complex(length); 
    }

    /**
     * @brief Complex container constructor (unique pointer)
     */
    static std::unique_ptr<complex<DTYPE>> make_unique(size_t length) {
        return std::make_unique<complex>(length);
    }

    /**
     * @brief Get the size of the complex container
     * @return size_t - capacity of the complex container 
     */
    size_t size() const noexcept { return size_m; } 

    /**
     * @brief Allows storing a complex number in the container
     * @param val complex_t value
     * @param idx index
     * @return capacity of the complex container 
     */
    void store(const complex_t<DTYPE>& val, size_t idx); 

    /**
     * @brief Decomposes [I1,...|Q1,...] array into
     *        [I1,...] - in-phase array components
     *        [Q1,...] - quadrature array components
     * @return std::array<2>
     *      - [0] - in-phase array components
     *      - [1] - quadrature array components
     */
    std::array<std::span<DTYPE>, 2> decompose();
    
    /**
     * OPERATORS
     */
    
    /**
     * @brief Simple and safe read access to any complex value
     */
    const complex_t<DTYPE> operator[](size_t index) const;

    /**
     * @brief Assignment operator
     */ 
    complex& operator=(const complex& other) {
        if (this != &other) {
            init(other.size_m);
            std::copy(other.arr_m.get(), other.arr_m.get() + size_m, arr_m.get());
        }
        return *this;
    }

private: 
    void init(size_t length); 
    
    // [I1,I2,I3,...,In|Q1,Q2,Q3,...,Qn]
    std::unique_ptr<DTYPE[]> arr_m;    
    
    // Length of arr_m 
    size_t size_m       = 0;
    // Center of arr_m 
    size_t center_m     = 0;
};

/* 
    Define a class for some types 
    (yes, I know about templates)
*/
/* Same as  */
COMPLEX_TEMPLATES(complex)
// template class complex<short>;
COMPLEX_TEMPLATES(complex_t)
