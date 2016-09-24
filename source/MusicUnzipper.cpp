// MusicUnzipper.cpp : Defines the entry point for the console application.
//

#include "intermediate.h"
#include "unzip.h"

#include "constants.hpp"
#include "types.hpp"

#include "stdafx.h"

#include <boost\range\algorithm.hpp>

#include <iostream>
#include <iterator>
#include <string>

auto main(const int argc, const char* const argv[]) -> int
{
	auto command = 'd';
	auto extended_path = std::string("Music");
	auto path = fs::path();
	if (argc <= 1)
	{
		path = ".";
	}
	else if (1 < argc)
	{
		path = argv[1];
		if (2 < argc)
		{
			command = argv[2][0];
		}
		if (3 < argc)
		{
			extended_path = argv[3];
		}
	}
	try
	{
		if (fs::exists(path) && fs::is_directory(path))
		{
			const auto ___INTERMEDIATE = path / L"___INTERMEDIATE";
			auto inter_data = read(path, extended_path, ___INTERMEDIATE.wstring());

			switch (command)
			{
			case 'd': case 'D':
			{
				std::wcout << L"Evaluating default command; parsing directory at " << path << std::endl;
				//the parse has already happened at this point--we always parse
				break;
			}
			case 'e': case 'E':
			{
				std::wcout << L"Evaluating execute command" << std::endl;
				unzip_all(inter_data);
				break;
			}
			default:
			{
				throw(std::runtime_error("Invalid command!"));
			}
			}

			write(___INTERMEDIATE, inter_data);
		}
		else
		{
			auto pstr = path.string();
			throw std::runtime_error(
				(!fs::exists(path) 
				? pstr.append(" does not exist!")
				 : (
					 !fs::is_directory(path)
					 ? pstr.append(" is not a directory!")
					 : pstr.append(" could not be parsed")
					)
				 )
			);
		}
	}
	catch (std::exception e)
	{
		std::wcout << e.what() << L"\n";
		return 1;
	}
	return 0;
}
