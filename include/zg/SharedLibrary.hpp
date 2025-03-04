#pragma once
#ifdef _WIN32
#include <windows.h>
#elif defined(LINUX) || defined(MACOS)
#include <dlfcn.h>
#endif
#include <string_view>
#include <stdexcept>
#include <string>
namespace zg
{
	struct SharedLibrary
	{
	#ifdef _WIN32
		HMODULE libraryPointer;
	#elif defined(LINUX) || defined(MACOS)
		void *libraryPointer;
	#endif
		explicit SharedLibrary(std::string_view path);
		~SharedLibrary();
		template<typename T>
		T getProc(std::string_view procName)
		{
			if (!libraryPointer)
			{
				throw std::runtime_error("Library not loaded");
			}
	#ifdef _WIN32
			void* procAddress = GetProcAddress(libraryPointer, procName.data());
	#elif defined(LINUX) || defined(MACOS)
			void* procAddress = dlsym(libraryPointer, procName.data());
	#endif
			if (!procAddress)
			{
				throw std::runtime_error("Failed to retrieve proc: " + std::string(procName));
			}
			return reinterpret_cast<T>(procAddress);
		};
		SharedLibrary(const SharedLibrary&) = delete;
		SharedLibrary& operator=(const SharedLibrary&) = delete;
		SharedLibrary(SharedLibrary&& other) noexcept;
		SharedLibrary& operator=(SharedLibrary&& other) noexcept;
	};
}