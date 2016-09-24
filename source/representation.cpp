#include "representation.h"

#include <boost\algorithm\string\predicate.hpp>	//for ilexicographical_compare

#include <iostream>
#include <string>

const auto locale = std::locale();	//not thread-safe

auto rep_less::operator()(const representation& lhs, const representation& rhs) const -> bool
{
    return alg::ilexicographical_compare(lhs.old_path.wstring(), rhs.old_path.wstring(), locale);
}

auto rep_less::operator()(const std::wstring& lhs, const representation& rhs) const -> bool
{
	return alg::ilexicographical_compare(lhs, rhs.old_path.wstring(), locale);
}

auto rep_less::operator()(const representation& lhs, const std::wstring& rhs) const -> bool
{
	return alg::ilexicographical_compare(lhs.old_path.wstring(), rhs, locale);
}

auto operator<<(std::wostream& os, const representation& r) -> std::wostream&
{
    return os << r.old_path.wstring() << L" -> " << r.new_path.wstring();
}
