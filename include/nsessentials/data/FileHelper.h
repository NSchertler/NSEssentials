/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

#if _WIN32
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#else
#include <sys/stat.h>
#endif

#include "nsessentials/NSELibrary.h"

namespace nse {
	namespace data
	{
		extern NSE_EXPORT std::string str_tolower(const std::string& str);

		extern NSE_EXPORT bool str_ends_with(const std::string& str, const std::string& end);

		extern NSE_EXPORT bool file_exists(const std::string& path);


		//returns if a given path is a directory
		extern NSE_EXPORT bool is_directory(const std::string& path);

		extern NSE_EXPORT size_t start_of_extension(const std::string& path);

		extern NSE_EXPORT std::string extension(const std::string& path);

		extern NSE_EXPORT std::string replace_extension(const std::string& path, const std::string& new_extension);

		extern NSE_EXPORT std::string filename_without_extension_and_directory(const std::string& path);

		//Returns the parent of a given path. The parent is found by cutting of everything
		//after the last directory separator
		extern NSE_EXPORT std::string parent_path(const std::string& path);

		extern NSE_EXPORT void files_in_dir(const std::string &path, std::vector<std::string>& result);
	}
}