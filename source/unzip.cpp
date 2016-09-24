#include "unzip.h"

#include "intermediate.h"
#include "representation.h"

#pragma warning( push, 1 )
#include <libzippp.h>
#pragma warning( pop )

#include <boost\range\algorithm.hpp>

#include <iostream>
#include <memory>

namespace lzip = libzippp;

using std::begin;
using std::end;

auto unzip(representation) -> bool;

auto unzip_all(intermediate_data& intermediate) -> void
{
	for (const representation& r : intermediate.ready_files)
	{
		if(!unzip(r))
		{
			intermediate.errored_files.insert(r);
		}
	}

	boost::set_difference(
		intermediate.ready_files
		, intermediate.errored_files
		, std::inserter(intermediate.processed_files, end(intermediate.processed_files))
		, rep_less()
	);
	intermediate.ready_files.clear();

}

auto unzip(representation r) -> bool
{
	lzip::ZipArchive zf(r.old_path.string());
	if (!zf.open(lzip::ZipArchive::READ_ONLY, true))
	{
		return false;
	}

	fs::create_directories(r.new_path);

	for (lzip::ZipEntry& entry : zf.getEntries())
	{
		auto binary_data = std::unique_ptr<char[]>(reinterpret_cast<char*>(entry.readAsBinary()));

		auto filepath = r.new_path;
		filepath.append(L"\\").append(entry.getName());
		std::ofstream output_file(filepath.wstring(), std::ios::binary);

		std::copy(
			binary_data.get()
			, binary_data.get() + entry.getSize()
			, std::ostreambuf_iterator<char>(output_file)
		);
	}

	return true;
}
