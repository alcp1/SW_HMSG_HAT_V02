# Include rules for compiling and linking
include("${CMAKE_CURRENT_LIST_DIR}/.generated/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/.generated/file.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/.generated/toolchain.cmake")

# Set outputs
set(My_image_name "SW_HMSG_HAT_V02.production.elf")
set(My_image_base_name "SW_HMSG_HAT_V02.production")
set(My_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/SW_HMSG_HAT_V02")
set(My_map_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../_build/SW_HMSG_HAT_V02/default")

# Main target for this project
add_executable(MyImage ${SW_HMSG_HAT_V02_default_library_list})

# Target properties - same as MPLAB X
set_target_properties(MyImage PROPERTIES
    OUTPUT_NAME "${My_image_base_name}"
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${My_output_dir}")

# Add the link options from the rule file.
SW_HMSG_HAT_V02_default_link_rule(MyImage)

#set OBJCOPY
set(MYOBJCOPY "${MP_CC_DIR}/avr-objcopy.exe" CACHE FILEPATH "Path to the object copy program")

# Post-build step
add_custom_command(
    TARGET MyImage POST_BUILD
    # Copy map
    COMMAND ${CMAKE_COMMAND} -E copy ${My_map_dir}/mem.map ${My_output_dir}/${My_image_base_name}.map
    # Copy memory
    COMMAND ${CMAKE_COMMAND} -E copy ${My_map_dir}/memoryfile.xml ${My_output_dir}/memoryfile.xml
    # Generate LST
    COMMAND ${OBJDUMP} -h -S ${My_output_dir}/${My_image_base_name}.elf > ${My_output_dir}/${My_image_base_name}.lst
    # Generate HEX
    COMMAND ${MYOBJCOPY} -O ihex ${My_output_dir}/${My_image_base_name}.elf ${My_output_dir}/${My_image_base_name}.hex
)

