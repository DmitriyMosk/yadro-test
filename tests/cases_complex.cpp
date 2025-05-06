#include <cassert>
#include <iostream>
#include "types/complex.hpp"

/**
 * TEST for class complex_t
 */
bool test_complex_t() {

    complex_t<int16_t> val_1(1,2); 

    assert(val_1.i == 1 && val_1.q == 2 && "val_1.i == 1 && val_1.q");

    complex_t<int16_t> val_2 = {1,2};

    assert(val_2.i == 1 && val_2.q == 2 && "val_2.i == 1 && val_2.q");

    complex_t<int16_t> val_3[] = { {1,2}, {2,4} }; 

    for (int i = 0; i < 2; ++i) { 
        assert(val_3[i].i == 1 * (i + 1) && val_3[i].q == 2 * (i + 1) && "val_3[i].i == 1 * (i + 1) && val_3[i].q == 2 * (i + 1)");
    }

    assert(val_1 == val_2 && "equal operator test failed: val_1 == val_2"); 

    return true; 
}

/**
 * TEST for class complex
 */
bool test_complex() { 
    const size_t N = 6;
    
    const complex_t<int16_t> iq_array[N] = { 
        {1,2}, {2,4}, {4,6}, 
        {6, 8}, {8,10}, {10,12} 
    }; 

    auto container_1 = complex<int16_t>::make(N * 2); 
    
    for (size_t i = 0; i < N; ++i) { 
        container_1.store(iq_array[i], i); 
    }

    try { 
        container_1.store(iq_array[0], N * 2); 
        assert(false && "overflow test failed: container_1.store(iq_array[0], N * 2) should throw");
    } catch (const std::out_of_range& e) { 
        // pass 
    }
    
    try { 
        container_1.store(iq_array[0], N);
        assert(false && "invalid index test failed: container_1.store(iq_array[0], N) should throw");
    } catch (const std::invalid_argument& e) { 
        // pass
    }

    assert(container_1.size() == N * 2 && "check size test failed: container_1.size() == N * 2"); 

    for (size_t i = 0; i < N; ++i) { 
        complex_t<int16_t> val = container_1[i];
        assert(val == iq_array[i] && "check validity stored data failed: val == iq_array[i]");
    }

    return true; 
}


/**
 * TEST для метода decompose()
 */
bool test_complex_decompose() {
    const size_t N = 6;
    
    const complex_t<int16_t> iq_array[N] = { 
        {1,2}, {2,4}, {4,6}, 
        {6,8}, {8,10}, {10,12} 
    }; 

    auto container = complex<int16_t>::make(N * 2); 
    
    for (size_t i = 0; i < N; ++i) { 
        container.store(iq_array[i], i); 
    }

    auto dec = container.decompose(); 
    assert(dec[0].size() == N && "I components size mismatch: dec[0].size() == N");
    assert(dec[1].size() == N && "Q components size mismatch: dec[1].size() == N");

    for (size_t i = 0; i < N; ++i) { 
        assert(dec[0][i] == iq_array[i].i && "I component mismatch: dec[0][i] == iq_array[i].i");
        assert(dec[1][i] == iq_array[i].q && "Q component mismatch: dec[1][i] == iq_array[i].q"); 
    }

    return true;
}

/**
 * TEST для make_unique и работы через указатель
 */
bool test_complex_unique_ptr() {
    const size_t N = 6;
    
    const complex_t<int16_t> iq_array[N] = { 
        {1,2}, {2,4}, {4,6}, 
        {6,8}, {8,10}, {10,12} 
    }; 

    auto container_ptr = complex<int16_t>::make_unique(N * 2); 
    
    for (size_t i = 0; i < N; ++i) { 
        container_ptr->store(iq_array[i], i); 
    }

    assert(container_ptr->size() == N * 2 && "check size test failed: container_ptr->size() == N * 2");

    for (size_t i = 0; i < N; ++i) { 
        complex_t<int16_t> val = (*container_ptr)[i];
        assert(val == iq_array[i] && "check validity stored data failed: val == iq_array[i]");
    }

    auto dec = container_ptr->decompose();
    assert(dec[0].size() == N && "I components size mismatch: dec[0].size() == N"); 
    assert(dec[1].size() == N && "Q components size mismatch: dec[1].size() == N"); 

    for (size_t i = 0; i < N; ++i) {
        assert(dec[0][i] == iq_array[i].i && "I component mismatch: dec[0][i] == iq_array[i].i");
        assert(dec[1][i] == iq_array[i].q && "Q component mismatch: dec[1][i] == iq_array[i].q");
    }

    return true;
}

/**
 * TEST для конструктора копирования
 */
bool test_complex_copy_constructor() {
    const size_t N = 6;
    
    const complex_t<int16_t> iq_array[N] = { 
        {1,2}, {2,4}, {4,6}, 
        {6,8}, {8,10}, {10,12} 
    }; 

    auto original = complex<int16_t>::make(N * 2); 
    
    for (size_t i = 0; i < N; ++i) { 
        original.store(iq_array[i], i); 
    }

    // Используем конструктор копирования
    complex<int16_t> copy(original);
    
    assert(copy.size() == original.size() && "copy size != original size");
    
    for (size_t i = 0; i < N; ++i) { 
        complex_t<int16_t> val_original = original[i];
        complex_t<int16_t> val_copy = copy[i];
        assert(val_original == val_copy && "copy data != original data");
    }

    return true;
}

/**
 * TEST для граничных случаев
 */
bool test_complex_edge_cases() {
    try {
        auto container = complex<int16_t>::make(2);
        complex_t<int16_t> val = {1, 2};
        container.store(val, 0);
        assert(container[0] == val && "minimal size container failed");
    } catch (const std::exception& e) {
        std::cerr << "Minimal size test failed: " << e.what() << std::endl;
        return false;
    }
    
    try {
        auto container = complex<int16_t>::make(3);
        assert(false && "odd size should throw exception");
    } catch (const std::invalid_argument& e) {
        // Это ожидаемое поведение
    }
    
    try {
        auto container = complex<int16_t>::make(0);
        assert(false && "zero size should throw exception");
    } catch (const std::invalid_argument& e) {
        // Это ожидаемое поведение
    }
    
    return true;
}

/**
 * TEST для разных типов данных
 */
template<typename T>
bool test_complex_different_types() {
    const size_t N = 4;
    
    const complex_t<T> iq_array[N] = { 
        {T(1),T(2)}, {T(2),T(4)}, {T(4),T(6)}, {T(6),T(8)}
    }; 

    auto container = complex<T>::make(N * 2); 
    
    for (size_t i = 0; i < N; ++i) { 
        container.store(iq_array[i], i); 
    }

    for (size_t i = 0; i < N; ++i) { 
        complex_t<T> val = container[i];
        assert(val == iq_array[i] && "check validity stored data failed");
    }

    auto dec = container.decompose(); 
    assert(dec[0].size() == N && "I components size mismatch");
    assert(dec[1].size() == N && "Q components size mismatch");

    for (size_t i = 0; i < N; ++i) { 
        assert(dec[0][i] == iq_array[i].i && "I component mismatch");
        assert(dec[1][i] == iq_array[i].q && "Q component mismatch"); 
    }

    return true;
}

int main() { 
    assert(test_complex_t() == true && "test_complex_t() != true");
    assert(test_complex() == true && "test_complex() != true");
    assert(test_complex_decompose() == true && "test_complex_decompose() != true");
    assert(test_complex_unique_ptr() == true && "test_complex_unique_ptr() != true");
    assert(test_complex_copy_constructor() == true && "test_complex_copy_constructor() != true");
    assert(test_complex_edge_cases() == true && "test_complex_edge_cases() != true");
    
    assert(test_complex_different_types<int8_t>() == true && "test_complex_different_types<int8_t>() != true");
    assert(test_complex_different_types<int16_t>() == true && "test_complex_different_types<int16_t>() != true");
    assert(test_complex_different_types<int32_t>() == true && "test_complex_different_types<int32_t>() != true");
    assert(test_complex_different_types<float>() == true && "test_complex_different_types<float>() != true");
    assert(test_complex_different_types<double>() == true && "test_complex_different_types<double>() != true");
    
    std::cout << "All tests passed successfully!" << std::endl;
    return EXIT_SUCCESS;
}
