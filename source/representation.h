#pragma once

#include "types.hpp"

#include <iosfwd>
#include <set>

struct representation
{
    fs::path old_path;
    fs::path new_path;
};

struct rep_less
{
    using is_transparent = std::true_type;

    auto operator()(const representation& lhs, const representation& rhs) const -> bool;
    auto operator()(const std::wstring& lhs, const representation& rhs) const -> bool;
    auto operator()(const representation& lhs, const std::wstring& rhs) const -> bool;
};

using rep_set = std::set<representation, rep_less>;

auto operator<<(std::wostream& os, const representation& r) -> std::wostream&;
