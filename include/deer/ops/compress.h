#pragma once
#include <Eigen/Dense>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <zlib.h>

namespace deer::ops {

/**
 * Fills the core five features into the target matrix row.
 */
inline void fill_core_five(Eigen::MatrixXd &feats, Eigen::Index row,
                            const std::string &text, size_t text_len, size_t num_words) {
  feats(row, 0) = static_cast<double>(text_len)  / config::NORMALIZE_LENGTH_DENOM;
  feats(row, 1) = static_cast<double>(num_words)  / config::NORMALIZE_WORD_DENOM;
  feats(row, 2) = static_cast<double>(text.find("code")     != std::string::npos);
  feats(row, 3) = static_cast<double>(text.find("tool")     != std::string::npos ||
                                      text.find("function") != std::string::npos);
  feats(row, 4) = static_cast<double>(text.find("decision") != std::string::npos);
}

/**
 * Fills the extra features used in Fast compression mode.
 */
inline void fill_fast_extra(Eigen::MatrixXd &feats, Eigen::Index row,
                             const std::string &text, size_t text_len, size_t num_words) {
  const double dlen = std::max(static_cast<double>(text_len), 1.0);

  const size_t digits = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c))); }));
  feats(row, 5) = static_cast<double>(digits) / dlen;

  feats(row, 6) = static_cast<double>(num_words > 10);

  feats(row, 7) = static_cast<double>(text.find("error") != std::string::npos ||
                                      text.find("bug")   != std::string::npos);

  feats(row, 8) = static_cast<double>(std::count(text.begin(), text.end(), '?') > 0);

  feats(row, 9) = static_cast<double>(text.find("http") != std::string::npos ||
                                      text.find("www.")  != std::string::npos);

  const size_t newlines = static_cast<size_t>(std::count(text.begin(), text.end(), '\n'));
  feats(row, 10) = static_cast<double>(newlines) / dlen;

  const double avg_word_len =
      (num_words > 0) ? static_cast<double>(text_len) / static_cast<double>(num_words) : 0.0;
  feats(row, 11) = avg_word_len / 12.0;
}

/**
 * Fills the rich balanced features used in Balanced compression mode.
 */
inline void fill_balanced_rich(Eigen::MatrixXd &feats, Eigen::Index row,
                                const std::string &text, size_t text_len, size_t num_words) {
  const double dlen   = std::max(static_cast<double>(text_len), 1.0);
  const double dwords = std::max(static_cast<double>(num_words), 1.0);

  std::string deduped = text;
  deduped.erase(std::unique(deduped.begin(), deduped.end()), deduped.end());
  feats(row, 5) = static_cast<double>(deduped.size()) / dlen;

  feats(row, 6) = (static_cast<double>(text_len) / dwords) / 12.0;

  const size_t punct = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return c == '.' || c == ',' || c == ';' || c == ':'; }));
  feats(row, 7) = static_cast<double>(punct) / dlen;

  const size_t upper = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return static_cast<bool>(std::isupper(static_cast<unsigned char>(c))); }));
  feats(row, 8) = static_cast<double>(upper) / dlen;

  const size_t sentences = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return c == '.' || c == '!' || c == '?'; }));
  feats(row, 9) = static_cast<double>(sentences) / dwords;

  const size_t questions = static_cast<size_t>(std::count(text.begin(), text.end(), '?'));
  feats(row, 10) = static_cast<double>(questions) / std::max(static_cast<double>(sentences) + 1.0, 1.0);
}

/**
 * Fills the extra balanced features used in Balanced compression mode.
 */
inline void fill_balanced_extra(Eigen::MatrixXd &feats, Eigen::Index row,
                                 const std::string &text, size_t text_len, size_t num_words) {
  const double dlen   = std::max(static_cast<double>(text_len), 1.0);
  const double dwords = std::max(static_cast<double>(num_words), 1.0);

  const size_t digits = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c))); }));
  feats(row, 11) = static_cast<double>(digits) / dlen;

  feats(row, 12) = static_cast<double>(num_words > 10);

  feats(row, 13) = static_cast<double>(text.find("error") != std::string::npos ||
                                       text.find("bug")   != std::string::npos);

  feats(row, 14) = static_cast<double>(text.find("http") != std::string::npos ||
                                       text.find("www.")  != std::string::npos);

  const size_t newlines = static_cast<size_t>(std::count(text.begin(), text.end(), '\n'));
  feats(row, 15) = static_cast<double>(newlines) / dlen;

  const size_t brackets = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']'; }));
  feats(row, 16) = static_cast<double>(brackets) / dlen;

  feats(row, 17) = static_cast<double>(text.find("important") != std::string::npos ||
                                       text.find("critical")  != std::string::npos ||
                                       text.find("urgent")    != std::string::npos);

  feats(row, 18) = static_cast<double>(text.find("TODO") != std::string::npos ||
                                       text.find("FIXME") != std::string::npos ||
                                       text.find("NOTE")  != std::string::npos);

  const size_t excl = static_cast<size_t>(std::count(text.begin(), text.end(), '!'));
  feats(row, 19) = static_cast<double>(excl) / dwords;
}

/**
 * Fills the maximal features used in Max compression mode.
 */
inline void fill_max_extra(Eigen::MatrixXd &feats, Eigen::Index row,
                            const std::string &text, size_t text_len, size_t num_words) {
  const double dlen   = std::max(static_cast<double>(text_len), 1.0);
  const double dwords = std::max(static_cast<double>(num_words), 1.0);

  const size_t sentences = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return c == '.' || c == '!' || c == '?'; }));
  const double dsentences = std::max(static_cast<double>(sentences), 1.0);
  feats(row, 20) = static_cast<double>(text_len) / (dsentences * 80.0);

  const size_t backticks = static_cast<size_t>(std::count(text.begin(), text.end(), '`'));
  feats(row, 21) = static_cast<double>(backticks) / dlen;

  feats(row, 22) = static_cast<double>(text.find("_") != std::string::npos ||
                                       (text.find("::") != std::string::npos));

  const size_t quotes = static_cast<size_t>(std::count(text.begin(), text.end(), '"'));
  feats(row, 23) = static_cast<double>(quotes) / dlen;

  const size_t specials = static_cast<size_t>(
      std::count_if(text.begin(), text.end(),
                    [](char c) { return c == '@' || c == '#' || c == '$'; }));
  feats(row, 24) = static_cast<double>(specials) / dlen;

  size_t long_words = 0;
  std::istringstream iss(text);
  std::string word;
  while (iss >> word) {
    if (word.length() > 8) {
      ++long_words;
    }
  }
  feats(row, 25) = static_cast<double>(long_words) / dwords;

  const size_t semicolons = static_cast<size_t>(std::count(text.begin(), text.end(), ';'));
  feats(row, 26) = static_cast<double>(semicolons) / dlen;

  feats(row, 27) = static_cast<double>(text.find("return") != std::string::npos ||
                                       text.find("class")  != std::string::npos ||
                                       text.find("struct") != std::string::npos ||
                                       text.find("def ")   != std::string::npos);
}

/**
 * Computes chunk features recursively for the given sequence of turns.
 */
inline size_t fill_chunk_features(Eigen::MatrixXd &feats, const json &turns,
                                  size_t chunk_idx, size_t chunk_size,
                                  config::CompressionLevel level) {
  std::ostringstream oss;
  const size_t start_idx = chunk_idx * chunk_size;
  const size_t end_idx   = std::min(start_idx + chunk_size, turns.size());
  size_t chunk_tokens    = 0;

  for (size_t i = start_idx; i < end_idx; ++i) {
    if (turns[i].contains("content") && turns[i]["content"].is_string()) {
      const std::string &content = turns[i]["content"].get<std::string>();
      oss << content << "\n";
      const size_t chars    = content.length();
      const size_t spaces   = static_cast<size_t>(std::count(content.begin(), content.end(), ' '));
      const size_t newlines = static_cast<size_t>(std::count(content.begin(), content.end(), '\n'));
      chunk_tokens += (chars / 4) + (spaces / 2) + newlines + 8;
    }
  }

  const std::string chunk_text = oss.str();
  if (chunk_text.empty()) {
    return chunk_tokens;
  }

  const size_t text_len  = chunk_text.length();
  const size_t num_words =
      static_cast<size_t>(std::count(chunk_text.begin(), chunk_text.end(), ' ')) + 1;
  const Eigen::Index row = static_cast<Eigen::Index>(chunk_idx);

  fill_core_five(feats, row, chunk_text, text_len, num_words);

  if (level == config::CompressionLevel::Fast) {
    fill_fast_extra(feats, row, chunk_text, text_len, num_words);
  } else {
    fill_balanced_rich(feats, row, chunk_text, text_len, num_words);
    fill_balanced_extra(feats, row, chunk_text, text_len, num_words);
    if (level == config::CompressionLevel::Max) {
      fill_max_extra(feats, row, chunk_text, text_len, num_words);
    }
  }

  return chunk_tokens;
}

/**
 * Reverses the KVTC and TurboQuant pipeline to reconstruct the numeric feature matrix.
 */
inline void decompressArchive(ArchiveState &state) {
  if (!state.codedArchive.is_object() || !state.codedArchive.contains("__deflate")) {
    std::cout << "[Deer] No compressed archive found.\n";
    return;
  }

  const auto start = std::chrono::high_resolution_clock::now();

  const std::string &hex_data  = state.codedArchive["__deflate"].get<std::string>();
  const size_t compressed_size = hex_data.length() / 2;

  std::vector<Bytef> compressed(compressed_size);
  for (size_t i = 0; i < compressed_size; ++i) {
    compressed[i] = static_cast<Bytef>(std::stoi(hex_data.substr(i * 2, 2), nullptr, 16));
  }

  std::vector<Bytef> decompressed(state.codedArchive["original_size"].get<size_t>());
  uLongf dest_len = decompressed.size();
  const int result =
      ::uncompress(decompressed.data(), &dest_len, compressed.data(), compressed_size);

  if (result != Z_OK) {
    std::cout << "[Deer] Decompression failed: " << result << "\n";
    return;
  }

  const std::string json_str(reinterpret_cast<char *>(decompressed.data()), dest_len);
  const json decoded = json::parse(json_str);

  const Eigen::Index num_chunks   = static_cast<Eigen::Index>(decoded.size());
  const Eigen::Index num_features = static_cast<Eigen::Index>(decoded[0].size());

  const bool has_per_channel = state.codedArchive.contains("col_min") &&
                               state.codedArchive.contains("col_max");

  /**
   * Applies per-channel dequantization to interpret variables.
   */
  Eigen::MatrixXd quantized(num_chunks, num_features);
  for (Eigen::Index i = 0; i < num_chunks; ++i) {
    for (Eigen::Index j = 0; j < num_features; ++j) {
      const int q = decoded[i][j].get<int>();
      if (has_per_channel) {
        const double col_min = state.codedArchive["col_min"][j].get<double>();
        const double col_max = state.codedArchive["col_max"][j].get<double>();
        const double scale   = (col_max > col_min) ? (col_max - col_min) : 1.0;
        quantized(i, j)      = col_min + ((static_cast<double>(q) / 15.0) * scale);
      } else {
        quantized(i, j) = static_cast<double>(q) / config::QUANTIZATION_RANGE;
      }
    }
  }

  /**
   * Inverts the randomized orthogonal rotation applied during compression.
   */
  Eigen::MatrixXd restored = quantized;
  if (state.codedArchive.contains("rotation_seed")) {
    const unsigned int seed = state.codedArchive["rotation_seed"].get<unsigned int>();
    std::srand(seed);
    const Eigen::MatrixXd rotation_matrix =
        Eigen::MatrixXd::Random(num_features, num_features).householderQr().householderQ();
    restored = quantized * rotation_matrix.transpose();
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const double duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

  std::cout << "[Deer] Decompression completed in " << duration_ms << " ms\n";
  std::cout << "   Restored " << restored.rows() << " chunks with " << restored.cols()
            << " features each\n";
  std::cout << "   Original size: " << state.codedArchive["original_size"] << " bytes\n";
  std::cout << "   Compressed size: " << state.codedArchive["compressed_size"] << " bytes\n";
  std::cout << "   Rotation inverted: "
            << (state.codedArchive.contains("rotation_seed") ? "yes" : "no (legacy archive)")
            << "\n";
}

/**
 * Compresses an archive state utilizing the KVTC and TurboQuant steps.
 */
inline void compressArchiveWithLevel(ArchiveState &state, config::CompressionLevel level) {
  if (state.recentTurns.empty()) {
    std::cout << "[Deer] Nothing to compress.\n";
    return;
  }

  const auto start = std::chrono::high_resolution_clock::now();

  const char *level_str = config::LEVEL_BALANCED;
  if (level == config::CompressionLevel::Fast) {
    level_str = config::LEVEL_FAST;
  } else if (level == config::CompressionLevel::Max) {
    level_str = config::LEVEL_MAX;
  }

  std::cout << "[Deer] Starting v0.6 optimized compression (level: " << level_str << ")...\n";

  const size_t chunk_size = config::CHUNK_SIZE;
  const size_t num_chunks = (state.recentTurns.size() + chunk_size - 1) / chunk_size;

  size_t num_features = config::FEATURE_COUNT_BALANCED;
  if (level == config::CompressionLevel::Fast) {
    num_features = config::FEATURE_COUNT_FAST;
  } else if (level == config::CompressionLevel::Max) {
    num_features = config::FEATURE_COUNT_MAX;
  }

  Eigen::MatrixXd feats(static_cast<Eigen::Index>(num_chunks),
                        static_cast<Eigen::Index>(num_features));
  feats.setZero();

  size_t total_tokens = 0;

  /**
   * Stage 1: Fast numeric feature extraction layer.
   */
  for (size_t chunk_idx = 0; chunk_idx < num_chunks; ++chunk_idx) {
    total_tokens += fill_chunk_features(feats, state.recentTurns, chunk_idx, chunk_size, level);
  }

  state.tokenEstimate = total_tokens;

  /**
   * Stage 2: SVD-based Principal Component extraction.
   */
  Eigen::JacobiSVD<Eigen::MatrixXd> svd(feats, Eigen::ComputeFullV);
  const Eigen::MatrixXd coeffs = feats * svd.matrixV();

  /**
   * Stage 3: Randomized orthogonal rotation for noise redistribution.
   */
  const unsigned int seed = static_cast<unsigned int>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xFFFFFFFF);
  std::srand(seed);
  const Eigen::MatrixXd rotation_matrix =
      Eigen::MatrixXd::Random(static_cast<Eigen::Index>(num_features),
                              static_cast<Eigen::Index>(num_features))
          .householderQr()
          .householderQ();
  const Eigen::MatrixXd rotated = coeffs * rotation_matrix;

  /**
   * Stage 4: Per-Channel INT4 quantization encoding.
   */
  json col_min_arr = json::array();
  json col_max_arr = json::array();
  json coded;

  for (Eigen::Index j = 0; j < rotated.cols(); ++j) {
    col_min_arr.push_back(rotated.col(j).minCoeff());
    col_max_arr.push_back(rotated.col(j).maxCoeff());
  }

  for (Eigen::Index i = 0; i < rotated.rows(); ++i) {
    json row;
    for (Eigen::Index j = 0; j < rotated.cols(); ++j) {
      const double col_min = col_min_arr[j].get<double>();
      const double col_max = col_max_arr[j].get<double>();
      const double scale   = (col_max > col_min) ? (col_max - col_min) : 1.0;
      const int q = static_cast<int>(std::round((rotated(i, j) - col_min) / scale * 15.0));
      row.push_back(std::clamp(q, 0, 15));
    }
    coded.push_back(std::move(row));
  }

  const std::string raw = coded.dump();
  uLongf dest_len       = compressBound(raw.size());
  std::vector<Bytef> compressed_buf(dest_len);
  ::compress(compressed_buf.data(), &dest_len,
             reinterpret_cast<const Bytef *>(raw.data()), raw.size());

  std::stringstream hex_stream;
  hex_stream << std::hex << std::setfill('0');
  for (size_t i = 0; i < dest_len; ++i) {
    hex_stream << std::setw(2) << static_cast<int>(compressed_buf[i]);
  }

  json archive_data = (state.codedArchive.is_object() && !state.codedArchive.empty())
                          ? state.codedArchive
                          : json::object();

  archive_data["__deflate"]       = hex_stream.str();
  archive_data["original_size"]   = raw.size();
  archive_data["compressed_size"] = static_cast<size_t>(dest_len);
  archive_data["encoding"]        = "hex";
  archive_data["method"]          = config::COMPRESSION_METHOD;
  archive_data["chunks"]          = num_chunks;
  archive_data["level"]           = level_str;
  archive_data["rotation_seed"]   = seed;
  archive_data["col_min"]         = col_min_arr;
  archive_data["col_max"]         = col_max_arr;

  state.codedArchive = std::move(archive_data);

  if (state.recentTurns.size() > config::MAX_RECENT_TURNS) {
    json trimmed;
    for (size_t i = state.recentTurns.size() - config::MAX_RECENT_TURNS;
         i < state.recentTurns.size(); ++i) {
      trimmed.push_back(state.recentTurns[i]);
    }
    state.recentTurns = std::move(trimmed);
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const double duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
  const size_t archive_size = state.codedArchive.dump().size();

  std::cout << "[Deer] Compression completed in " << duration_ms << " ms\n";
  std::cout << "   Estimated tokens : " << state.tokenEstimate << " (~"
            << (state.tokenEstimate / 1000) << "k)\n";
  std::cout << "   Final archive size: ~" << (archive_size / 1024) << " KB\n";
  std::cout << "   Features: " << num_features << " | Quant: per-channel INT4 | Rotation: seeded\n";
}

} // namespace deer::ops