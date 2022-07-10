if(OTA_UPDATES)
    set(INCLUDE_OTA_UPDATES ./ota.c)
endif()

idf_component_register(SRCS ./main.c ./sensors.c ./backend.c ./wifi.c
                       ${INCLUDE_OTA_UPDATES} INCLUDE_DIRS ".")

add_definitions(-DPROJECT_NAME="${CMAKE_PROJECT_NAME}" -DPROJECT_VER="${PROJECT_VER}"
                -DSENSORS_SCALE_${SENSORS_SCALE} -DOTA_UPDATES_${OTA_UPDATES})
