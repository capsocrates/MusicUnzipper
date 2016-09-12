#include "representation.h"

#include <iostream>
#include <string>

auto rep_less::operator()(const representation& lhs, const representation& rhs) const -> bool
{
    return lhs.old_path < rhs.old_path;
}

auto rep_less::operator()(const std::wstring& lhs, const representation& rhs) const -> bool
{
    return lhs < rhs.old_path.wstring();
}

auto rep_less::operator()(const representation& lhs, const std::wstring& rhs) const -> bool
{
    return lhs.old_path.wstring() < rhs;
}

auto operator<<(std::wostream& os, const representation& r) -> std::wostream&
{
    return os << r.old_path.wstring() << L" -> " << r.new_path.wstring();
}
