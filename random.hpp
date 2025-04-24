#pragma once

#include <array>
#include <concepts>
#include <limits>
#include <random>
#include <ranges>
#include <string>
#include <vector>

#ifdef RANDOM_STATIC
	#define AUTO_SIGNATURE inline static auto
	#define VARIABLE_TYPE static inline thread_local
#else
	#define AUTO_SIGNATURE inline auto
	#define VARIABLE_TYPE
#endif

namespace API_Random
{
	/**
	 * @brief Concept that checks if a type is integral or floating-point
	 *
	 * @tparam T The type to check
	 */
	template <typename T>
	concept Numeric_Type = std::is_arithmetic_v<T>;

	/**
	 * @brief Concept that checks if a type is a string
	 *
	 * @tparam T The type to check
	 */
	template <typename T>
	concept IsString = std::is_same_v<T, std::string>;

	using string_gen_pred = std::function<bool(uint8_t)>;

	/**
	 * @brief Concept that checks if the type is suitable for generating string
	 *
	 * @note The T should be the range of chars`(string/string_view/const char*, ...)` or predicate `(isalpha, isprint, ...)`
	 * @tparam T The type to check
	 */
	template <typename T>
	concept StringGeneraionType = std::is_convertible_v<T, std::string> || std::is_convertible_v<T, string_gen_pred>;
} // namespace API_Random

/**
 * @def CREATE_LIMIT
 * @brief Macro to create a limit variable of a specified type
 *
 * @param T The type of the limit
 * @param name The name of the limit variable
 * @param val The limit value (min or max)
 */
#define CREATE_LIMIT(T, name, val) T name = (std::numeric_limits<T>::val)()

/**
 * @def MIN_MAX_LIMIT
 * @brief Macro to create min or max limit variables
 *
 * @param T The type of the limit
 * @param op The operation (min or max)
 */
#define MIN_MAX_LIMIT(T, op) CREATE_LIMIT(T, op##_val, op)

#define MAX_LIMIT(T) MIN_MAX_LIMIT(T, max)
#define MIN_LIMIT(T) MIN_MAX_LIMIT(T, min)

/**
 * @class Random_t
 * @brief A templated class for generating random numbers and strings.
 *
 * This class provides methods to generate random numbers within specified ranges,
 * fill ranges with random values, and generate random strings based on given criteria
 *
 * @tparam RandomEngine The random number engine type. Defaults to std::minstd_rand.
 */
template <typename RandomEngine = std::minstd_rand>
class Random_t
{
	template <std::integral I>
	using int_dist = std::uniform_int_distribution<I>;

	template <std::floating_point R>
	using real_dist = std::uniform_real_distribution<R>;

  private:
	VARIABLE_TYPE RandomEngine gen{std::random_device{}()};

	/**
	 * @brief Get the appropriate distribution for the given numeric type
	 *
	 * @tparam Num_Type The numeric type for which to get the distribution
	 * @param min_val minimum value for the distribution (inclusive)
	 * @param max_val maximum value for the distribution (inclusive)
	 * @return A distribution object for the specified numeric type
	 */
	template <API_Random::Numeric_Type Num_Type>
	[[nodiscard]] constexpr AUTO_SIGNATURE get_distribution(MIN_LIMIT(Num_Type), MAX_LIMIT(Num_Type))
	{
		if constexpr (std::is_floating_point_v<Num_Type>)
			return real_dist<Num_Type>{min_val, max_val};
		else if constexpr (sizeof(Num_Type) == 1)		// cant use 1 byte types on msvc
			return int_dist<int16_t>{min_val, max_val}; // int16_t covers all numeric limits of 1 byte types
		else
			return int_dist<Num_Type>{min_val, max_val};
	}

  public:
	using seed_t = std::decay_t<decltype(decltype(gen)::default_seed)>;

	/**
	 * @brief Constructor with a seed
	 *
	 * @param seed The seed for the random number generator
	 */
	Random_t(seed_t seed)
	{
		gen.seed(seed);
	}

	Random_t() = default;
	Random_t(Random_t&&) = default;
	Random_t& operator=(Random_t&&) = default;

	Random_t(const Random_t&) = delete;
	Random_t& operator=(const Random_t&) = delete;

	~Random_t() = default;

	/**
	 * @brief Generate a random number within a specified range
	 *
	 * @tparam Num_Type The numeric type of the random number
	 * @param min_val The minimum value (inclusive)
	 * @param max_val The maximum value (inclusive)
	 * @return A random number of type Num_Type in selected range
	 */
	template <API_Random::Numeric_Type Num_Type>
	[[nodiscard]] AUTO_SIGNATURE in_range(MIN_LIMIT(Num_Type), MAX_LIMIT(Num_Type)) -> Num_Type
	{
		return static_cast<Num_Type>(get_distribution(min_val, max_val)(gen));
	}

	/**
	 * @brief Generate a random number from zero to a specified maximum
	 *
	 * @tparam Num_Type The numeric type of the random number
	 * @param max_val The maximum value (inclusive)
	 * @return A random number of type Num_Type
	 */
	template <API_Random::Numeric_Type Num_Type>
	[[nodiscard]] AUTO_SIGNATURE from_zero_to(MAX_LIMIT(Num_Type)) -> Num_Type
	{
		return in_range<Num_Type>({}, max_val);
	}

	/**
	 * @brief Generate a random boolean value
	 *
	 * @return A random boolean value
	 */
	[[nodiscard]] AUTO_SIGNATURE get_bool() -> bool
	{
		return in_range<bool>();
	}

	/**
	 * @brief Generate a random boolean value based on a probability
	 *
	 * @param prob The probability of returning true (between 0 and 1)
	 * @return A random boolean value
	 */
	[[nodiscard]] AUTO_SIGNATURE chance_probability(double prob) -> bool
	{
		return std::bernoulli_distribution{std::clamp<double>(prob, 0, 1)}(gen);
	}

	/**
	 * @brief Generate a random boolean value based on a percentage
	 *
	 * @param percent The percentage chance of returning true (between 0 and 100)
	 * @return A random boolean value
	 */
	[[nodiscard]] AUTO_SIGNATURE chance_percent(double percent) -> bool
	{
		return chance_probability(percent / 100);
	}

	/**
	 * @brief Get a random element from a given range
	 *
	 * @tparam R The type of the range
	 * @param range The range from which to select an element
	 * @return A random element from the range
	 */
	[[nodiscard]] AUTO_SIGNATURE get_elem(std::ranges::range auto&& range)
	{
		const auto last_idx = std::ranges::distance(range) - 1;
		auto it = std::ranges::begin(range);
		std::ranges::advance(it, from_zero_to(last_idx));

		return *it;
	}

	/**
	 * @brief Fill a range with random numeric values within specified limits
	 *
	 * @tparam R The type of the range
	 * @tparam T The type of the elements in the range
	 * @param range The range to fill
	 * @param min_val The minimum value (inclusive)
	 * @param max_val The maximum value (inclusive)
	 */
	template <std::ranges::range R, typename T = std::ranges::range_value_t<R>>
		requires API_Random::Numeric_Type<T>
	AUTO_SIGNATURE fill_range(R& range, MIN_LIMIT(T), MAX_LIMIT(T)) -> void
	{
		std::ranges::generate(range, [&] { return in_range(min_val, max_val); });
	}

	/**
	 * @brief Fill a range of strings with random strings based on specified criteria
	 *
	 * @tparam R The type of the range
	 * @tparam Gen The type of the range of chars`(string/string_view/const char*, ...)` or predicate `(isalpha, isprint, ...)`
	 * @param range The range to fill
	 * @param min_len The minimum length of the strings
	 * @param max_len The maximum length of the strings
	 * @param gen The chars or predicate for string generation
	 */
	template <std::ranges::range R, API_Random::StringGeneraionType Gen, std::unsigned_integral Length_Type = uint8_t>
		requires API_Random::IsString<std::ranges::range_value_t<R>>
	AUTO_SIGNATURE fill_range(R& range, Length_Type min_len, Length_Type max_len, Gen&& gen) -> void
	{
		if constexpr (std::is_convertible_v<Gen, API_Random::string_gen_pred>)
		{
			std::ranges::generate(range, [&, pred = API_Random::string_gen_pred(std::move(gen))] {
				return get_string_pred(in_range(min_len, max_len), pred);
			});
		}
		else
		{
			std::ranges::generate(range, [&, view = std::string_view(gen)] {
				return get_string_chars(in_range(min_len, max_len), view);
			});
		}
	}

	/**
	 * @brief Fill a range with elements from another range
	 *
	 * @tparam R1 The type of the destination range
	 * @tparam R2 The type of the source range
	 * @param to_fill The range to fill
	 * @param fill_from The range to fill from
	 */
	template <std::ranges::range R1,
			  std::ranges::range R2,
			  typename T1 = std::ranges::range_value_t<R1>,
			  typename T2 = std::ranges::range_value_t<R2>>
		requires std::convertible_to<T2, T1>
	AUTO_SIGNATURE fill_range_from(R1& to_fill, R2&& fill_from) -> void
	{
		if (not std::ranges::empty(fill_from))
		{
			std::ranges::generate(to_fill, [&] { return static_cast<T1>(get_elem(fill_from)); });
		}
	}

	/**
	 * @brief Generate an array of random numbers within specified limits
	 *
	 * @tparam T The numeric type of the array elements
	 * @tparam SZ The size of the array
	 * @param min_val The minimum value (inclusive)
	 * @param max_val The maximum value (inclusive)
	 * @return An array of random numbers
	 */
	template <API_Random::Numeric_Type T, size_t SZ>
	[[nodiscard]] AUTO_SIGNATURE get_array(MIN_LIMIT(T), MAX_LIMIT(T)) -> std::array<T, SZ>
	{
		std::array<T, SZ> arr;
		fill_range(arr, min_val, max_val);

		return arr;
	}

	/**
	 * @brief Generate an array of random strings based on specified criteria
	 *
	 * @tparam T The string type of the array elements
	 * @tparam SZ The size of the array
	 * @tparam Length_Type The type for the length of the strings
	 * @param gen The chars or predicate for string generation
	 * @param min_str_length The minimum length of the strings
	 * @param max_str_length The maximum length of the strings
	 * @return An array of random strings
	 */
	template <API_Random::IsString T, size_t SZ, std::unsigned_integral Length_Type>
	[[nodiscard]] AUTO_SIGNATURE get_array(API_Random::StringGeneraionType auto&& gen,
										   CREATE_LIMIT(Length_Type, min_str_length, min),
										   CREATE_LIMIT(Length_Type, max_str_length, max)) -> std::array<T, SZ>
	{
		std::array<T, SZ> arr;
		fill_range(arr, min_str_length, max_str_length, std::move(gen));

		return arr;
	}

	/**
	 * @brief Generate a vector of random numbers within specified limits
	 *
	 * @tparam T The numeric type of the vector elements
	 * @param size The size of the vector
	 * @param min_val The minimum value (inclusive)
	 * @param max_val The maximum value (inclusive)
	 * @return A vector of random numbers
	 */
	template <API_Random::Numeric_Type T>
	[[nodiscard]] AUTO_SIGNATURE get_vector(size_t size, MIN_LIMIT(T), MAX_LIMIT(T)) -> std::vector<T>
	{
		std::vector<T> vec(size);
		fill_range(vec, min_val, max_val);

		return vec;
	}

	/**
	 * @brief Generate a vector of random strings based on specified criteria
	 *
	 * @tparam T The string type of the vector elements
	 * @param size The size of the vector
	 * @param gen The chars or predicate for string generation
	 * @param min_str_length The minimum length of the strings
	 * @param max_str_length The maximum length of the strings
	 * @return A vector of random strings
	 */
	template <API_Random::IsString T, std::unsigned_integral Length_Type>
	[[nodiscard]] AUTO_SIGNATURE get_vector(size_t size,
											API_Random::StringGeneraionType auto&& gen,
											CREATE_LIMIT(Length_Type, min_str_length, min),
											CREATE_LIMIT(Length_Type, max_str_length, max)) -> std::vector<T>
	{
		std::vector<std::string> vec(size);
		fill_range(vec, min_str_length, max_str_length, std::move(gen));

		return vec;
	}

	/**
	 * @brief Shuffle the elements of a random access range
	 *
	 * @param range The range to shuffle
	 */
	AUTO_SIGNATURE shuffle_range(std::ranges::random_access_range auto& range) -> void
	{
		std::ranges::shuffle(range, gen);
	}

	/**
	 * @brief Generate a random string of specified length using given symbols
	 *
	 * @param str_len The length of the string to generate
	 * @param symbols The symbols to use for generating the string
	 * @return A random string composed of the specified symbols
	 */
	[[nodiscard]] AUTO_SIGNATURE get_string_chars(size_t str_len, const std::string_view symbols) -> std::string
	{
		std::string result(str_len, '\0');
		fill_range_from(result, symbols);

		return result;
	}

	/**
	 * @brief Generate a random string of specified length using a predicate to filter symbols
	 *
	 * @param str_len The length of the string to generate
	 * @param pred The predicate function to filter symbols
	 * @return A random string composed of symbols that satisfy the predicate
	 */
	[[nodiscard]] AUTO_SIGNATURE get_string_pred(size_t str_len, const API_Random::string_gen_pred& pred) -> std::string
	{
		auto symbols = std::views::iota(CHAR_MIN, CHAR_MAX + 1) | std::views::filter(pred);

		std::string result(str_len, '\0');
		fill_range_from(result, symbols);

		return result;
	}

	/**
	 * @brief Generate a random alphanumeric string of specified length
	 *
	 * @param str_len The length of the string to generate
	 * @return A random alphanumeric string
	 */
	[[nodiscard]] AUTO_SIGNATURE get_string(size_t str_len) -> std::string
	{
		return get_string_pred(str_len, isalnum);
	}
};

#undef AUTO_SIGNATURE
#undef VARIABLE_TYPE
#undef CREATE_LIMIT
#undef MIN_MAX_LIMIT
#undef MAX_LIMIT
#undef MIN_LIMIT