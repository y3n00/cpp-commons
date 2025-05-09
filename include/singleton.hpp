#pragma once

#include <memory>
#include <mutex>

template <typename T>
class Singleton
{
  public:
	Singleton(const Singleton&)			   = delete;
	Singleton(Singleton&&)				   = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&)	   = delete;

	static T& get_instance()
	{
		std::call_once(init_flag, [] { instance = std::make_unique<T>(); });

		return *instance;
	}

  protected:
	Singleton()	 = default;
	~Singleton() = default;

  private:
	inline static std::unique_ptr<T> instance = nullptr;
	inline static std::once_flag	 init_flag;
};