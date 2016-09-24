#include "intermediate.h"

#include "interpret.h"

#include "constants.hpp"

#include <boost\range\algorithm.hpp>

#include <fstream>
#include <iterator>

const auto locale = std::locale();	//not thread-safe

enum class parse_state
{
    IGNORED
    , PARSED
    , READY
    , NEW
    , PROCESSED
	, ERROR
};

auto is_zip_file(const fs::path& de) -> bool
{
    return alg::iequals(de.extension().wstring(), L".zip", locale);
}

auto read(const fs::path& directory_path, const std::string& extended_path, const fs::path& intermediate_path) -> intermediate_data
{
    auto ignored_files = std::set<fs::path>();
    auto parsed_filenames = rep_set();
    auto ready_files = rep_set();
    auto processed_files = rep_set();
	auto errored_files = rep_set();

    auto explode = [&extended_path](fs::path path) -> representation
    {
        return explode_path(path, extended_path);
    };

    {
        auto infile = std::wifstream(intermediate_path.wstring(), std::ios::binary);
        auto current_line = std::wstring();
        auto state = parse_state::IGNORED;

        std::for_each(std::istreambuf_iterator<wchar_t>(infile), {}, [&](const wchar_t c)
        {
            if (c == L'\n' || c == L'\r')
            {
                if (current_line == ___IGNORING)
                {
                    state = parse_state::PARSED;
                }
                else if (current_line == ___PARSED)
                {
                    state = parse_state::READY;
                }
                else if (current_line == ___READY)
                {
                    state = parse_state::PROCESSED;
                }
				else if (current_line == ___PROCESSED)
				{
					state = parse_state::ERROR;
				}
                else if (current_line == ___ERROR)
                {
                    state = parse_state::NEW;
                }
                else if (!current_line.empty())
                {
                    switch (state)
                    {
                    case parse_state::IGNORED:
                    {
                        ignored_files.insert(current_line);
                        break;
                    }
                    case parse_state::PARSED:
                    {
                        parsed_filenames.insert(split_string_to_rep(current_line, extended_path));
                        break;
                    }
                    case parse_state::READY:
                    {
                        ready_files.insert(split_string_to_rep(current_line, extended_path));
                        break;
                    }
                    case parse_state::PROCESSED:
                    {
                        processed_files.insert(split_string_to_rep(current_line, extended_path));
                        break;
                    }
					case parse_state::ERROR:
					{
						errored_files.insert(split_string_to_rep(current_line, extended_path));
						break;
					}
                    case parse_state::NEW:
                    {
                        const auto temp = split_string_to_rep(current_line, extended_path);
                        if (ignored_files.find(current_line) == ignored_files.end()
                            && ready_files.find(temp) == ready_files.end()
                            && processed_files.find(temp) == processed_files.end()
							&& errored_files.find(temp) == errored_files.end())
                        {
                            parsed_filenames.insert(temp);
                        }
                        break;
                    }
                    default:
                    {
                        assert(false); break;
                    }
                    }
                }
                current_line.clear();
            }
            else
            {
                current_line.push_back(c);
            }
        }
        );
    }

    auto is_new = [&](const fs::path& file) -> bool
    {
        return ignored_files.find(file) == ignored_files.end()
            && parsed_filenames.find(file.wstring()) == parsed_filenames.end()
            && ready_files.find(file.wstring()) == ready_files.end()
            && processed_files.find(file.wstring()) == processed_files.end()
			&& errored_files.find(file.wstring()) == errored_files.end();
    };

    auto get_new_zip_files = [&directory_path, is_new]() -> auto
    {
        return fs::directory_iterator(directory_path)
            | ad::filtered([&](const fs::directory_entry& de) -> bool
        {
			const auto nonrelative_path = fs::system_complete(de);
            return fs::is_regular_file(nonrelative_path) && is_zip_file(nonrelative_path) && is_new(nonrelative_path);
        }
        )
			| ad::transformed([](const fs::directory_entry& de) -> fs::path
		{
			return fs::system_complete(de);
		}
		);
    };

    for (representation r : get_new_zip_files() | ad::transformed(explode))
    {
        parsed_filenames.emplace(std::move(r));
    }

    return{
        std::move(ignored_files)
        , std::move(parsed_filenames)
        , std::move(ready_files)
        , std::move(processed_files)
		, std::move(errored_files)
    };
}

auto write(
    const fs::path& intermediate_path
    , const intermediate_data& data
) -> void
{
    auto outfile = std::wofstream(intermediate_path.wstring());

    boost::copy(
        data.ignored_files | ad::transformed([](const fs::path& p) -> const std::wstring&{return p.wstring();})
        , std::ostream_iterator<std::wstring, wchar_t>(outfile, L"\n")
    );
    outfile << ___IGNORING << L"\n";

    boost::copy(
        data.parsed_filenames
        , std::ostream_iterator<representation, wchar_t>(outfile, L"\n")
    );
    outfile << ___PARSED << L"\n";

    boost::copy(
        data.ready_files
        , std::ostream_iterator<representation, wchar_t>(outfile, L"\n")
    );

    outfile << ___READY << L"\n";

    boost::copy(
        data.processed_files
        , std::ostream_iterator<representation, wchar_t>(outfile, L"\n")
    );

    outfile << ___PROCESSED << L"\n";

	boost::copy(
		data.errored_files
		, std::ostream_iterator<representation, wchar_t>(outfile, L"\n")
	);

	outfile << ___ERROR << L"\n";
}