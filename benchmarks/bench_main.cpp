/**
 * @file bench_main.cpp
 * @brief Deer Evaluation & Benchmarking Suite
 *
 * Self-contained harness that runs four experiment categories against
 * the Deer KVTC/TurboQuant compression pipeline:
 *   1. Round-Trip Fidelity  (MSE, cosine similarity)
 *   2. Compression Ratio    (bytes saved, gzip baseline)
 *   3. Latency              (μs percentiles over 100 iterations)
 *   4. Golden Fact Retention (feature-space signal distinguishability)
 *
 * Build:  cmake --build build --target deer_bench
 * Run:    ./build/deer_bench
 */

#include <Eigen/Dense>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <deer/ops/compress.h>
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <numeric>
#include <set>
#include <sstream>
#include <vector>
#include <zlib.h>

#include "conversation_generator.h"

using json = nlohmann::json;

namespace {

/**
 * RAII guard that redirects std::cout to a null stream.
 */
struct SuppressCout {
    std::ostringstream sink;
    std::streambuf *original;
    SuppressCout() : sink(), original(std::cout.rdbuf(sink.rdbuf())) {}
    ~SuppressCout() { std::cout.rdbuf(original); }
};

/**
 * Returns the feature count for a given compression level.
 */
size_t feature_count(deer::config::CompressionLevel level) {
    switch (level) {
    case deer::config::CompressionLevel::Fast:
        return deer::config::FEATURE_COUNT_FAST;
    case deer::config::CompressionLevel::Balanced:
        return deer::config::FEATURE_COUNT_BALANCED;
    case deer::config::CompressionLevel::Max:
        return deer::config::FEATURE_COUNT_MAX;
    }
    return deer::config::FEATURE_COUNT_BALANCED;
}

const char *level_name(deer::config::CompressionLevel level) {
    switch (level) {
    case deer::config::CompressionLevel::Fast:
        return "fast";
    case deer::config::CompressionLevel::Balanced:
        return "balanced";
    case deer::config::CompressionLevel::Max:
        return "max";
    }
    return "balanced";
}

/**
 * Builds the raw feature matrix from turns, matching the pipeline's first stage.
 */
Eigen::MatrixXd build_feature_matrix(const json &turns, deer::config::CompressionLevel level) {
    const size_t chunk_size  = deer::config::CHUNK_SIZE;
    const size_t num_chunks  = (turns.size() + chunk_size - 1) / chunk_size;
    const size_t num_feats   = feature_count(level);

    Eigen::MatrixXd feats(static_cast<Eigen::Index>(num_chunks),
                          static_cast<Eigen::Index>(num_feats));
    feats.setZero();

    for (size_t c = 0; c < num_chunks; ++c) {
        deer::ops::fill_chunk_features(feats, turns, c, chunk_size, level);
    }
    return feats;
}

/**
 * Decompresses the coded archive and returns the restored feature matrix.
 * Mirrors deer::ops::decompressArchive but returns the matrix instead of printing.
 */
Eigen::MatrixXd decompress_to_matrix(const json &codedArchive) {
    const std::string &hex_data = codedArchive["__deflate"].get<std::string>();
    const size_t compressed_size = hex_data.length() / 2;

    std::vector<Bytef> compressed(compressed_size);
    for (size_t i = 0; i < compressed_size; ++i) {
        compressed[i] = static_cast<Bytef>(std::stoi(hex_data.substr(i * 2, 2), nullptr, 16));
    }

    std::vector<Bytef> decompressed(codedArchive["original_size"].get<size_t>());
    uLongf dest_len = decompressed.size();
    ::uncompress(decompressed.data(), &dest_len, compressed.data(), compressed_size);

    const std::string json_str(reinterpret_cast<char *>(decompressed.data()), dest_len);
    const json decoded = json::parse(json_str);

    const Eigen::Index num_chunks   = static_cast<Eigen::Index>(decoded.size());
    const Eigen::Index num_features = static_cast<Eigen::Index>(decoded[0].size());

    const bool has_per_channel = codedArchive.contains("col_min") &&
                                 codedArchive.contains("col_max");

    Eigen::MatrixXd quantized(num_chunks, num_features);
    for (Eigen::Index i = 0; i < num_chunks; ++i) {
        for (Eigen::Index j = 0; j < num_features; ++j) {
            const int q = decoded[i][j].get<int>();
            if (has_per_channel) {
                const double col_min = codedArchive["col_min"][j].get<double>();
                const double col_max = codedArchive["col_max"][j].get<double>();
                const double scale   = (col_max > col_min) ? (col_max - col_min) : 1.0;
                quantized(i, j)      = col_min + ((static_cast<double>(q) / 15.0) * scale);
            } else {
                quantized(i, j) = static_cast<double>(q) / deer::config::QUANTIZATION_RANGE;
            }
        }
    }

    Eigen::MatrixXd restored = quantized;
    if (codedArchive.contains("rotation_seed")) {
        const unsigned int seed = codedArchive["rotation_seed"].get<unsigned int>();
        std::srand(seed);
        const Eigen::MatrixXd rotation_matrix =
            Eigen::MatrixXd::Random(num_features, num_features).householderQr().householderQ();
        restored = quantized * rotation_matrix.transpose();
    }
    return restored;
}

/**
 * Computes MSE between two identically-shaped matrices.
 */
double compute_mse(const Eigen::MatrixXd &a, const Eigen::MatrixXd &b) {
    return (a - b).array().square().mean();
}

/**
 * Computes average cosine similarity across rows.
 */
double compute_cosine_similarity(const Eigen::MatrixXd &a, const Eigen::MatrixXd &b) {
    double total = 0.0;
    for (Eigen::Index i = 0; i < a.rows(); ++i) {
        const double dot  = a.row(i).dot(b.row(i));
        const double norm = a.row(i).norm() * b.row(i).norm();
        total += (norm > 1e-12) ? (dot / norm) : 1.0;
    }
    return total / static_cast<double>(a.rows());
}

/**
 * Computes max absolute error across all elements.
 */
double compute_max_abs_error(const Eigen::MatrixXd &a, const Eigen::MatrixXd &b) {
    return (a - b).array().abs().maxCoeff();
}

/**
 * Returns the gzipped size of a string (raw zlib compression for baseline).
 */
size_t gzip_size(const std::string &input) {
    uLongf dest_len = compressBound(input.size());
    std::vector<Bytef> buf(dest_len);
    ::compress(buf.data(), &dest_len,
               reinterpret_cast<const Bytef *>(input.data()), input.size());
    return static_cast<size_t>(dest_len);
}

/**
 * Computes percentile from a sorted vector.
 */
double percentile(const std::vector<double> &sorted, double p) {
    const double idx = p / 100.0 * static_cast<double>(sorted.size() - 1);
    const size_t lo  = static_cast<size_t>(std::floor(idx));
    const size_t hi  = std::min(lo + 1, sorted.size() - 1);
    const double frac = idx - static_cast<double>(lo);
    return sorted[lo] * (1.0 - frac) + sorted[hi] * frac;
}

double mean(const std::vector<double> &v) {
    return std::accumulate(v.begin(), v.end(), 0.0) / static_cast<double>(v.size());
}

double stddev(const std::vector<double> &v) {
    const double m = mean(v);
    double sum = 0.0;
    for (double x : v) sum += (x - m) * (x - m);
    return std::sqrt(sum / static_cast<double>(v.size()));
}

void print_separator() {
    std::cout << std::string(80, '-') << "\n";
}

void print_header(const std::string &title) {
    std::cout << "\n";
    print_separator();
    std::cout << "  " << title << "\n";
    print_separator();
}

} // anonymous namespace

json run_fidelity_experiment() {
    print_header("EXPERIMENT 1: Round-Trip Fidelity");

    const std::vector<size_t> turn_counts = {50, 100, 200, 500};
    const std::vector<deer::config::CompressionLevel> levels = {
        deer::config::CompressionLevel::Fast,
        deer::config::CompressionLevel::Balanced,
        deer::config::CompressionLevel::Max,
    };

    json results = json::array();

    std::cout << std::fixed << std::setprecision(6);
    std::cout << std::left << std::setw(8)  << "Turns"
              << std::setw(10) << "Level"
              << std::setw(14) << "MSE"
              << std::setw(14) << "Cosine Sim"
              << std::setw(14) << "Max Abs Err"
              << "\n";
    print_separator();

    for (size_t turns_n : turn_counts) {
        const json turns = deer::bench::generate_filler(turns_n);

        for (auto level : levels) {
            const Eigen::MatrixXd original = build_feature_matrix(turns, level);

            deer::ArchiveState state;
            state.recentTurns = turns;
            {
                SuppressCout guard;
                deer::ops::compressArchiveWithLevel(state, level);
            }

            const Eigen::MatrixXd restored = decompress_to_matrix(state.codedArchive);

            // SVD projection changes basis — compare dimensions that match
            const Eigen::Index rows = std::min(original.rows(), restored.rows());
            const Eigen::Index cols = std::min(original.cols(), restored.cols());
            const Eigen::MatrixXd orig_sub = original.topLeftCorner(rows, cols);
            const Eigen::MatrixXd rest_sub = restored.topLeftCorner(rows, cols);

            const double mse     = compute_mse(orig_sub, rest_sub);
            const double cosine  = compute_cosine_similarity(orig_sub, rest_sub);
            const double max_err = compute_max_abs_error(orig_sub, rest_sub);

            std::cout << std::setw(8) << turns_n
                      << std::setw(10) << level_name(level)
                      << std::setw(14) << mse
                      << std::setw(14) << cosine
                      << std::setw(14) << max_err
                      << "\n";

            results.push_back({
                {"turns", turns_n},
                {"level", level_name(level)},
                {"mse", mse},
                {"cosine_similarity", cosine},
                {"max_abs_error", max_err},
            });
        }
    }

    return results;
}

json run_compression_ratio_experiment() {
    print_header("EXPERIMENT 2: Compression Ratio Analysis");

    const std::vector<size_t> turn_counts = {50, 100, 200, 500};
    const std::vector<deer::config::CompressionLevel> levels = {
        deer::config::CompressionLevel::Fast,
        deer::config::CompressionLevel::Balanced,
        deer::config::CompressionLevel::Max,
    };

    json results = json::array();

    std::cout << std::fixed << std::setprecision(1);
    std::cout << std::left << std::setw(8)  << "Turns"
              << std::setw(10) << "Level"
              << std::setw(12) << "Raw (B)"
              << std::setw(14) << "Deer (B)"
              << std::setw(12) << "Deer %"
              << std::setw(12) << "gzip (B)"
              << std::setw(12) << "gzip %"
              << std::setw(8)  << "Chunks"
              << "\n";
    print_separator();

    for (size_t turns_n : turn_counts) {
        const json turns = deer::bench::generate_filler(turns_n);

        const std::string raw_json = turns.dump();
        const size_t raw_size = raw_json.size();
        const size_t gzip_baseline = gzip_size(raw_json);

        for (auto level : levels) {
            deer::ArchiveState state;
            state.recentTurns = turns;
            {
                SuppressCout guard;
                deer::ops::compressArchiveWithLevel(state, level);
            }

            const size_t deer_size = state.codedArchive.dump().size();
            const size_t num_chunks = (turns_n + deer::config::CHUNK_SIZE - 1) /
                                      deer::config::CHUNK_SIZE;

            const double deer_ratio = 100.0 * (1.0 - static_cast<double>(deer_size) /
                                                          static_cast<double>(raw_size));
            const double gzip_ratio = 100.0 * (1.0 - static_cast<double>(gzip_baseline) /
                                                          static_cast<double>(raw_size));

            std::cout << std::setw(8)  << turns_n
                      << std::setw(10) << level_name(level)
                      << std::setw(12) << raw_size
                      << std::setw(14) << deer_size
                      << std::setw(11) << deer_ratio << "%"
                      << std::setw(12) << gzip_baseline
                      << std::setw(11) << gzip_ratio << "%"
                      << std::setw(8)  << num_chunks
                      << "\n";

            results.push_back({
                {"turns", turns_n},
                {"level", level_name(level)},
                {"raw_bytes", raw_size},
                {"deer_bytes", deer_size},
                {"deer_compression_pct", deer_ratio},
                {"gzip_bytes", gzip_baseline},
                {"gzip_compression_pct", gzip_ratio},
                {"chunks", num_chunks},
                {"features", feature_count(level)},
            });
        }
    }

    return results;
}

json run_latency_experiment() {
    print_header("EXPERIMENT 3: Latency Microbenchmarks (100 iterations)");

    const std::vector<size_t> turn_counts = {50, 100, 200, 500};
    const std::vector<deer::config::CompressionLevel> levels = {
        deer::config::CompressionLevel::Fast,
        deer::config::CompressionLevel::Balanced,
        deer::config::CompressionLevel::Max,
    };

    constexpr size_t ITERATIONS = 100;
    json results = json::array();

    // Compress latency table
    std::cout << "  Compress Latency (μs)\n\n";
    std::cout << std::fixed << std::setprecision(1);
    std::cout << std::left << std::setw(8) << "Turns"
              << std::setw(10) << "Level"
              << std::setw(10) << "Mean"
              << std::setw(10) << "P50"
              << std::setw(10) << "P95"
              << std::setw(10) << "P99"
              << std::setw(10) << "Stddev"
              << "\n";
    print_separator();

    for (size_t turns_n : turn_counts) {
        const json turns = deer::bench::generate_filler(turns_n);

        for (auto level : levels) {
            std::vector<double> compress_times;
            std::vector<double> decompress_times;

            for (size_t iter = 0; iter < ITERATIONS; ++iter) {
                deer::ArchiveState state;
                state.recentTurns = turns;

                auto t0 = std::chrono::high_resolution_clock::now();
                {
                    SuppressCout guard;
                    deer::ops::compressArchiveWithLevel(state, level);
                }
                auto t1 = std::chrono::high_resolution_clock::now();
                compress_times.push_back(
                    std::chrono::duration<double, std::micro>(t1 - t0).count());

                auto t2 = std::chrono::high_resolution_clock::now();
                {
                    SuppressCout guard;
                    deer::ops::decompressArchive(state);
                }
                auto t3 = std::chrono::high_resolution_clock::now();
                decompress_times.push_back(
                    std::chrono::duration<double, std::micro>(t3 - t2).count());
            }

            std::sort(compress_times.begin(), compress_times.end());
            std::sort(decompress_times.begin(), decompress_times.end());

            std::cout << std::setw(8)  << turns_n
                      << std::setw(10) << level_name(level)
                      << std::setw(10) << mean(compress_times)
                      << std::setw(10) << percentile(compress_times, 50)
                      << std::setw(10) << percentile(compress_times, 95)
                      << std::setw(10) << percentile(compress_times, 99)
                      << std::setw(10) << stddev(compress_times)
                      << "\n";

            results.push_back({
                {"turns", turns_n},
                {"level", level_name(level)},
                {"compress_us", {
                    {"mean", mean(compress_times)},
                    {"p50", percentile(compress_times, 50)},
                    {"p95", percentile(compress_times, 95)},
                    {"p99", percentile(compress_times, 99)},
                    {"stddev", stddev(compress_times)},
                }},
                {"decompress_us", {
                    {"mean", mean(decompress_times)},
                    {"p50", percentile(decompress_times, 50)},
                    {"p95", percentile(decompress_times, 95)},
                    {"p99", percentile(decompress_times, 99)},
                    {"stddev", stddev(decompress_times)},
                }},
            });
        }
    }

    // Decompress latency table
    std::cout << "\n  Decompress Latency (μs)\n\n";
    std::cout << std::left << std::setw(8) << "Turns"
              << std::setw(10) << "Level"
              << std::setw(10) << "Mean"
              << std::setw(10) << "P50"
              << std::setw(10) << "P95"
              << std::setw(10) << "P99"
              << std::setw(10) << "Stddev"
              << "\n";
    print_separator();

    for (const auto &r : results) {
        const auto &d = r["decompress_us"];
        std::cout << std::setw(8)  << r["turns"].get<size_t>()
                  << std::setw(10) << r["level"].get<std::string>()
                  << std::setw(10) << d["mean"].get<double>()
                  << std::setw(10) << d["p50"].get<double>()
                  << std::setw(10) << d["p95"].get<double>()
                  << std::setw(10) << d["p99"].get<double>()
                  << std::setw(10) << d["stddev"].get<double>()
                  << "\n";
    }

    return results;
}

json run_golden_fact_experiment() {
    print_header("EXPERIMENT 4: Golden Fact Signal Retention");

    const std::vector<deer::config::CompressionLevel> levels = {
        deer::config::CompressionLevel::Fast,
        deer::config::CompressionLevel::Balanced,
        deer::config::CompressionLevel::Max,
    };

    constexpr size_t NUM_TURNS = 200;
    const auto &facts = deer::bench::DEFAULT_GOLDEN_FACTS;

    auto [turns, fact_indices] = deer::bench::generate_with_golden_facts(NUM_TURNS, facts);

    // Determine which chunks contain golden facts
    std::set<size_t> fact_chunks;
    for (size_t idx : fact_indices) {
        fact_chunks.insert(idx / deer::config::CHUNK_SIZE);
    }

    json results = json::array();

    std::cout << "  200 turns, 20 golden facts, chunk_size=" << deer::config::CHUNK_SIZE << "\n";
    std::cout << "  Fact chunks: {";
    bool first = true;
    for (size_t c : fact_chunks) {
        if (!first) std::cout << ", ";
        std::cout << c;
        first = false;
    }
    std::cout << "}\n\n";

    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::left
              << std::setw(10) << "Level"
              << std::setw(20) << "Fact Norm (μ±σ)"
              << std::setw(20) << "Filler Norm (μ±σ)"
              << std::setw(14) << "Separation"
              << std::setw(16) << "Detection %"
              << "\n";
    print_separator();

    for (auto level : levels) {
        // Build original feature matrix
        const Eigen::MatrixXd original = build_feature_matrix(turns, level);

        // Compress + decompress
        deer::ArchiveState state;
        state.recentTurns = turns;
        {
            SuppressCout guard;
            deer::ops::compressArchiveWithLevel(state, level);
        }
        const Eigen::MatrixXd restored = decompress_to_matrix(state.codedArchive);

        // Compute per-chunk L2 norms of the restored matrix
        std::vector<double> fact_norms, filler_norms;
        for (Eigen::Index i = 0; i < restored.rows(); ++i) {
            const double norm = restored.row(i).norm();
            if (fact_chunks.count(static_cast<size_t>(i))) {
                fact_norms.push_back(norm);
            } else {
                filler_norms.push_back(norm);
            }
        }

        const double fact_mean   = mean(fact_norms);
        const double fact_std    = stddev(fact_norms);
        const double filler_mean = mean(filler_norms);
        const double filler_std  = stddev(filler_norms);

        // Compute median filler norm as threshold
        std::vector<double> all_norms;
        for (Eigen::Index i = 0; i < restored.rows(); ++i) {
            all_norms.push_back(restored.row(i).norm());
        }
        std::sort(all_norms.begin(), all_norms.end());
        const double median_norm = percentile(all_norms, 50.0);

        // Detection: % of fact chunks with norm above median
        size_t detected = 0;
        for (double fn : fact_norms) {
            if (fn >= median_norm) ++detected;
        }
        const double detection_rate = 100.0 * static_cast<double>(detected) /
                                      static_cast<double>(fact_norms.size());

        // Cohen's d separation metric
        const double pooled_std = std::sqrt((fact_std * fact_std + filler_std * filler_std) / 2.0);
        const double cohens_d = (pooled_std > 1e-12) ?
            std::abs(fact_mean - filler_mean) / pooled_std : 0.0;

        std::ostringstream fact_str, filler_str;
        fact_str   << std::fixed << std::setprecision(4) << fact_mean << "±" << fact_std;
        filler_str << std::fixed << std::setprecision(4) << filler_mean << "±" << filler_std;

        std::cout << std::setw(10) << level_name(level)
                  << std::setw(20) << fact_str.str()
                  << std::setw(20) << filler_str.str()
                  << std::setw(14) << cohens_d
                  << std::setw(15) << detection_rate << "%"
                  << "\n";

        results.push_back({
            {"level", level_name(level)},
            {"fact_norm_mean", fact_mean},
            {"fact_norm_std", fact_std},
            {"filler_norm_mean", filler_mean},
            {"filler_norm_std", filler_std},
            {"cohens_d", cohens_d},
            {"detection_rate_pct", detection_rate},
            {"fact_chunks", fact_chunks.size()},
            {"filler_chunks", restored.rows() - static_cast<Eigen::Index>(fact_chunks.size())},
        });
    }

    return results;
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║       Deer v0.6 — Evaluation & Benchmarking Suite          ║\n";
    std::cout << "║       KVTC/TurboQuant Compression Pipeline                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";

    json all_results;

    all_results["fidelity"]    = run_fidelity_experiment();
    all_results["compression"] = run_compression_ratio_experiment();
    all_results["latency"]     = run_latency_experiment();
    all_results["golden_fact"] = run_golden_fact_experiment();

    // Write machine-readable results
    std::filesystem::create_directories("benchmarks/results");
    std::ofstream out("benchmarks/results/bench_results.json");
    out << all_results.dump(2);
    out.close();

    print_header("COMPLETE");
    std::cout << "  Results written to benchmarks/results/bench_results.json\n";
    std::cout << "  Run: python3 benchmarks/report.py\n\n";

    return 0;
}
