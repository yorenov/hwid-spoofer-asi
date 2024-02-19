#pragma once

#include <optional>
#include <string>
#include <filesystem>

std::filesystem::path GetProjectDirectory(std::optional<std::filesystem::path> append = std::nullopt);