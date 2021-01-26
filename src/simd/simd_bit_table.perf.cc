#include "simd_bit_table.h"

#include "../benchmark_util.h"

BENCHMARK(simd_bit_table_inplace_square_transpose_diam10K) {
    size_t n = 10 * 1000;
    simd_bit_table table(n, n);
    benchmark_go([&]() { table.do_square_transpose(); }).goal_millis(6).show_rate("Bits", n * n);
}

BENCHMARK(simd_bit_table_out_of_place_transpose_diam10K) {
    size_t n = 10 * 1000;
    simd_bit_table table(n, n);
    simd_bit_table out(n, n);
    benchmark_go([&]() { table.transpose_into(out); }).goal_millis(12).show_rate("Bits", n * n);
}