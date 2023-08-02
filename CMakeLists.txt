cmake_minimum_required(VERSION 3.22)

project(
	airheads
	DESCRIPTION "Meow"
	VERSION 0.1.0
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_COMPANY_NAME "MeowFace Inc.")
set(PROJECT_COMPANY_NAMESPACE "com.meowface")  # Reverse domain name notation

include(cmake/project_settings.cmake)
include(GNUInstallDirs)

include(cmake/compiler_warnings.cmake)

add_subdirectory(external)
add_subdirectory(app)