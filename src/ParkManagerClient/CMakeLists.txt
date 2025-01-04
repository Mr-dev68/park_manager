project(park_manager_client)


file(GLOB  SOURCES "*.cpp" "${CMAKE_SOURCE_DIR}/src/CommonLibraries/*.cpp")
add_executable(${PROJECT_NAME}  ${SOURCES})

target_include_directories (${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src/CommonLibraries/json
	${CMAKE_SOURCE_DIR}/src/CommonLibraries/includes
	${CMAKE_SOURCE_DIR}/src/CommonLibraries/includes/nlohmann
	${CMAKE_SOURCE_DIR}/src/CommonLibraries
	)
###############################################################################

target_link_libraries(${PROJECT_NAME}
	pthread
	rt
)