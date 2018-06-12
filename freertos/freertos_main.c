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

//ulong monitor_flash_len;

void vMainAssertCalled( const char *pcFileName, uint32_t ulLineNumber )
{
	#if 1 //RyanYao
	//printk( "ASSERT!  Line %lu of file %s\r\n", ulLineNumber, pcFileName );
	//debug( "ASSERT!  Line %lu of file %s\r\n", ulLineNumber, pcFileName );
	debug( "ASSERT!  Line %u of file %s\r\n", ulLineNumber, pcFileName );
	
	#endif
	
	taskENTER_CRITICAL();
	for( ;; );
}

void hello_world_task(void *p)
{
	//configASSERT( ( ( uint64_t ) p) == 1 );
	while(1) {
		//printk("Hello World Task! p=%x", p);
     	//print_uart0("hello_world_task() Hello world!\n");
     	debug("hello_world_task() Hello world!\n");
		//vTaskDelay(1000);
	}
}

void freertos_main(void)
{
	debug("Ryan: %s()\n", __func__); //RyanYao
#if 1
	/* Create Tasks */
	xTaskCreate(hello_world_task, "hello_task", 64, 0, 1, 0);

	/* Start the scheduler */	
	vTaskStartScheduler();
#else
	portDISABLE_INTERRUPTS();
	debug("Ryan: %s() disable IRQ\n", __func__); //RyanYao
	portENABLE_INTERRUPTS();
	debug("Ryan: %s() enable IRQ\n", __func__); //RyanYao
#endif
}
