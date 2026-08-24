# Add componens
add_subdirectory(component/ili9486)
# Select only platforms that are implemented and buildable.
if(PLATFORM_TARGET STREQUAL "PLATFORM_NUCLEOF411RE")
    add_subdirectory(nucleof411re)
    set(PLATFORM_TO_LINK "PLATFORM_NUCLEOF411RE")
else()
    message(FATAL_ERROR
        "Unsupported PLATFORM_TARGET='${PLATFORM_TARGET}'. "
        "The validated platform is PLATFORM_NUCLEOF411RE."
    )
endif()

# Define generic plaftorm interface to link to
add_library(PLATFORM INTERFACE)
target_link_libraries(PLATFORM INTERFACE ${PLATFORM_TO_LINK} )
target_include_directories(PLATFORM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
