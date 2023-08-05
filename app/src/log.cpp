#include "log.h"

//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/stdout_color_sinks.h>
//
//#include <vector>

namespace Airheads {

	//Log::Log() {
	//	std::vector<spdlog::sink_ptr> logSinks;

	//	const spdlog::level::level_enum level{spdlog::level::debug};

	//	logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	//	logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("app.log", true));

	//	logSinks[0]->set_pattern("%^[%T] %n(%l): %v%$");
	//	logSinks[1]->set_pattern("[%T] [%l] %n(%l): %v");

	//	m_logger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
	//	spdlog::register_logger(m_logger);
	//	spdlog::set_default_logger(m_logger);
	//	m_logger->set_level(level);
	//	m_logger->flush_on(level);
	//}

}
