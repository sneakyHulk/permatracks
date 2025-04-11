#include <common_output.h>

#include <array>
#include <iostream>

#include "codegen/lib/model/model.h"

extern "C" {
#include "codegen/lib/model_jacobian/model_jacobian.h"
#include "sensor_array_dipol_model.h"
}

#include <benchmark/benchmark.h>

#include "dipol_model.h"

static void BM_sympy_codegen(benchmark::State& state) {
	// Perform setup here
	double x = 1.0, y = 2.0, z = 3.0, m1 = 4.0, theta1 = 5.0, phi1 = 6.0, Gx = 7.0, Gy = 8.0, Gz = 9.0;

	std::array<double, 48> res{};
	std::array<double, 384> res2{};

	for (auto _ : state) {
		sensor_array_dipol_model(&res[0], nullptr, m1, x, y, z, theta1, phi1, Gx, Gy, Gz);
		sensor_array_dipol_model_jacobian(nullptr, &res2[0], m1, x, y, z, theta1, phi1, Gx, Gy, Gz);

		benchmark::DoNotOptimize(res);
		benchmark::DoNotOptimize(res2);
	}
}
// Register the function as a benchmark
BENCHMARK(BM_sympy_codegen);

static void BM_sympy_cse_codegen(benchmark::State& state) {
	// Perform setup here
	double x = 1.0, y = 2.0, z = 3.0, m1 = 4.0, theta1 = 5.0, phi1 = 6.0, Gx = 7.0, Gy = 8.0, Gz = 9.0;

	std::array<double, 48> res{};
	std::array<double, 384> res2{};

	for (auto _ : state) {
		dipol_model(&res[0], m1, x, y, z, theta1, phi1, Gx, Gy, Gz);
		dipol_model_jacobian(&res2[0], m1, x, y, z, theta1, phi1);

		benchmark::DoNotOptimize(res);
		benchmark::DoNotOptimize(res2);
	}
}
// Register the function as a benchmark
BENCHMARK(BM_sympy_cse_codegen);

static void BM_matlab_codegen(benchmark::State& state) {
	// Perform setup here
	double x = 1.0, y = 2.0, z = 3.0, m1 = 4.0, theta1 = 5.0, phi1 = 6.0, Gx = 7.0, Gy = 8.0, Gz = 9.0;

	std::array<double, 48> res{};
	std::array<double, 384> res2{};

	for (auto _ : state) {
		model(Gx, Gy, Gz, m1, phi1, theta1, x, y, z, &res[0]);
		model_jacobian(m1, phi1, theta1, x, y, z, &res2[0]);

		benchmark::DoNotOptimize(res);
		benchmark::DoNotOptimize(res2);
	}
}
// Register the function as a benchmark
BENCHMARK(BM_matlab_codegen);

// Run the benchmark
BENCHMARK_MAIN();