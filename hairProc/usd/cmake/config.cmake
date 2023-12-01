get_filename_component(CUSTOM_SCHEMAS_DIRECTORY "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET vik::vik)
    include("${CUSTOM_SCHEMAS_DIRECTORY}/config.cmake")
endif()