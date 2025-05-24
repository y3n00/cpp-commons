#pragma once

#include <array>
#include <cctype>
#include <concepts>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>

#ifdef _WIN32
	#include <conio.h>
	#include <windows.h>
#else
	#include <sys/ioctl.h>
	#include <termios.h>
	#include <unistd.h>
#endif

namespace Output
{
	class Text;
}

class Console
{
	friend class Output::Text;
	friend struct ConsoleManager;

  private:
	struct Constants
	{
		static constexpr std::string_view CSI		  = "\033[";
		static constexpr std::string_view CLEAR_ALL	  = "\033[2J\033[H";
		static constexpr std::string_view CURSOR_SHOW = "\033[?25h";
		static constexpr std::string_view CURSOR_HIDE = "\033[?25l";
		static constexpr std::string_view RESET		  = "\033[0m";
	};

	class Platform
	{
	  public:
#ifdef _WIN32
		inline static HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
		inline static DWORD	 original_mode;
#else
		inline static termios original_termios;
		inline static bool	  initialized = false;
#endif

		static void initialize() noexcept
		{
#ifdef _WIN32
			GetConsoleMode(h_out, &original_mode);
			SetConsoleMode(h_out, original_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
			if (!initialized)
			{
				tcgetattr(STDIN_FILENO, &original_termios);
				initialized = true;
			}
#endif
		}

		static void restore() noexcept
		{
#ifdef _WIN32
			SetConsoleMode(h_out, original_mode);
#else
			tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
#endif
		}

		class Size
		{
		  public:
			static std::pair<int, int> get() noexcept
			{
#ifdef _WIN32
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(Platform::h_out, &csbi);

				return std::pair(
					csbi.srWindow.Right - csbi.srWindow.Left + 1,
					csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
#else
				winsize ws{};
				ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
				return std::pair(ws.ws_col, ws.ws_row);
#endif
			}
		};
	};

  public:
	static std::pair<int, int> get_terminal_size() noexcept
	{
		return Platform::Size::get();
	}

	static void clear() noexcept
	{
		std::cout << Constants::CLEAR_ALL;
	}

	static void set_cursor(int row, int col) noexcept
	{
		std::cout << std::format("{};{}H", row, col);
	}

	static void cursor_visibility(bool visible) noexcept
	{
		std::cout << (visible ? Constants::CURSOR_SHOW : Constants::CURSOR_HIDE);
	}
};
namespace Output
{
	namespace Style
	{
		enum EStyles : uint64_t
		{
			style_bold				= 1ULL << 0,
			style_dim				= 1ULL << 1,
			style_italic			= 1ULL << 2,
			style_underline			= 1ULL << 3,
			style_blink				= 1ULL << 4,
			style_inverse			= 1ULL << 5,
			style_hidden			= 1ULL << 6,
			style_strikethrough		= 1ULL << 7,
			text_black				= 1ULL << 8,
			text_red				= 1ULL << 9,
			text_green				= 1ULL << 10,
			text_yellow				= 1ULL << 11,
			text_blue				= 1ULL << 12,
			text_purple				= 1ULL << 13,
			text_cyan				= 1ULL << 14,
			text_gray				= 1ULL << 15,
			text_brightgray			= 1ULL << 16,
			text_brightred			= 1ULL << 17,
			text_brightgreen		= 1ULL << 18,
			text_lightblue			= 1ULL << 19,
			text_magenta			= 1ULL << 20,
			text_lightcyan			= 1ULL << 21,
			text_white				= 1ULL << 22,
			text_default			= 1ULL << 23,
			background_black		= 1ULL << 24,
			background_red			= 1ULL << 25,
			background_green		= 1ULL << 26,
			background_yellow		= 1ULL << 27,
			background_blue			= 1ULL << 28,
			background_purple		= 1ULL << 29,
			background_cyan			= 1ULL << 30,
			background_gray			= 1ULL << 31,
			background_brightgray	= 1ULL << 32,
			background_brightred	= 1ULL << 33,
			background_brightgreen	= 1ULL << 34,
			background_brightyellow = 1ULL << 35,
			background_lightblue	= 1ULL << 36,
			background_magenta		= 1ULL << 37,
			background_lightcyan	= 1ULL << 38,
			background_white		= 1ULL << 39,
			background_default		= 1ULL << 40,
		};
	} // namespace Style

	class Text
	{
	  private:
		std::string content;
		uint64_t	flags = 0;

		static constexpr std::array<int, 64> ansi_map = {
			1, 2, 3, 4, 5, 7, 8, 9, // styles

			30, 31, 32, 33, 34, 35, 36, 37, // text_black - text_gray
			90, 91, 92, 94, 95, 96, 97, 39, // text_brightgray - text_default

			40, 41, 42, 43, 44, 45, 46, 47,				// background_black - background_gray
			100, 101, 102, 103, 104, 105, 106, 107, 49, // background_brightgray - background_default
		};

		[[nodiscard]] std::string build() const
		{
			if (content.empty() || flags == 0)
			{
				return content;
			}

			const auto codes = std::views::iota(0, 64) |
							   std::views::filter([&](int i) { return (flags & (1ULL << i)); }) |
							   std::views::transform([&map = ansi_map](int i) { return std::to_string(map[i]); }) |
							   std::views::join_with(';') |
							   std::ranges::to<std::string>();

			return std::format("{}{}m{}{}", Console::Constants::CSI, codes, content, Console::Constants::RESET);
		}

	  public:
		Text() = default;

		explicit Text(std::string_view str, uint64_t style = 0)
			: content(str), flags(style)
		{
		}

		Text& operator=(std::string_view str) noexcept
		{
			content = str;

			return *this;
		}

		Text& apply_style(uint64_t style) noexcept
		{
			flags |= style;

			return *this;
		}

		Text& remove_style(uint64_t style) noexcept
		{
			flags &= ~style;

			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const Text& text)
		{
			if (dynamic_cast<std::ofstream*>(&os))
			{
				return os << text.content;
			}

			return os << text.build();
		}
	};
} // namespace Output

namespace Input
{
	enum Constants
	{
#ifdef _WIN32
		BACKSPACE = 8,
		ENTER	  = '\r'
#else
		BACKSPACE = 127,
		ENTER	  = '\n'
#endif
	};

	enum class Mode
	{
		Plain,
		Password
	};

	namespace Platform
	{
		int getch(bool echo) noexcept
		{
#ifdef _WIN32
			return _getch();
#else
			termios newt = Console::Platform::originalTermios;
			newt.c_lflag &= ~ICANON;

			if (echo)
			{
				newt.c_lflag |= ECHO;
			}
			else
			{
				newt.c_lflag &= ~ECHO;
			}

			tcsetattr(STDIN_FILENO, TCSANOW, &newt);
			int ch = getchar();
			tcsetattr(STDIN_FILENO, TCSANOW, &Console::Platform::originalTermios);

			return ch;
#endif
		}
	} // namespace Platform

	template <typename T, Mode mode = Mode::Plain>
		requires std::is_arithmetic_v<T> || std::is_same_v<T, std::string>
	[[nodiscard]] T read(size_t input_length = SIZE_MAX, char secret_char = '*', auto&& filter = std::isgraph)
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			std::string buffer;

			while (true)
			{
				const int  ch  = Platform::getch(mode == Mode::Plain);
				const auto uch = static_cast<unsigned char>(ch);

				if (ch == Constants::ENTER)
				{
					std::cout << '\n';

					break;
				}

				if (ch == Constants::BACKSPACE)
				{
					if (!buffer.empty())
					{
						buffer.pop_back();
						std::cout << "\b \b";
					}

					continue;
				}

				if (!filter(uch) || buffer.length() == input_length)
				{
					std::cout << '\a';

					continue;
				}

				buffer.push_back(static_cast<char>(uch));
				std::cout << (mode == Mode::Password ? secret_char : uch);
			}

			return buffer;
		}
		else
		{
			constexpr static auto ssize_max = (std::numeric_limits<std::streamsize>::max)();
			constexpr static auto ignore	= std::bind_front(&std::istream::clear, &std::cin, ssize_max, '\n');
			static_assert(mode == Mode::Plain, "password mode only available for string input");

			T value;
			while (true)
			{
				if (std::cin >> value)
				{
					ignore();

					return value;
				}

				std::cin.clear();
				ignore();
			}
		}
	}
} // namespace Input

struct ConsoleManager
{
	ConsoleManager()
	{
		Console::Platform::initialize();
	}

	~ConsoleManager()
	{
		Console::Platform::restore();
	}

	ConsoleManager(const ConsoleManager&)			 = delete;
	ConsoleManager& operator=(const ConsoleManager&) = delete;
};