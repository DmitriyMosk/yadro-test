#include "types/complex.hpp"

#include <iostream>
#include <stdexcept>
#include <memory>

/**
 * @privatesection
 */
template<typename DTYPE> 
void complex<DTYPE>::init(size_t length) { 
    if (length == 0) {
        throw std::invalid_argument("[size_t length] must be > 0");
    }

    if (length % 2 != 0) {
        throw std::invalid_argument("[size_t length] must be even");
    }

    size_m      = length; 
    center_m    = length / 2; 
    
    try {
        arr_m = std::make_unique<DTYPE[]>(size_m);
    } catch (std::bad_alloc& e) { 
        std::cerr << "Memory allocation failed: " << e.what() << "\n";
        // throw;
    }
}

/**
 * @publicsection
 */

template<typename DTYPE>
std::array<std::span<DTYPE>, 2> complex<DTYPE>::decompose() { 
    const size_t half = size_m / 2; 

    DTYPE* data_ptr = arr_m.get(); 

    return {
        std::span<DTYPE>(data_ptr, half),       // i
        std::span<DTYPE>(data_ptr + half, half) // q
    };
}

template<typename DTYPE> 
void complex<DTYPE>::store(const complex_t<DTYPE>& val, size_t index) { 
    if (index >= size_m) { 
        throw std::out_of_range("requested index is out of range");
    }
    if (index >= center_m) { 
        throw std::invalid_argument("requested index must be half of size");
    }

    arr_m[index]                = val.i; 
    arr_m[index + center_m]     = val.q;
}

template<typename DTYPE> 
const complex_t<DTYPE> complex<DTYPE>::operator[](size_t index) const { 
    if (index >= size_m) { 
        throw std::out_of_range("requested index is out of range");
    }
    if (index >= center_m) { 
        throw std::invalid_argument("requested index must be half of size");
    }

    auto ptr = arr_m.get();

    return {ptr[index], ptr[index + center_m]};
}