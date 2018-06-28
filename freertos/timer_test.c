// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 */

#include <common.h>
#include <api.h>
/* TODO: can we just include all these headers whether needed or not? */
#if defined(CONFIG_CMD_BEDBUG)
#include <bedbug/type.h>
#endif
#include <command.h>
#include <console.h>
#include <dm.h>
#include <environment.h>
#include <fdtdec.h>
#include <ide.h>
#include <initcall.h>
#if defined(CONFIG_CMD_KGDB)
#include <kgdb.h>
#endif
#include <malloc.h>
#include <mapmem.h>
#ifdef CONFIG_BITBANGMII
#include <miiphy.h>
#endif
#include <mmc.h>
#include <nand.h>
#include <of_live.h>
#include <onenand_uboot.h>
#include <scsi.h>
#include <serial.h>
#include <spi.h>
#include <stdio_dev.h>
#include <timer.h>
#include <trace.h>
#include <watchdog.h>
#ifdef CONFIG_ADDR_MAP
#include <asm/mmu.h>
#endif
#include <asm/sections.h>
#include <dm/root.h>
#include <linux/compiler.h>
#include <linux/err.h>
#include <efi_loader.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

DECLARE_GLOBAL_DATA_PTR;

#define ARM_ARCH_TIMER_ENABLE			(1 << 0)
#define ARM_ARCH_TIMER_IMASK			(1 << 1)
#define ARM_ARCH_TIMER_ISTATUS			(1 << 2)


#if 0 //RyanYao
/* MMIO */
// Memory-Mapped I/O output
static inline void mmio_write(intptr_t reg, uint32_t data)
{
    *(volatile uint32_t*) reg = data;
}

// Memory-Mapped I/O input
static inline uint32_t mmio_read(intptr_t reg)
{
    return *(volatile uint32_t*) reg;
}
#endif

/* xxxxxxx */
uint32_t read_current_el(void)
{
/* 
EL, bits [3:2]
	Current exception level. Possible values of this field are:
	00 EL0
	01 EL1
	10 EL2
	11 EL3
*/
	uint32_t val;
	asm volatile ("mrs %0, CurrentEL" : "=r" (val));
	return (val & 0xC >> 2);
}

uint32_t read_cntkctl(void)
{
	uint32_t val;
	asm volatile ("mrs %0, CNTKCTL_EL1" : "=r" (val));
	return val;
}

uint32_t read_cntfrq(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
	return val;
}

/* CNPT ========================================*/
void write_cntp_cval(uint32_t val)
{
	asm volatile ("msr cntp_cval_el0, %0" :: "r" (val));
	return;
}

uint32_t read_cntp_cval(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntp_cval_el0" : "=r" (val));
	return val;
}

uint32_t read_cntp_ctl(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntp_ctl_el0" : "=r" (val));
	return val;
}

uint32_t read_cntp_tval(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntp_tval_el0" : "=r" (val));
	return val;
}

void enable_cntp(void)
{
	uint32_t cntp_ctl;
	cntp_ctl = 1;
	asm volatile ("msr cntp_ctl_el0, %0" :: "r" (cntp_ctl));
}

void disable_cntp(void)
{
	uint32_t cntp_ctl;
	cntp_ctl = 0;
	asm volatile ("msr cntp_ctl_el0, %0" :: "r" (cntp_ctl));
}

uint32_t read_cntpct(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntpct_el0" : "=r" (val));
	return val;
}

/* CNPV ========================================*/
/*
uint64_t read_cntv_cval(void)
{
	uint64_t val;
	asm volatile ("mrs %0, cntv_cval_el0" : "=r" (val));
	return val;
}

void write_cntv_tval(uint32_t val)
{
	asm volatile ("msr cntv_tval_el0, %0" :: "r" (val));
	return;
}
uint32_t read_cntv_tval(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntv_tval_el0" : "=r" (val));
	return val;
}
*/

uint64_t read_cntv_cval(void)
{
	uint64_t val;
	asm volatile ("mrs %0, cntv_cval_el0" : "=r" (val));
	return val;
}

void write_cntv_cval(uint64_t val)
{
	asm volatile ("msr cntv_cval_el0, %0" :: "r" (val));
	return;
}

uint32_t read_cntv_ctl(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntv_ctl_el0" : "=r" (val));
	return val;
}

void disable_cntv(void)
{
	uint32_t val;

	val = read_cntv_ctl();
	val &= ARM_ARCH_TIMER_ENABLE;
	asm volatile ("msr cntv_ctl_el0, %0" :: "r" (val));
}

void enable_cntv(void)
{
	uint32_t val;

	val = read_cntv_ctl();
	val |= ARM_ARCH_TIMER_ENABLE;
	asm volatile ("msr cntv_ctl_el0, %0" :: "r" (val));
}

uint64_t read_cntvct(void)
{
	uint64_t val;
	asm volatile ("mrs %0, cntvct_el0" : "=r" (val));
	return val;
}

/* CNPT ========================================*/
static inline void io_halt(void)
{
	asm volatile ("wfi");
}

void timer_test(void)
{
	uint32_t cntfrq;
	uint64_t ticks, current_cnt;

	printf("Ryan: %s()\n", __func__);

	/* enable the GIC distributor */
	printf("GICD_CTLR = %u\n", readl(0x08000000));
	writel(0x61, 0x08000000);
	printf("GICD_CTLR = %u\n", readl(0x08000000));

	// Always disable the timer
	disable_cntv();
	cntfrq = read_cntfrq();

	// Next timer IRQ is after 1 sec.
	ticks = cntfrq;
	
	// Get value of the current timer
	current_cnt = read_cntvct();

	// Set the interrupt in Current Time + TimerTick
	write_cntv_cval(current_cnt + ticks);

	printf("ticks=%u\n", cntfrq);
	printf("current_cnt=%llu\n", current_cnt);
	printf("cntv_cval=%llu\n", read_cntv_cval());
	
	// Enable the timer
	enable_cntv();
	portENABLE_INTERRUPTS();

	while(1){
		printf("current_cnt=%llu, cntv_ctl=%u\n", read_cntvct(), read_cntv_ctl());
#if 0 // Wait Interrupt
		io_halt();
#else // Observe CNTP_CTL_EL0[2]: ISTATUS
		uint32_t val;
		val = read_cntpct();
		printf("CNTPCT_EL0 = %u\n", val);
		val = read_cntp_ctl();
		printf("CNTP_CTL_EL0 = %u\n", val);
		if (val != 0)
			break;
#endif
	}
}

#if 0 
void timer_test(void)
{
	uint32_t cntfrq, val;

	printf("Ryan: %s()\n", __func__); //RyanYao
	cntfrq = read_cntfrq();
	printf("CNTFRQ_EL0 = %u\n", cntfrq);
	val = read_current_el();
	printf("Current exception level = %u\n", val);
	val = read_cntkctl();
	printf("CNTKCTL = %u\n", val);
	printf("--------------------------\n");
	val = read_cntp_cval();
	printf("CNTP_CVAL_EL0 = %u\n", val);
	val = read_cntp_ctl();
	printf("CNTP_CTL_EL0 = %u\n", val);
	val = read_cntp_tval();
	printf("CNTP_TVAL_EL0 = %u\n", val);
	printf("--------------------------\n");
	val = read_cntv_cval();
	printf("CNTV_CVAL_EL0 = %u\n", val);
	val = read_cntv_ctl();
	printf("CNTV_CTL_EL0 = %u\n", val);
	val = read_cntv_tval();
	printf("CNTV_TVAL_EL0 = %u\n", val);

/*
	write_cntv_tval(cntfrq);
	val = read_cntv_tval();
	printf("CNTV_TVAL_EL0 = %u\n", val);
	enable_cntv();
	enable_irq();
*/
	while(1){
/*
		val = read_cntpct();
		printf("CNTPCT_EL0 = %u\n", val);
		val = read_cntp_ctl();
		printf("CNTP_CTL_EL0 = %u\n", val);
		if (val != 0)
			break;
*/
		io_halt();
	}
}
#endif
