#include "nsessentials/data/FileHelper.h"

#if !_WIN32
#include <dirent.h>
#endif

std::string nse::data::str_tolower(const std::string& str)
{
	std::string lower = str;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower;
}

bool nse::data::str_ends_with(const std::string& str, const std::string& end)
{
	//https://stackoverflow.com/a/874160/1210053
	if (str.length() >= end.length())
	{
		return (0 == str.compare(str.length() - end.length(), end.length(), end));
	}
	else
	{
		return false;
	}
}

bool nse::data::file_exists(const std::string& path)
{
	//implementation from https://stackoverflow.com/a/12774387/1210053
	if (FILE *file = fopen(path.c_str(), "r"))
	{
		fclose(file);
		return true;
	}
	else
	{
		return false;
	}
}


//returns if a given path is a directory
bool nse::data::is_directory(const std::string& path)
{
#if _WIN32
	return PathIsDirectory(path.c_str());
#else
	//code based on https://stackoverflow.com/a/146938/1210053
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}
	else
	{
		throw std::runtime_error("Cannot find properties of path \"" + path + "\".");
	}
#endif
}

size_t nse::data::start_of_extension(const std::string& path)
{
	for (size_t i = path.size() - 1; i > 0; --i)
	{
		bool isSeparator = path[i] == '\\' || path[i] == '/';
		if (isSeparator)
			return -1;
		bool isDot = path[i] == '.';
		if (isDot)
			return i;
	}
	return -1;
}

std::string nse::data::extension(const std::string& path)
{
	auto soe = start_of_extension(path);
	if (soe == -1)
		return "";
	return str_tolower(path.substr(soe, path.size() - soe));	
}

std::string nse::data::replace_extension(const std::string& path, const std::string& new_extension)
{
	auto soe = start_of_extension(path);
	bool dot_included = new_extension[0] == '.';
	if (soe == -1)
		return path;
	else
		return path.substr(0, soe + (dot_included ? 0 : 1)) + new_extension;
}

std::string nse::data::filename_without_extension_and_directory(const std::string& path)
{
	size_t filenameUpTo = path.size();
	for (size_t i = path.size() - 1; i > 0; --i)
	{
		bool isSeparator = path[i] == '\\' || path[i] == '/';
		if (isSeparator)
			return path.substr(i + 1, filenameUpTo - i - 1);
		bool isDot = path[i] == '.';
		if (isDot && filenameUpTo == path.size())
			filenameUpTo = i;
	}
	return path.substr(0, filenameUpTo);
}

//Returns the parent of a given path. The parent is found by cutting of everything
//after the last directory separator
std::string nse::data::parent_path(const std::string& path)
{
	bool observedPathName = false; //the path might end with path separators; this flag determines if we are already past them
	for (size_t i = path.size() - 1; i > 0; --i)
	{
		bool isSeparator = path[i] == '\\' || path[i] == '/';
		if (isSeparator && observedPathName)
			return path.substr(0, i);
		if (!isSeparator)
			observedPathName = true;
	}
	throw std::runtime_error("The path \"" + path + "\" has an invalid format.");
}

void nse::data::files_in_dir(const std::string &path, std::vector<std::string>& result)
{
	//based on nanogui::loadImageDirectory
	result.clear();
#if !defined(_WIN32)
	DIR *dp = opendir(path.c_str());
	if (!dp)
		throw std::runtime_error("Could not open directory!");
	struct dirent *ep;
	while ((ep = readdir(dp)))
	{
		const char *fname = ep->d_name;
#else
	WIN32_FIND_DATA ffd;
	std::string searchPath = path + "/*.*";
	HANDLE handle = FindFirstFileA(searchPath.c_str(), &ffd);
	if (handle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not open image directory!");
	do
	{
		const char *fname = ffd.cFileName;
#endif
		result.push_back(path + "/" + std::string(fname));
#if !defined(_WIN32)
	}
	closedir(dp);
#else
	} while (FindNextFileA(handle, &ffd) != 0);
	FindClose(handle);
#endif
}