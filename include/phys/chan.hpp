#pragma once

#include <cmath>
#include <random>
#include <vector>
#include "types/def.hpp"
#include "types/complex.hpp"

/**
 * @enum noise_type
 * @brief Types of noise models
 */
enum class noise_type { 
    AWGN = 1, //< AWGN NOISE MODEL (Additive White Gaussian Noise)
};

/**
 * @class noise
 * @brief Class for generating and managing noise
 * @tparam DTYPE Data type for noise components
 */
template<typename DTYPE>
class noise { 
public:
    /**
     * @brief Default constructor
     */
    explicit noise() : sigma_m(0.0), noise_seq_len_m(1000), noise_type_m(noise_type::AWGN) {
        recalc_noise();
    }
    
    /**
     * @brief Constructor with parameters
     * @param sigma Standard deviation of noise
     * @param seq_len Noise sequence length
     * @param type Noise type
     */
    explicit noise(double sigma, size_t seq_len = 1000, noise_type type = noise_type::AWGN) 
        : sigma_m(sigma), noise_seq_len_m(seq_len), noise_type_m(type) {
        recalc_noise();
    }
    
    /**
     * @brief Destructor
     */
    ~noise() = default;
    
    /**
     * @brief Gets the current sigma value
     * @return Standard deviation of noise
     */
    double get_sigma() const { 
        return sigma_m;
    }
    
    /**
     * @brief Gets the noise type
     * @return Noise type
     */
    noise_type get_type() const {
        return noise_type_m;
    }
    
    /**
     * @brief Gets the noise sequence length
     * @return Sequence length
     */
    size_t get_sequence_length() const {
        return noise_seq_len_m;
    }
    
    /**
     * @brief Recalculates the noise sequence with new sigma value
     * @param new_sigma New sigma value
     */
    void recalc_sequence(double new_sigma) { 
        if (sigma_m == new_sigma) { 
            // If sigma has not changed, do nothing
            return;
        }

        sigma_m = new_sigma;
        recalc_noise();
    }
    
    /**
     * @brief Sets the noise sequence length
     * @param seq_len New sequence length
     */
    void set_sequence_length(size_t seq_len) {
        noise_seq_len_m = seq_len;
        recalc_noise();
    }
    
    /**
     * @brief Sets the noise type
     * @param type New noise type
     */
    void set_type(noise_type type) {
        noise_type_m = type;
        recalc_noise();
    }
    
    /**
     * @brief Gets the next noise value
     * @return Noise value
     */
    DTYPE get_next_noise() {
        if (noise_sequence_m.empty()) {
            recalc_noise();
        }
        
        if (current_index_m >= noise_sequence_m.size()) {
            current_index_m = 0;
        }
        
        return noise_sequence_m[current_index_m++];
    }
    
    /**
     * @brief Adds noise to a value
     * @param value Original value
     * @return Value with added noise
     */
    DTYPE add_noise(DTYPE value) {
        return value + get_next_noise();
    }
    
    /**
     * @brief Adds noise to a complex value
     * @param value Original complex value
     * @return Complex value with added noise
     */
    complex_t<DTYPE> add_noise(const complex_t<DTYPE>& value) {
        return {value.i + get_next_noise(), value.q + get_next_noise()};
    }

protected: 
    /**
     * @brief Recalculates the noise sequence
     */
    void recalc_noise() { 
        noise_sequence_m.resize(noise_seq_len_m);
        current_index_m = 0;
        
        switch (noise_type_m) {
            case noise_type::AWGN:
                generate_awgn_noise();
                break;
            default:
                // Use AWGN by default
                generate_awgn_noise();
                break;
        }
    }
    
    /**
     * @brief Generates AWGN noise
     */
    void generate_awgn_noise() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<DTYPE> dist(0.0, sigma_m);
        
        for (size_t i = 0; i < noise_seq_len_m; i++) {
            noise_sequence_m[i] = dist(gen);
        }
    }

private: 
    double sigma_m;                    // Standard deviation of noise
    size_t noise_seq_len_m;            // Noise sequence length
    noise_type noise_type_m;           // Noise type
    std::vector<DTYPE> noise_sequence_m; // Noise sequence
    size_t current_index_m = 0;        // Current index in the sequence
};

/**
 * @class channel
 * @brief Class for modeling a communication channel
 * @tparam DTYPE Data type for signal components
 */
template<typename DTYPE>
class channel { 
public:
    /**
     * @brief Default constructor
     */
    explicit channel() : noise_m(0.0), snr_m(0.0) {}
    
    /**
     * @brief Constructor with parameters
     * @param sigma Standard deviation of noise
     */
    explicit channel(double sigma) : noise_m(sigma), snr_m(calculate_snr(sigma)) {}
    
    /**
     * @brief Destructor
     */
    ~channel() = default;
    
    /**
     * @brief Sets the channel response model
     * @param ns Noise object
     */
    void set_channel_response_model(const noise<DTYPE>& ns) {
        noise_m = ns;
        snr_m = calculate_snr(ns.get_sigma());
    }
    
    /**
     * @brief Sets the sigma value for noise
     * @param sigma New sigma value
     */
    void set_sigma(double sigma) {
        noise_m.recalc_sequence(sigma);
        snr_m = calculate_snr(sigma);
    }
    
    /**
     * @brief Gets the channel quality estimate
     * @return Sigma value
     */
    DTYPE get_quality() const {
        return static_cast<DTYPE>(noise_m.get_sigma());
    }
    
    /**
     * @brief Gets the logarithmic channel quality estimate
     * @return SNR in dB
     */
    DTYPE get_log_quality() const {
        return static_cast<DTYPE>(snr_m);
    }
    
    /**
     * @brief Passes a value through the channel (adds noise)
     * @param value Original value
     * @return Value after passing through the channel
     */
    DTYPE transmit(DTYPE value) {
        return noise_m.add_noise(value);
    }
    
    /**
     * @brief Passes a complex value through the channel
     * @param value Original complex value
     * @return Complex value after passing through the channel
     */
    complex_t<DTYPE> transmit(const complex_t<DTYPE>& value) {
        return noise_m.add_noise(value);
    }
    
    /**
     * @brief Passes a sequence of complex values through the channel
     * @param symbols Original sequence
     * @return Sequence after passing through the channel
     */
    complex<DTYPE> transmit(const complex<DTYPE>& symbols) {
        auto result = complex<DTYPE>::make(symbols.size());
        for (size_t i = 0; i < symbols.size() / 2; i++) {
            complex_t<DTYPE> symbol = symbols[i];
            complex_t<DTYPE> noisy_symbol = noise_m.add_noise(symbol);
            result.store(noisy_symbol, i);
        }
        return result;
    }

private: 
    /**
     * @brief Calculates SNR from sigma value
     * @param sigma Sigma value
     * @return SNR in dB
     */
    double calculate_snr(double sigma) {
        if (sigma <= 0) {
            return std::numeric_limits<double>::infinity();
        }
        // Assume signal power = 1
        double signal_power = 1.0;
        double noise_power = sigma * sigma;
        // SNR = 10 * log10(signal_power / noise_power)
        return 10.0 * std::log10(signal_power / noise_power);
    }

    noise<DTYPE> noise_m;  // Noise object
    double snr_m;          // Signal-to-noise ratio in dB
};

// Explicit template instantiation declarations
CHANNEL_TEMPLATES(noise)
CHANNEL_TEMPLATES(channel)
