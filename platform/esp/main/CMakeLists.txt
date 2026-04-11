file(GLOB CORE_SRCS "../../../src/*.c")
file(GLOB PLAT_SRCS "*.c")

idf_component_register(
    SRCS ${PLAT_SRCS} ${CORE_SRCS}
    INCLUDE_DIRS "../include" "../../../include"
)
