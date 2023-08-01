#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace Airheads {

	class Log {
	public:
		Log(const Log&) = delete;
		Log(const Log&&) = delete;
		Log& operator=(const Log&) = delete;
		Log& operator=(const Log&&) = delete;
		~Log() = default;

		static std::shared_ptr<spdlog::logger>& logger() {
			return get().m_logger;
		}

	private:
		// The constructor shall not be deleted but used to bootstrap the logger. Ignoring
		// the lint warning is ignoring doing `Log() = delete`.
		// NOLINTNEXTLINE
		Log();

		static Log& get() {
			static Log instance{};
			return instance;
		}

		std::shared_ptr<spdlog::logger> m_logger;
	};

}

#ifndef APP_DEACTIVATE_LOGGING

#if DEBUG
#define APP_TRACE(...) ::Airheads::Log::logger()->trace(__VA_ARGS__)
#define APP_DEBUG(...) ::Airheads::Log::logger()->debug(__VA_ARGS__)
#else
#define APP_TRACE(...)
#define APP_DEBUG(...)
#endif

#define APP_INFO(...) ::Airheads::Log::logger()->info(__VA_ARGS__)
#define APP_WARN(...) ::Airheads::Log::logger()->warn(__VA_ARGS__)
#define APP_ERROR(...) ::Airheads::Log::logger()->error(__VA_ARGS__)
#define APP_FATAL(...) ::Airheads::Log::logger()->fatal(__VA_ARGS__)

#else

#define APP_TRACE(...)
#define APP_DEBUG(...)
#define APP_INFO(...)
#define APP_WARN(...)
#define APP_ERROR(...)
#define APP_FATAL(...)

#endif
