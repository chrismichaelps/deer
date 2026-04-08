#pragma once
#include <deer/layer.h>
#include <deer/types/cli_service.h>
#include <deer/types/compress_service.h>
#include <deer/types/logger.h>
#include <deer/types/storage.h>
#include <iostream>

namespace deer::layer {

/**
 * Constructs the CliService using its concrete dependencies.
 */
inline Layer<CliService, LoggerService, StorageService, CompressService> make_cli_layer() {
  return Layer<CliService, LoggerService, StorageService, CompressService>{
      [](const LoggerService &logger, const StorageService &storage,
         const CompressService &compress) -> CliService {
        return CliService{
            [](const ArchiveState &state) {
              json payload = {
                  {"system", "You are resuming from the Deer memory archive."},
                  {"archive",
                   {{"coded", state.codedArchive},
                    {"slots", state.memorySlots},
                    {"recent", state.recentTurns}}}};
              std::cout << payload.dump(2) << "\n";
            },
            [logger](ArchiveState &state) {
              std::string input((std::istreambuf_iterator<char>(std::cin)), {});
              if (!input.empty() && input.back() == '\n')
                input.pop_back();
              if (!input.empty()) {
                state.recentTurns.push_back({{"role", "user"}, {"content", input}});
                logger.log("Turn added.");
              }
            },
            []() { std::cout << "Usage: deer [resume | add | compress]\n"; }};
      }};
}

} // namespace deer::layer
