#pragma once

#include "types.hpp"

#include <iosfwd>

struct representation;

auto explode_path(const fs::path& path, const std::string& extended_path) -> representation;
auto split_string_to_rep(const std::wstring& str, const std::string& extended_path) -> representation;
