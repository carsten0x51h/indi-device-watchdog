# 
# External dependencies
#
find_package(INDI COMPONENTS client REQUIRED)

# 
# Executable name and options
# 

# Target name
set(target indi_device_watchdog)


# 
# Sources
#
set(sources
	enum_helper.h
	device_data.h
	device_data.cpp
	option_level.h
	logging.h
	logging.cpp
	device_data_persistance.h	
	device_data_persistance.cpp
	indi_driver_restart_manager.h
	indi_driver_restart_manager.cpp
	indi_client.cpp
	indi_client.h
	indi_device_watchdog.cpp
	indi_device_watchdog.h
	main.cpp
)


# 
# Create executable
# 

# Build executable
add_executable(${target}
        MACOSX_BUNDLE
        ${sources}
        )


# 
# Project options
#
set_target_properties(${target}
        PROPERTIES
        ${DEFAULT_PROJECT_OPTIONS}
        FOLDER "${IDE_FOLDER}"
        )


# 
# Include directories
#
target_include_directories(${target}
        PRIVATE
        ${INDI_INCLUDE_DIR}
        ${DEFAULT_INCLUDE_DIRECTORIES}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}/source/include
        )

# 
# Libraries
# 
# Add the following to the section below to add further libs
target_link_libraries(${target}
        PRIVATE
        ${DEFAULT_LIBRARIES}
        ${Boost_LOG_LIBRARY}
        ${Boost_LOG_SETUP_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
		"$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>"
	${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${INDI_LIBRARIES}
        ${ZLIB_LIBRARIES}
	${DEFAULT_LINKER_OPTIONS}
        )


#
# Compile definitions
#
target_compile_definitions(${target}
        PRIVATE
        ${DEFAULT_COMPILE_DEFINITIONS}
        )


# 
# Compile options
#
target_compile_options(${target}
        PRIVATE
        ${DEFAULT_COMPILE_OPTIONS}
        )

# 
# Deployment
#

# Executable
install(TARGETS ${target}
        RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT examples
        BUNDLE DESTINATION ${INSTALL_BIN} COMPONENT examples
        )

# IMPORTANT: Otherwise C++11 is used...
set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
