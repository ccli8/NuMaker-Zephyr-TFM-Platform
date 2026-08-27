# SPDX-FileCopyrightText: Copyright (c) 2026 Nuvoton Technology Corporation
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(NuMaker-Zephyr-TFM-Platform)

set(APP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(NUVOTON_SECURE_LIB_DIR ${ZEPHYR_BASE}/modules/trusted-firmware-m/nuvoton)
if(NOT IS_DIRECTORY ${NUVOTON_SECURE_LIB_DIR})
  set(NUVOTON_SECURE_LIB_DIR ${ZEPHYR_MCUBOOT_MODULE_DIR}/boot/bootutil/zephyr/nuvoton)
endif()

target_include_directories(app
  PRIVATE
    ${APP_SOURCE_DIR}
)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})


if(CONFIG_ARM_SECURE_FIRMWARE)
  zephyr_include_directories(
    ${NUVOTON_SECURE_LIB_DIR}/include
    ${ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR}/interface/include
    )

  zephyr_library_sources(
    ${NUVOTON_SECURE_LIB_DIR}/src/tfm_platform_hal_ioctl_api.c
    ${ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR}/interface/src/tfm_platform_api.c
    ${ZEPHYR_TRUSTED_FIRMWARE_M_MODULE_DIR}/interface/src/tfm_tz_psa_ns_api.c
    )

  zephyr_sources_ifdef(CONFIG_SOC_SERIES_M335X
    ${ZEPHYR_NUVOTON_MODULE_DIR}/m335x/StdDriver/src/fmc.c
    )
endif()
