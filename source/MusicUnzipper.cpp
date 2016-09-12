// MusicUnzipper.cpp : Defines the entry point for the console application.
//

#include "intermediate.h"

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

    switch (command)
    {
    case 'd': case 'D':
    {
        std::wcout << L"Evaluating default command; parsing directory at " << path << std::endl;
        try
        {
            if (fs::exists(path) && fs::is_directory(path))
            {
                const auto ___INTERMEDIATE = path.wstring() + L"___INTERMEDIATE";
                const auto inter_data = read(path, extended_path, ___INTERMEDIATE);
                write(___INTERMEDIATE, inter_data);
            }
            else
            {
                if (!fs::exists(path))
                {
                    std::wcout << path << L" does not exist!\n";
                }
                else if (!fs::is_directory(path))
                {

                    std::wcout << path << L" is not a directory!\n";
                }
                return 1;
            }
        }
        catch (std::exception e)
        {
            std::wcout << e.what() << L"\n";

            return 1;
        }
        break;
    }
    default:
    {
        std::wcout << L"Invalid command!" << std::endl;
        return 1;
    }
    }
    return 0;
}

