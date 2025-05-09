/**
 * @file filesys.hpp
 * @brief Cross-platform filesystem utilities
 * @details Provides file creation, hidden attribute management, and binary I/O operations.
 */

#pragma once

#include <filesystem>
#include <fstream>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

namespace filesys
{
	using namespace std::filesystem;

	using byte		   = uint8_t;
	using byte_storage = std::vector<byte>;
	using byte_view	   = std::span<const byte>;

	/**
	 * @brief Checks if file is hidden
	 * @param path Target file path
	 * @return true if file is hidden
	 */
	[[nodiscard]] inline bool is_hidden_file(const path& path)
	{
#ifdef _WIN32
		const auto attrs = GetFileAttributesW(path.c_str());

		return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_HIDDEN);
#else
		const auto name = path.filename().string();

		return !name.empty() && name.front() == '.';
#endif
	}

	/**
	 * @brief Updates file hidden attribute
	 * @param file Target file path
	 * @param hide Whether to hide the file
	 * @return true if operation succeeded
	 */
	inline bool change_file_visibility(const path& file, bool hide)
	{
#ifdef _WIN32
		auto attrs = GetFileAttributesW(file.c_str());

		if (attrs == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}

		if (hide)
		{
			attrs |= FILE_ATTRIBUTE_HIDDEN;
		}
		else
		{
			attrs &= ~FILE_ATTRIBUTE_HIDDEN;
		}

		return SetFileAttributesW(file.c_str(), attrs);
#else
		const auto fname = file.filename().string();

		if (fname.empty())
		{
			return false;
		}

		if (const bool current_state = (fname.front() == '.'); current_state == hide)
		{
			return true;
		}

		const path new_path = file.parent_path() / (hide ? ('.' + fname) : fname.substr(1));

		std::error_code ec;
		rename(file, new_path, ec);

		return !ec;
#endif
	}

	/**
	 * @brief Creates a new file with specified parameters
	 * @param where Target directory path
	 * @param filename Base filename (without extension)
	 * @param extension File extension (without leading dot)
	 * @param hidden Whether the file should be hidden
	 * @return Full path to created file
	 */
	[[nodiscard]] inline path create_file(const path& where, const std::string_view filename,
										  const std::string_view extension = "", bool hidden = false)
	{
		std::string fname(filename);

#ifndef _WIN32
		if (hidden && !fname.empty() && fname[0] != '.')
		{
			fname.insert(0, 1, '.');
		}
#endif
		path full_path;

		if (const auto filename_path = path{filename}; filename_path.has_extension())
		{
			full_path = where / fname;
		}
		else
		{
			full_path = where / path{filename}.replace_extension(extension);
		}

		create_directories(full_path.parent_path());

		if (std::ofstream ofs(full_path); ofs.bad())
		{
			return {};
		}

		if (hidden)
		{
			change_file_visibility(full_path, true);
		}

		return full_path;
	}

	/**
	 * @brief Reads entire file into byte vector
	 * @param path Source file path
	 * @return File contents or empty vector on error
	 */
	[[nodiscard]] inline byte_storage load_bytes(const path& path) noexcept
	{
		if (not exists(path))
		{
			return {};
		}

		std::ifstream ifs(path, std::ios::binary | std::ios::ate);

		return byte_storage{std::istream_iterator<char>{ifs}, {}};
	}

	/**
	 * @brief Writes binary data to file
	 * @param path Target file path
	 * @param data Data to write
	 * @return true if write succeeded
	 */
	inline bool write_bytes(const path& path, byte_view data) noexcept
	{
		std::ofstream ofs(path, std::ios::binary | std::ios::trunc);

		return ofs.write(reinterpret_cast<const char*>(data.data()), data.size()).good();
	}
} // namespace filesys