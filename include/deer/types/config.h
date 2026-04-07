#pragma once
#include <cstddef>
#include <string>

namespace deer::config {

/** @deer.config.ArchivePath.Constant */
inline const char *ARCHIVE_PATH = "deer_archive.json";

/** @deer.config.MaxRecentTurns.Constant */
constexpr size_t MAX_RECENT_TURNS = 30;

/** @deer.config.ChunkSize.Constant */
constexpr size_t CHUNK_SIZE = 8;

/** @deer.config.TargetSlots.Constant */
constexpr size_t TARGET_SLOTS = 25;

/** @deer.config.QuantBits.Constant */
constexpr int QUANT_BITS = 4;

/** @deer.config.CompressionLevel.Enum */
enum class CompressionLevel : uint8_t {
  Fast,     /** @deer.config.CompressionLevel.Fast */
  Balanced, /** @deer.config.CompressionLevel.Balanced */
  Max       /** @deer.config.CompressionLevel.Max */
};

/** @deer.config.DefaultCompressionLevel.Constant */
constexpr CompressionLevel DEFAULT_COMPRESSION_LEVEL = CompressionLevel::Balanced;

/** @deer.config.FeatureCount.Fast */
constexpr size_t FEATURE_COUNT_FAST = 12;

/** @deer.config.FeatureCount.Balanced */
constexpr size_t FEATURE_COUNT_BALANCED = 20;

/** @deer.config.FeatureCount.Max */
constexpr size_t FEATURE_COUNT_MAX = 28;

/** @deer.config.MaxTokenEstimate.Constant */
constexpr size_t MAX_TOKEN_ESTIMATE = 50000;

/** @deer.config.NormalizeLength.Denominator */
constexpr double NORMALIZE_LENGTH_DENOM = 20000.0;

/** @deer.config.NormalizeWord.Denominator */
constexpr double NORMALIZE_WORD_DENOM = 3000.0;

/** @deer.config.QuantizationRange.Constant */
constexpr double QUANTIZATION_RANGE = 15.0;

/** @deer.config.QuantizationMin.Constant */
constexpr int QUANTIZATION_MIN = -8;

/** @deer.config.QuantizationMax.Constant */
constexpr int QUANTIZATION_MAX = 7;

/** @deer.config.MethodVersion.Constant */
inline const char *COMPRESSION_METHOD = "deer_v0.5_optimized";

/** @deer.config.LevelFast.String */
inline const char *LEVEL_FAST = "fast";

/** @deer.config.LevelBalanced.String */
inline const char *LEVEL_BALANCED = "balanced";

/** @deer.config.LevelMax.String */
inline const char *LEVEL_MAX = "max";

/** @deer.config.CmdArg.Fast */
inline const char *CMD_ARG_FAST = "--fast";

/** @deer.config.CmdArg.Max */
inline const char *CMD_ARG_MAX = "--max";

/** @deer.config.CmdArg.Balanced */
inline const char *CMD_ARG_BALANCED = "--balanced";

/** @deer.config.CmdResume.Constant */
inline const char *CMD_RESUME = "resume";

/** @deer.config.CmdAdd.Constant */
inline const char *CMD_ADD = "add";

/** @deer.config.CmdCompress.Constant */
inline const char *CMD_COMPRESS = "compress";

/** @deer.config.CmdDecompress.Constant */
inline const char *CMD_DECOMPRESS = "decompress";

/** @deer.config.CmdHelp.Constant */
inline const char *CMD_HELP = "help";

} // namespace deer::config