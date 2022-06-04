add_subdirectory(XYModem/)
add_library(XYModem INTERFACE)

if(${BUILD_EXAMPLES} OR ${BUILD_BACKEND} OR ${BUILD_TESTS})
	add_subdirectory(Backends/)
	target_link_libraries(XYModem INTERFACE XYModemCore XYModemBackends)
else()
	target_link_libraries(XYModem INTERFACE XYModemCore)
endif()
