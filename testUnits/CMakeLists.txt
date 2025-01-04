


project(TestUntis)

file(GLOB SOURCES "*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/ParkManagerUnitTests/*.cpp" "${CMAKE_SOURCE_DIR}/src/ParkManager/ParkManagerServer.cpp" "${CMAKE_SOURCE_DIR}/src/ParkManager/WebSocketServer.cpp" "${CMAKE_SOURCE_DIR}/src/ParkManagerClient/Gate.cpp" "${CMAKE_SOURCE_DIR}/src/ParkManagerClient/WebSocketClient.cpp" "${CMAKE_SOURCE_DIR}/src/CommonLibraries/*.cpp")
add_executable(${PROJECT_NAME}    ${SOURCES})

target_include_directories (${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/ParkManagerUnitTests
	${CMAKE_SOURCE_DIR}/src/CommonLibraries/json
	${CMAKE_SOURCE_DIR}/src/CommonLibraries/cute
	${CMAKE_SOURCE_DIR}/src/CommonLibraries/includes
	${CMAKE_SOURCE_DIR}/src/CommonLibraries/includes/nlohmann
	${CMAKE_SOURCE_DIR}/src/CommonLibraries
	${CMAKE_SOURCE_DIR}/src/ParkManagerClient
	${CMAKE_SOURCE_DIR}/src/ParkManager
	
	)


target_link_libraries("TestUntis"
	pthread
	rt
)