#if !defined(__TUNES_H__)
#define __TUNES_H__
#pragma once

#include <filesystem>
#include <optional>
#include <string>

std::optional<std::filesystem::path> get_tune(const std::string& path);

#endif