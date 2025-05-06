#pragma once

#include <map>
#include <functional>
#include <memory>

#include "types/def.hpp"
#include "types/complex.hpp"
#include "phys/qam/qam.hpp"

    
constexpr uint32_t ext_get_bits_per_symbol(qam_order order) { 
    switch (order) {
        case qam_order::QPSK:   return 2; 
        case qam_order::QAM16:  return 4; 
        case qam_order::QAM64:  return 6;
        default:                return 0; // For the compiler
    }
}

/**
 * @class qam_mapper
 * @brief 
 */
template<typename DTYPE, qam_order ORDER>
class qam_mapper : public mapper_base { 
public:
    using symbol_type                           = complex_t<DTYPE>;
    using constellation_map_type                = std::map<uint32_t, symbol_type>;
    using generator_function                    = std::function<constellation_map_type()>;
    
    static constexpr uint32_t BITS_PER_SYMBOL   = ext_get_bits_per_symbol(ORDER);

    /**
     * @brief default constructor 
     */
    explicit qam_mapper() {
        init_default_constellation();
    }
    
    /**
     * @brief default destructor
     */
    ~qam_mapper() = default; 

    /**
     * @brief class constructor
     */
    static std::shared_ptr<qam_mapper> make() { 
        return std::shared_ptr<qam_mapper>(new qam_mapper());
    }

    void set_generator(generator_function generator) { 
        constellation_generator_m = generator;
        generate(); 
    }

    const constellation_map_type& get_constellation() const { 
        return constellation_map_m;
    }

    qam_order get_order() const override { 
        return ORDER; 
    }

    uint32_t get_bits_per_symbol() const override { 
        return BITS_PER_SYMBOL;
    }
private:
    void generate() {
        if (constellation_generator_m) {
            constellation_map_m = constellation_generator_m();
        }
    }

    void init_default_constellation() { 
        if constexpr (ORDER == qam_order::QPSK) { 
            init_qpsk_constellation(); 
        } else if constexpr (ORDER == qam_order::QAM16) { 
            init_qam16_constellation();
        } else if constexpr (ORDER == qam_order::QAM64) { 
            init_qam64_constellation(); 
        }
    }

    void init_qpsk_constellation() { 
        constellation_generator_m = []() { 
            constellation_map_type map; 

            // QPSK constellation (Gray codes)
            // 00 -> (-1, -1)
            // 01 -> (-1, 1)
            // 10 -> ( 1, -1)
            // 11 -> ( 1, 1)

            map[0] = {static_cast<DTYPE>(-1), static_cast<DTYPE>(-1)}; 
            map[1] = {static_cast<DTYPE>(-1), static_cast<DTYPE>(1)}; 
            map[2] = {static_cast<DTYPE>(1), static_cast<DTYPE>(-1)};
            map[3] = {static_cast<DTYPE>(1), static_cast<DTYPE>(1)}; 

            return map; 
        };

        generate();
    }

    void init_qam16_constellation() { 
        constellation_generator_m = []() { 
            constellation_map_type map; 
            // QAM16 constellation (Gray codes)
            for (int i = 0; i < 16; i++) { 
                // convert idx to coords
                int gray_i = i ^ (i >> 1); 
                
                int x_idx = gray_i & 0x3;
                int y_idx = (gray_i >> 2) & 0x3; 

                DTYPE x = static_cast<DTYPE>(2 * (x_idx & 0x1) + 1); 
                DTYPE y = static_cast<DTYPE>(2 * (y_idx & 0x1) + 1); 

                if ((x_idx & 0x2) == 0) x = -x; 
                if ((y_idx & 0x2) == 0) y = -y; 

                map[i] = {x, y}; 
            }

            return map; 
        };

        generate(); 
    }

    void init_qam64_constellation() { 
        constellation_generator_m = []() { 
            constellation_map_type map; 
            // QAM64 constellation (Gray codes)
            for (int i = 0; i < 64; i++) { 
                int gray_i = i ^ (i >> 1); 

                int x_idx = gray_i & 0x7; 
                int y_idx = (gray_i >> 3) & 0x7; 

                DTYPE x = static_cast<DTYPE>(2 * (x_idx & 0x3) + 1); 
                DTYPE y = static_cast<DTYPE>(2 * (y_idx & 0x3) + 1); 

                if ((x_idx & 0x4) == 0) x = -x; 
                if ((y_idx & 0x4) == 0) y = -y;

                map[i] = {x, y}; 
            }

            return map; 
        };

        generate(); 
    }

    constellation_map_type  constellation_map_m;
    generator_function      constellation_generator_m;
};

QAM_TEMPLATES(qam_mapper, qam_order::QPSK)
QAM_TEMPLATES(qam_mapper, qam_order::QAM16)
QAM_TEMPLATES(qam_mapper, qam_order::QAM64)
