#pragma once
#include <cstddef>
#include <string>

namespace deer::config {

/// Default path for the archive JSON file.
inline const char *ARCHIVE_PATH = "deer_archive.json";

/// Number of recent conversation turns to keep uncompressed.
constexpr size_t MAX_RECENT_TURNS = 30;

/// Number of turns grouped into a single compression chunk.
constexpr size_t CHUNK_SIZE = 8;

/// Target number of memory slots for recall.
constexpr size_t TARGET_SLOTS = 25;

/// Number of bits for integer quantization.
constexpr int QUANT_BITS = 4;

/**
 * Supported compression effort levels.
 */
enum class CompressionLevel : uint8_t {
  Fast,     ///< Optimized for speed, fewer features.
  Balanced, ///< Recommended tradeoff between speed and recall.
  Max       ///< Maximum feature extraction, slower but more robust.
};

/// Default compression level used if none is specified.
constexpr CompressionLevel DEFAULT_COMPRESSION_LEVEL = CompressionLevel::Balanced;

/// Feature dimensionality for the 'Fast' compression level.
constexpr size_t FEATURE_COUNT_FAST = 12;

/// Feature dimensionality for the 'Balanced' compression level.
constexpr size_t FEATURE_COUNT_BALANCED = 20;

/// Feature dimensionality for the 'Max' compression level.
constexpr size_t FEATURE_COUNT_MAX = 28;

/// Rough estimate of characters per token for heuristic counting.
constexpr size_t MAX_TOKEN_ESTIMATE = 50000;

/// Normalization denominator for text length features.
constexpr double NORMALIZE_LENGTH_DENOM = 20000.0;

/// Normalization denominator for word count features.
constexpr double NORMALIZE_WORD_DENOM = 3000.0;

/// Scaling factor for global quantization.
constexpr double QUANTIZATION_RANGE = 15.0;

/// Minimum global quantization value.
constexpr int QUANTIZATION_MIN = -8;

/// Maximum global quantization value.
constexpr int QUANTIZATION_MAX = 7;

/// Identifier for the current compression implementation.
inline const char *COMPRESSION_METHOD = "deer_v0.6_research";

/// CLI matching string for the Fast level.
inline const char *LEVEL_FAST = "fast";

/// CLI matching string for the Balanced level.
inline const char *LEVEL_BALANCED = "balanced";

/// CLI matching string for the Max level.
inline const char *LEVEL_MAX = "max";

/// CLI argument switch for the Fast level.
inline const char *CMD_ARG_FAST = "--fast";

/// CLI argument switch for the Max level.
inline const char *CMD_ARG_MAX = "--max";

/// CLI argument switch for the Balanced level.
inline const char *CMD_ARG_BALANCED = "--balanced";

/// CLI command string for the resume command.
inline const char *CMD_RESUME = "resume";

/// CLI command string for the add command.
inline const char *CMD_ADD = "add";

/// CLI command string for the compress command.
inline const char *CMD_COMPRESS = "compress";

/// CLI command string for the decompress command.
inline const char *CMD_DECOMPRESS = "decompress";

/// CLI command string for the help command.
inline const char *CMD_HELP = "help";

/**
 * Service configuration schema containing settings.
 */
struct ConfigService {
  std::string      archivePath      = ARCHIVE_PATH;
  std::size_t      maxRecentTurns   = MAX_RECENT_TURNS;
  std::size_t      chunkSize        = CHUNK_SIZE;
  std::size_t      targetSlots      = TARGET_SLOTS;
  int              quantBits        = QUANT_BITS;
  CompressionLevel defaultLevel     = DEFAULT_COMPRESSION_LEVEL;
  std::size_t      featureCountFast = FEATURE_COUNT_FAST;
  std::size_t      featureCountBal  = FEATURE_COUNT_BALANCED;
  std::size_t      featureCountMax  = FEATURE_COUNT_MAX;
  std::size_t      maxTokenEstimate = MAX_TOKEN_ESTIMATE;
  double           normLengthDenom  = NORMALIZE_LENGTH_DENOM;
  double           normWordDenom    = NORMALIZE_WORD_DENOM;
  double           quantRange       = QUANTIZATION_RANGE;
  int              quantMin         = QUANTIZATION_MIN;
  int              quantMax         = QUANTIZATION_MAX;
  std::string      method           = COMPRESSION_METHOD;
  std::string      logLevel         = "INFO";
};

} // namespace deer::config