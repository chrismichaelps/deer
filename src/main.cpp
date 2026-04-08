#include <cstring>
#include <deer/layers/cli_layer.h>
#include <deer/layers/compress_layer.h>
#include <deer/layers/config_layer.h>
#include <deer/layers/logger_layer.h>
#include <deer/layers/storage_layer.h>
#include <deer/types/version.h>
#include <iostream>

/**
 * Main entry point for the Deer CLI.
 */
int main(int argc, char **argv) {
  std::cout << deer::FULL_NAME << " v" << deer::VERSION << "\n\n";

  /**
   * Initializes the Dependency Injection Layer Graph.
   */
  const auto config   = deer::layer::make_config_layer().build();
  const auto logger   = deer::layer::make_logger_layer().build(config);
  const auto storage  = deer::layer::make_storage_layer().build(config);
  const auto compress = deer::layer::make_compress_layer().build(config);
  const auto cli      = deer::layer::make_cli_layer().build(logger, storage, compress);

  /**
   * Executes the appropriate CLI command based on arguments.
   */
  auto state = storage.load();
  if (state.recentTurns.empty() && state.memorySlots.empty()) {
    logger.log("[Deer] New archive initialized.");
  }

  const std::string cmd = (argc > 1) ? argv[1] : deer::config::CMD_HELP;

  if (cmd == deer::config::CMD_RESUME) {
    cli.resume(state);
  } else if (cmd == deer::config::CMD_ADD) {
    cli.add(state);
    storage.save(state);
  } else if (cmd == deer::config::CMD_COMPRESS) {
    deer::config::CompressionLevel level = config.defaultLevel;
    if (argc > 2) {
      if (std::strcmp(argv[2], deer::config::CMD_ARG_FAST) == 0) {
        level = deer::config::CompressionLevel::Fast;
      } else if (std::strcmp(argv[2], deer::config::CMD_ARG_MAX) == 0) {
        level = deer::config::CompressionLevel::Max;
      } else if (std::strcmp(argv[2], deer::config::CMD_ARG_BALANCED) == 0) {
        level = deer::config::CompressionLevel::Balanced;
      }
    }
    compress.compress(state, level);
    storage.save(state);
  } else if (cmd == deer::config::CMD_DECOMPRESS) {
    compress.decompress(state);
  } else {
    cli.help();
  }

  return 0;
}