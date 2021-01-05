#include <iostream>
#include <immintrin.h>
#include <new>
#include <cassert>
#include <sstream>
#include "simd_util.h"
#include "pauli_string.h"
#include "chp_sim.h"
#include <chrono>
#include <complex>

void run_surface_code_sim(size_t distance) {
    size_t diam = distance * 2 - 1;
    auto qubit = [&](std::complex<float> c){
        return (size_t)(c.real() * diam + c.imag());
    };
    auto in_range = [=](std::complex<float> c) {
        return c.real() >= 0 && c.real() < (float)diam
            && c.imag() >= 0 && c.imag() < (float)diam;
    };
    std::vector<std::complex<float>> data;
    std::vector<std::complex<float>> xs;
    std::vector<std::complex<float>> zs;
    auto sim = ChpSim(diam*diam);
    for (size_t x = 0; x < diam; x++) {
        for (size_t y = 0; y < diam; y++) {
            if (x % 2 == 1 && y % 2 == 0) {
                xs.emplace_back((float)x, (float)y);
            } else if (x % 2 == 0 && y % 2 == 1) {
                zs.emplace_back((float)x, (float)y);
            } else {
                data.emplace_back((float)x, (float)y);
            }
        }
    }
    std::vector<std::complex<float>> dirs {
            {1, 0},
            {0, 1},
            {0, -1},
            {-1, 0},
    };
    for (size_t round = 0; round < distance; round++) {
        std::cerr << "round " << round << "\n";
        for (const auto &x : xs) {
            sim.hadamard(qubit(x));
        }
        for (const auto &d : dirs) {
            for (const auto &z : zs) {
                auto p = z + d;
                if (in_range(p)) {
                    sim.cnot(qubit(p), qubit(z));
                }
            }
            for (const auto &x : xs) {
                auto p = x + d;
                if (in_range(p)) {
                    sim.cnot(qubit(x), qubit(p));
                }
            }
        }
        for (const auto &z : zs) {
            assert(sim.is_deterministic(qubit(z)));
            sim.measure(qubit(z));
        }
        for (const auto &x : xs) {
            sim.hadamard(qubit(x));
        }
        for (const auto &x : xs) {
            if (round == 0) {
                std::cerr << "x measure " << x << "," << round << "\n";
            }
            assert(sim.is_deterministic(qubit(x)) == round > 0);
            sim.measure(qubit(x));
        }
    }
    for (auto d : data) {
        std::cerr << "data measure " << d << "\n";
        sim.measure(qubit(d));
    }
}

void time_clifford_sim(size_t reps, size_t distance) {
    auto start = std::chrono::steady_clock::now();
    for (size_t rep = 0; rep < reps; rep++) {
        run_surface_code_sim(distance);
    }
    auto end = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 / 1000.0;
    std::cout << dt / reps << " sec/eval " << distance << "\n";
}

void time_transpose(size_t block_diameter, size_t reps) {
    size_t w = 256 * block_diameter;
    size_t num_bits = w * w;
    auto data = aligned_bits256::random(num_bits);
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < reps; i++) {
        transpose_bit_matrix(data.data, w);
    }
    auto end = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 / 1000.0;
    std::cout << reps / dt << " transposes/sec (" << w << "x" << w << ", " << (num_bits >> 23) << " MiB)\n";
}

void time_mike_transpose(size_t block_diameter, size_t reps) {
    size_t w = 256 * block_diameter;
    size_t num_bits = w * w;
    auto data = aligned_bits256::random(num_bits);
    auto out = aligned_bits256(num_bits);
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < reps; i++) {
        mike_transpose_bit_matrix(data.data, out.data, w);
    }
    auto end = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 / 1000.0;
    std::cout << reps / dt << " mike transposes/sec ("
        << w << "x"
        << w << ", "
        << (num_bits >> 23) << " MiB, reps "
        << reps << ", dt=" << dt << "s)\n";
}

void time_transpose_blockwise(size_t block_diameter, size_t reps) {
    size_t w = 256 * block_diameter;
    size_t num_bits = w * w;
    auto data = aligned_bits256::random(num_bits);

    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < reps; i++) {
        transpose_bit_matrix_256x256blocks(data.data, w);
    }
    auto end = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 / 1000.0;
    auto transposes_ps = reps / dt;
    auto kblocks_ps = (size_t)(transposes_ps * block_diameter * block_diameter / 1000);
    std::cout << transposes_ps << " blockwise transposes/s, " << kblocks_ps << "K basic block (256x256) transposes/sec (" << w << "x" << w << ", " << (num_bits >> 23) << " MiB, " << dt << "s)\n";
}

void time_pauli_multiplication(size_t reps, size_t num_qubits) {
    auto p1 = PauliStringVal::from_pattern(
            false,
            num_qubits,
            [](size_t i) { return "_XYZX"[i % 5]; });
    auto p2 = PauliStringVal::from_pattern(
            true,
            num_qubits,
            [](size_t i) { return "_XZYZZX"[i % 7]; });
    PauliStringPtr p1_ptr = p1;
    PauliStringPtr p2_ptr = p2;
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < reps; i++) {
        p1_ptr.inplace_right_mul_with_scalar_output(p2_ptr);
    }
    auto end = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 / 1000.0;
    std::cout << (num_qubits * reps) / dt / 1000.0 / 1000.0 / 1000.0 << " GigaPauliMuls/sec (q=" << num_qubits << ",r=" << reps << ",dt=" << dt << "s)\n";
}

int main() {
//    time_pauli_multiplication(100000, 100000);
//    time_clifford_sim(1, 25);
//    time_transpose();
    size_t block_diam = 512;
    size_t reps = 1;
    time_transpose_blockwise(block_diam, reps);
    time_mike_transpose(block_diam, reps);
    time_transpose(block_diam, reps);
}
