/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/math_extras.h>
#include <soc.h>
#include "tfm_platform_hal_ioctl_api.h"

static void test_flash(void);
static void test_sram(void);
static void test_register_lock(void);
static void test_module_reset(void);
static void test_mfp(void);
static void test_mfos(void);
static void test_usbphy(void);
static void test_sys(void);
static void test_module_clock(void);
static void test_clk(void);
static void test_fmc(void);

int main(void)
{
	/* Start of test */
	printf("TF-M Platform service test ...\n");

	test_flash();
	test_sram();
	test_register_lock();
	test_module_reset();
	test_mfp();
	test_mfos();
	test_usbphy();
	test_sys();
	test_module_clock();
	test_clk();
	test_fmc();

	/* End of test */
	printf("TF-M Platform service test ... DONE\n");

	return 0;
}

static void test_flash(void)
{
	/* Flash */
	printf("FMC_SECURE_REGION_SIZE=0x%08lx\n", FMC_SECURE_REGION_SIZE);
	printf("SCU_FNSADDR=0x%08x\n", SCU->FNSADDR);
	__ASSERT_NO_MSG(FMC_SECURE_END == SCU->FNSADDR);
}

static void test_sram(void)
{
	/* SRAM */
	printf("SCU_SECURE_SRAM_SIZE=0x%08lx\n", SCU_SECURE_SRAM_SIZE);
	printf("SCU_SRAMNSSET=0x%08x\n", SCU->SRAMNSSET);
	printf("SRAM section size=%d\n", SRAM_SECTION_SIZE);
	uint32_t sram_s_sec_num = u32_count_trailing_zeros(SCU->SRAMNSSET);
	uint32_t sram_s_size = SRAM_SECTION_SIZE * sram_s_sec_num;
	__ASSERT_NO_MSG(SCU_SECURE_SRAM_SIZE == sram_s_size);
}

static void test_register_lock(void)
{
	/* Lock/Unlock register */
	NVT_TFM_PLAT_IOCTL_NS(SYS_UnlockReg)();
	NVT_TFM_PLAT_IOCTL_NS(SYS_LockReg)();
	NVT_TFM_PLAT_IOCTL_NS(SYS_UnlockReg)();
	__ASSERT_NO_MSG(!SYS_IsRegLocked());
	NVT_TFM_PLAT_IOCTL_NS(SYS_LockReg)();
	__ASSERT_NO_MSG(SYS_IsRegLocked());
}

static void test_module_reset(void)
{
	/* Module reset */
	uint32_t mod_rst = EADC0_RST;
	NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_Deassert)(mod_rst);
	NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_Assert)(mod_rst);
	__ASSERT_NO_MSG(NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_IsAsserted(mod_rst)));
	NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_Deassert)(mod_rst);
	__ASSERT_NO_MSG(!NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_IsAsserted(mod_rst)));
}

static void test_mfp(void)
{
	uint32_t mfp_addr = (uint32_t)&SYS->GPF_MFP2;
	uint32_t mfp_val, mfp_val2;

	/* Write == Read */
	mfp_val = NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFPx_Read)(mfp_addr);
	NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFPx_Write)(mfp_addr, mfp_val);
	mfp_val2 = NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFPx_Read)(mfp_addr);
	printf("GPx_MFPx: addr=0x%08x, write=0x%08x, read=0x%08x\n", mfp_addr, mfp_val, mfp_val2);
	__ASSERT_NO_MSG(mfp_val2 == mfp_val);

#if defined(SCU_INIT_IONSSET5_VAL) && (SCU_INIT_IONSSET5_VAL == 0x00000600UL)
	mfp_val = 0x1F1F1F1FUL;
	NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFPx_Write)(mfp_addr, mfp_val);
	mfp_val2 = NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFPx_Read)(mfp_addr);
	printf("GPx_MFPx: addr=0x%08x, write=0x%08x, read=0x%08x\n", mfp_addr, mfp_val, mfp_val2);
	__ASSERT_NO_MSG(mfp_val2 == 0x001F1F00UL);
#else
	printf("GPx_MFPx: Set SCU_INIT_IONSSET5_VAL to 0x00000600UL for NS mask test\n");
#endif
}

static void test_mfos(void)
{
	/* GPx_MFOSx */
	uint32_t mfos_addr = (uint32_t)&SYS->GPF_MFOS;
	uint32_t mfos_val, mfos_val2;

	mfos_val = NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFOSx_Read)(mfos_addr);
	NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFOSx_Write)(mfos_addr, mfos_val);
	mfos_val2 = NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFOSx_Read)(mfos_addr);
	printf("GPx_MFOSx: addr=0x%08x, write=0x%08x, read=0x%08x\n", mfos_addr, mfos_val,
	       mfos_val2);
	__ASSERT_NO_MSG(mfos_val2 == mfos_val);

#if defined(SCU_INIT_IONSSET5_VAL) && (SCU_INIT_IONSSET5_VAL == 0x00000600UL)
	mfos_val = 0xFFFFFFFFUL;
	NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFOSx_Write)(mfos_addr, mfos_val);
	mfos_val2 = NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFOSx_Read)(mfos_addr);
	printf("GPx_MFOSx: addr=0x%08x, write=0x%08x, read=0x%08x\n", mfos_addr, mfos_val,
	       mfos_val2);
	__ASSERT_NO_MSG(mfos_val2 == 0x00000600UL);
#else
	printf("GPx_MFOSx: Set SCU_INIT_IONSSET5_VAL to 0x00000600UL for NS mask test\n");
#endif
}

static void test_usbphy(void)
{
	/* USBPHY */
	NVT_TFM_PLAT_IOCTL_NS(SYS_UnlockReg)();

	uint32_t usbphy_addr = (uint32_t)&SYS->USBPHY;
	uint32_t usbphy_val;

	usbphy_val = NVT_TFM_PLAT_IOCTL_NS(SYS_USBPHY_Read)(usbphy_addr);
	usbphy_val &= ~SYS_USBPHY_USBROLE_Msk;
	usbphy_val |= SYS_USBPHY_USBROLE_STD_USBD;
	NVT_TFM_PLAT_IOCTL_NS(SYS_USBPHY_Write)(usbphy_addr, usbphy_val);
	__ASSERT_NO_MSG((SYS->USBPHY & SYS_USBPHY_USBROLE_Msk) == SYS_USBPHY_USBROLE_STD_USBD);

	usbphy_val = NVT_TFM_PLAT_IOCTL_NS(SYS_USBPHY_Read)(usbphy_addr);
	usbphy_val &= ~SYS_USBPHY_USBROLE_Msk;
	usbphy_val |= SYS_USBPHY_USBROLE_STD_USBH;
	NVT_TFM_PLAT_IOCTL_NS(SYS_USBPHY_Write)(usbphy_addr, usbphy_val);
	__ASSERT_NO_MSG((SYS->USBPHY & SYS_USBPHY_USBROLE_Msk) == SYS_USBPHY_USBROLE_STD_USBH);

	NVT_TFM_PLAT_IOCTL_NS(SYS_LockReg)();
}

static void test_sys(void)
{
	/* Drop SYS_REG_Read/SYS_REG_Write */
#if 0
	NVT_TFM_PLAT_IOCTL_NS(SYS_UnlockReg)();

	uint32_t bodctl_addr = (uint32_t)&SYS->BODCTL;
	uint32_t bodctl_val;

	bodctl_val = NVT_TFM_PLAT_IOCTL_NS(SYS_REG_Read)(bodctl_addr);
	bodctl_val |= SYS_BODCTL_BODEN_Msk;
	NVT_TFM_PLAT_IOCTL_NS(SYS_REG_Write)(bodctl_addr, bodctl_val);
	__ASSERT_NO_MSG(SYS->BODCTL == bodctl_val);

	bodctl_val = NVT_TFM_PLAT_IOCTL_NS(SYS_REG_Read)(bodctl_addr);
	bodctl_val &= ~SYS_BODCTL_BODEN_Msk;
	NVT_TFM_PLAT_IOCTL_NS(SYS_REG_Write)(bodctl_addr, bodctl_val);
	__ASSERT_NO_MSG(SYS->BODCTL == bodctl_val);

	NVT_TFM_PLAT_IOCTL_NS(SYS_LockReg)();
#endif
}

static void test_module_clock(void)
{
	/* Module clock */
	NVT_TFM_PLAT_IOCTL_NS(CLK_SetModuleClock)(EADC0_MODULE, CLK_CLKSEL0_EADC0SEL_HCLK,
						  CLK_CLKDIV0_EADC0(12));
	NVT_TFM_PLAT_IOCTL_NS(CLK_EnableModuleClock)(EADC0_MODULE);
	NVT_TFM_PLAT_IOCTL_NS(CLK_DisableModuleClock)(EADC0_MODULE);
}

static void test_clk(void)
{
	/* Drop CLK_REG_Read/CLK_REG_Write */
#if 0
	NVT_TFM_PLAT_IOCTL_NS(SYS_UnlockReg)();

	uint32_t bodctl_addr = (uint32_t)&CLK->PWRCTL;
	uint32_t bodctl_val;

	bodctl_val = NVT_TFM_PLAT_IOCTL_NS(CLK_REG_Read)(bodctl_addr);
	bodctl_val |= CLK_PWRCTL_PDEN_Msk;
	NVT_TFM_PLAT_IOCTL_NS(CLK_REG_Write)(bodctl_addr, bodctl_val);
	__ASSERT_NO_MSG(CLK->PWRCTL == bodctl_val);

	bodctl_val = NVT_TFM_PLAT_IOCTL_NS(CLK_REG_Read)(bodctl_addr);
	bodctl_val &= ~CLK_PWRCTL_PDEN_Msk;
	NVT_TFM_PLAT_IOCTL_NS(CLK_REG_Write)(bodctl_addr, bodctl_val);
	__ASSERT_NO_MSG(CLK->PWRCTL == bodctl_val);

	NVT_TFM_PLAT_IOCTL_NS(SYS_LockReg)();
#endif
}

static void test_fmc(void)
{
	/* CLK */
	NVT_TFM_PLAT_IOCTL_NS(SYS_UnlockReg)();

	FMC_Open();
	FMC_ENABLE_AP_UPDATE();
	FMC_DISABLE_AP_UPDATE();
	FMC_Close();

	NVT_TFM_PLAT_IOCTL_NS(FMC_Open)();
	__ASSERT_NO_MSG(FMC_NS->ISPCTL & FMC_ISPCTL_ISPEN_Msk);

	NVT_TFM_PLAT_IOCTL_NS(FMC_ENABLE_AP_UPDATE)();
	__ASSERT_NO_MSG(FMC_NS->ISPCTL & FMC_ISPCTL_APUEN_Msk);

	NVT_TFM_PLAT_IOCTL_NS(FMC_DISABLE_AP_UPDATE)();
	__ASSERT_NO_MSG(!(FMC_NS->ISPCTL & FMC_ISPCTL_APUEN_Msk));

	NVT_TFM_PLAT_IOCTL_NS(FMC_Close)();
	__ASSERT_NO_MSG(!(FMC_NS->ISPCTL & FMC_ISPCTL_ISPEN_Msk));

	NVT_TFM_PLAT_IOCTL_NS(SYS_LockReg)();
}
