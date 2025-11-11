# Add componens
add_subdirectory(component/ili9486)
# Add all platforms
add_subdirectory(nucleof411re)

# Establish build properties based on target plaftorm
set(PLATFORM_TO_LINK "")
if(PLATFORM_TARGET STREQUAL "PLATFORM_NUCLEOF411RE")
    set(PLATFORM_TO_LINK "PLATFORM_NUCLEOF411RE")
elseif(PLATFORM_TARGET STREQUAL "PLATFORM_NUCLEOF412ZG")
    set(PLATFORM_TO_LINK "PLATFORM_NUCLEOF412ZG")
else()
    message(FATAL_ERROR "PLATFORM_TARGET must be provided with a valid target platform.")
endif()

# Define generic plaftorm interface to link to
add_library(PLATFORM INTERFACE)
target_link_libraries(PLATFORM INTERFACE ${PLATFORM_TO_LINK} )
target_include_directories(PLATFORM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
