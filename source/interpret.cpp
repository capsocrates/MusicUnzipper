#include "interpret.h"

#include "representation.h"

#include <boost\range\algorithm.hpp>

#include <iostream>
#include <string>

const auto locale = std::locale();	//not thread-safe

auto is_not_space(const wchar_t c) -> bool
{
    return c != L' ' && c != L'_' && c != L'-';
}

auto trim(sub_str range_ref) -> sub_str
{
    return sub_str(
        boost::find_if(
            range_ref
            , is_not_space
        )
        , (boost::find_if(
            ad::reverse(range_ref)
            , is_not_space
        )
            ).base()
    );
}

auto print(sub_str str_range) -> std::wostream&
{
    boost::copy(str_range, std::ostreambuf_iterator<wchar_t>(std::wcout));
    return std::wcout;
}

auto dash_or_underscore(const wchar_t c) -> bool
{
    return c == L'_' || c == L'-';
};

auto maybe_capitalize(char_range str) -> void
{
    constexpr wchar_t* to_capitalize[]{
        L"ep"
        , L"lp"
        , L"ato"
    };
    for (const auto check : to_capitalize)
    {
        if (alg::iequals(str, check, locale))
        {
            alg::to_upper(str, locale);
            return;
        }
    }
};

auto sanitize(sub_str str) -> sub_str
{
    char_range last_str = {};
    auto char_count = 0;
    bool last_char_was_space = true;
    auto sub_parts = std::vector<char_range>();
    for (wchar_t& c : str)
    {
        if (last_char_was_space && !dash_or_underscore(c))
        {
            last_str = {&c, &c + 1};
            alg::to_upper(last_str, locale);
            last_char_was_space = false;
        }
        if (dash_or_underscore(c))
        {
            c = L' ';
            last_char_was_space = true;
            last_str = {last_str.begin(), last_str.begin() + char_count};
            char_count = 0;
            maybe_capitalize(last_str);
        }
        else
        {
            ++char_count;
        }
    }

    return str;
};

auto explode_path(const fs::path& path, const std::string& intermediate_path) -> representation
{
    auto filename = path.filename().wstring();
    auto path_str = sub_str(filename);
    path_str.drop_back(4);  //shear off ".zip"
    auto parts = std::vector<sub_str>();
    {
        auto contiguous_underscores = 0;
        alg::split(parts, path_str, [&](const wchar_t c) -> bool
        {
            if (c == L'_')
            {
                ++contiguous_underscores;
            }
            else
            {
                contiguous_underscores = 0;
            }

            return 3 <= contiguous_underscores;
        });
    }

    if (parts.empty())
    {
        throw std::runtime_error(std::string("bad filename!"));
    }

    return{path.wstring(), path.parent_path()
		.append(intermediate_path)
		.append(sanitize(trim(parts.front())))
		.append(sanitize(trim(parts.back())))};
}

auto split_string_to_rep(const std::wstring& str, const std::string& intermediate_path) -> representation
{
    constexpr auto divider = L" -> ";
    constexpr auto div_sz = 4;  //4 chars
    const auto pos = str.find(divider);
    if (pos == std::wstring::npos)
    {
        return explode_path(str, intermediate_path);
    }
    const auto b = std::begin(str), e = std::end(str);
    return{{b, b + pos},{b + pos + div_sz, e}};
}