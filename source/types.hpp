#pragma once

#include <boost\algorithm\string.hpp>
#include <boost\filesystem.hpp>
#include <boost\range\adaptors.hpp>
#include <boost\range\sub_range.hpp>

namespace fs = boost::filesystem;
namespace alg = boost::algorithm;
namespace ad = boost::adaptors;
using sub_str = boost::sub_range<std::wstring>;
using char_range = boost::iterator_range<wchar_t*>;
