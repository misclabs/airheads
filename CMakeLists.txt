cmake_minimum_required(VERSION 3.22)

project(
	airheads
	DESCRIPTION "Tiptoe detection for mask fit-testing."
	VERSION 0.1.5
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_COMPANY_NAME "Airheads")
set(PROJECT_COMPANY_NAMESPACE "Airheads")  # Reverse domain name notation

include(cmake/git_version.cmake)
CheckGitSetup()

include(cmake/project_settings.cmake)
include(GNUInstallDirs)

include(cmake/compiler_warnings.cmake)

add_subdirectory(external)
add_subdirectory(app)