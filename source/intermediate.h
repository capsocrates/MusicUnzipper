#pragma once

#include "representation.h"

#include "types.hpp"

#include <set>

struct intermediate_data
{
    std::set<fs::path> ignored_files;
    rep_set parsed_filenames;
    rep_set ready_files;
    rep_set processed_files;
};

auto read(const fs::path& directory_path, const std::string& extended_path, const std::wstring& intermediate_path) -> intermediate_data;

auto write(
    const std::wstring& intermediate_path
    , const intermediate_data& data
) -> void;
