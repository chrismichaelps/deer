#pragma once
#include <Eigen/Dense>
#include <algorithm>
#include <chrono>
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <zlib.h>

namespace deer::ops {

/** @deer.ops.compressArchiveWithLevel */
inline void compressArchiveWithLevel(ArchiveState &state, config::CompressionLevel level);

/** @deer.ops.compressArchive */
inline void compressArchive(ArchiveState &state) {
  compressArchiveWithLevel(state, config::DEFAULT_COMPRESSION_LEVEL);
}

/** @deer.ops.decompressArchive */
inline void decompressArchive(ArchiveState &state);

/// @brief Decodes __deflate back to compressed feature matrix for LLM context
/// @deer.ops.decompressArchive
inline void decompressArchive(ArchiveState &state) {
  if (!state.codedArchive.is_object() || !state.codedArchive.contains("__deflate")) {
    std::cout << "[Deer] No compressed archive found.\n";
    return;
  }

  auto start = std::chrono::high_resolution_clock::now();

  const std::string &hexData = state.codedArchive["__deflate"].get<std::string>();
  const size_t compressedSize = hexData.length() / 2;

  std::vector<Bytef> compressed(compressedSize);
  for (size_t i = 0; i < compressedSize; ++i) {
    std::string byteStr = hexData.substr(i * 2, 2);
    compressed[i] = static_cast<Bytef>(std::stoi(byteStr, nullptr, 16));
  }

  std::vector<Bytef> decompressed(state.codedArchive["original_size"].get<size_t>());
  uLongf destLen = decompressed.size();
  int result = ::uncompress(decompressed.data(), &destLen, compressed.data(), compressedSize);

  if (result != Z_OK) {
    std::cout << "[Deer] Decompression failed: " << result << "\n";
    return;
  }

  std::string jsonStr(reinterpret_cast<char *>(decompressed.data()), destLen);
  json decoded = json::parse(jsonStr);

  Eigen::MatrixXd restored(decoded.size(), decoded[0].size());
  for (size_t i = 0; i < decoded.size(); ++i) {
    for (size_t j = 0; j < decoded[i].size(); ++j) {
      restored(i, j) = decoded[i][j].get<int>() / config::QUANTIZATION_RANGE;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  double durationMs = std::chrono::duration<double, std::milli>(end - start).count();

  std::cout << "[Deer] Decompression completed in " << durationMs << " ms\n";
  std::cout << "   Restored " << restored.rows() << " chunks with " << restored.cols()
            << " features each\n";
  std::cout << "   Original size: " << state.codedArchive["original_size"] << " bytes\n";
  std::cout << "   Compressed size: " << state.codedArchive["compressed_size"] << " bytes\n";
}

/** @deer.ops.compressArchiveWithLevel */
inline void compressArchiveWithLevel(ArchiveState &state, config::CompressionLevel level) {
  if (state.recentTurns.empty()) {
    std::cout << "[Deer] Nothing to compress.\n";
    return;
  }

  auto start = std::chrono::high_resolution_clock::now();

  const char *levelStr = (level == config::CompressionLevel::Fast)  ? config::LEVEL_FAST
                         : (level == config::CompressionLevel::Max) ? config::LEVEL_MAX
                                                                    : config::LEVEL_BALANCED;

  std::cout << "[Deer] Starting v0.5 optimized compression (level: " << levelStr << ")...\n";

  const size_t chunkSize = config::CHUNK_SIZE;
  const size_t numChunks = (state.recentTurns.size() + chunkSize - 1) / chunkSize;

  const size_t numFeatures = (level == config::CompressionLevel::Fast) ? config::FEATURE_COUNT_FAST
                             : (level == config::CompressionLevel::Max)
                                 ? config::FEATURE_COUNT_MAX
                                 : config::FEATURE_COUNT_BALANCED;

  Eigen::MatrixXd feats(numChunks, numFeatures);
  feats.setZero();

  size_t totalTokens = 0;

  for (size_t chunkIdx = 0; chunkIdx < numChunks; ++chunkIdx) {
    std::ostringstream oss;
    size_t startIdx = chunkIdx * chunkSize;
    size_t endIdx = std::min(startIdx + chunkSize, state.recentTurns.size());

    for (size_t i = startIdx; i < endIdx; ++i) {
      if (state.recentTurns[i].contains("content") && state.recentTurns[i]["content"].is_string()) {
        const std::string &content = state.recentTurns[i]["content"].get<std::string>();
        oss << content << "\n";

        size_t chars = content.length();
        size_t spaces = std::count(content.begin(), content.end(), ' ');
        size_t newlines = std::count(content.begin(), content.end(), '\n');
        totalTokens += (chars / 4) + (spaces / 2) + newlines + 8;
      }
    }

    const std::string chunkText = oss.str();

    feats(chunkIdx, 0) = static_cast<double>(chunkText.length()) / config::NORMALIZE_LENGTH_DENOM;
    feats(chunkIdx, 1) = static_cast<double>(std::count(chunkText.begin(), chunkText.end(), ' ')) /
                         config::NORMALIZE_WORD_DENOM;
    feats(chunkIdx, 2) = static_cast<double>(chunkText.find("code") != std::string::npos);
    feats(chunkIdx, 3) = static_cast<double>(chunkText.find("tool") != std::string::npos ||
                                             chunkText.find("function") != std::string::npos);
    feats(chunkIdx, 4) = static_cast<double>(chunkText.find("decision") != std::string::npos);

    if (level != config::CompressionLevel::Fast) {
      std::string deduped = chunkText;
      deduped.erase(std::unique(deduped.begin(), deduped.end()), deduped.end());
      feats(chunkIdx, 5) =
          static_cast<double>(deduped.size()) / std::max(chunkText.size(), size_t(1));
    }
  }

  state.tokenEstimate = totalTokens;

  Eigen::JacobiSVD<Eigen::MatrixXd> svd(feats, Eigen::ComputeFullV);
  Eigen::MatrixXd coeffs = feats * svd.matrixV();

  Eigen::MatrixXd R =
      Eigen::MatrixXd::Random(numFeatures, numFeatures).householderQr().householderQ();
  Eigen::MatrixXd rotated = coeffs * R;

  json coded;
  for (Eigen::Index i = 0; i < rotated.rows(); ++i) {
    json row;
    for (Eigen::Index j = 0; j < rotated.cols(); ++j) {
      int q = static_cast<int>(std::round(rotated(i, j) * config::QUANTIZATION_RANGE));
      q = std::clamp(q, config::QUANTIZATION_MIN, config::QUANTIZATION_MAX);
      row.push_back(q);
    }
    coded.push_back(std::move(row));
  }

  std::string raw = coded.dump();
  uLongf destLen = compressBound(raw.size());
  std::vector<Bytef> compressed(destLen);
  ::compress(compressed.data(), &destLen, reinterpret_cast<const Bytef *>(raw.data()), raw.size());

  std::stringstream hexStream;
  hexStream << std::hex << std::setfill('0');
  for (size_t i = 0; i < destLen; ++i) {
    hexStream << std::setw(2) << static_cast<int>(compressed[i]);
  }

  json archiveData;
  if (state.codedArchive.is_object() && !state.codedArchive.empty()) {
    archiveData = state.codedArchive;
  } else {
    archiveData = json::object();
  }

  archiveData["__deflate"] = hexStream.str();
  archiveData["original_size"] = raw.size();
  archiveData["compressed_size"] = static_cast<size_t>(destLen);
  archiveData["encoding"] = "hex";
  archiveData["method"] = config::COMPRESSION_METHOD;
  archiveData["chunks"] = numChunks;
  archiveData["level"] = levelStr;

  state.codedArchive = std::move(archiveData);

  if (state.recentTurns.size() > config::MAX_RECENT_TURNS) {
    json trimmed;
    for (size_t i = state.recentTurns.size() - config::MAX_RECENT_TURNS;
         i < state.recentTurns.size(); ++i) {
      trimmed.push_back(state.recentTurns[i]);
    }
    state.recentTurns = std::move(trimmed);
  }

  auto end = std::chrono::high_resolution_clock::now();
  double durationMs = std::chrono::duration<double, std::milli>(end - start).count();

  const size_t compressedSize = state.codedArchive.dump().size();

  std::cout << "[Deer] Compression completed in " << durationMs << " ms\n";
  std::cout << "   Estimated tokens : " << state.tokenEstimate << " (~"
            << (state.tokenEstimate / 1000) << "k)\n";
  std::cout << "   Final archive size: ~" << (compressedSize / 1024) << " KB\n";
  std::cout << "   __deflate encoding: hex\n";
}

} // namespace deer::ops