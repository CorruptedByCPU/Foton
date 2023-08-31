/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef	KERNEL_RTC
	#define	KERNEL_RTC

	#define	KERNEL_RTC_IRQ_number					0x08
	#define	KERNEL_RTC_IO_APIC_register				KERNEL_IO_APIC_iowin + (KERNEL_RTC_IRQ_number * 0x02)

	#define	KERNEL_RTC_Hz						1024

	#define	KERNEL_RTC_PORT_command					0x0070
	#define	KERNEL_RTC_PORT_data					0x0071

	#define	KERNEL_RTC_REGISTER_seconds				0x00
	#define	KERNEL_RTC_REGISTER_minutes				0x02
	#define	KERNEL_RTC_REGISTER_hour				0x04
	#define	KERNEL_RTC_REGISTER_weekday				0x06
	#define	KERNEL_RTC_REGISTER_day_of_month			0x07
	#define	KERNEL_RTC_REGISTER_month				0x08
	#define	KERNEL_RTC_REGISTER_year				0x09
	
	#define	KERNEL_RTC_STATUS_REGISTER_A				0x0A
	#define	KERNEL_RTC_STATUS_REGISTER_B				0x0B
	#define	KERNEL_RTC_STATUS_REGISTER_C				0x0C

	#define	KERNEL_RTC_STATUS_REGISTER_A_rate			0b00000110	// 1024 Hz
	#define	KERNEL_RTC_STATUS_REGISTER_A_divider			0b00100000	// 32768 kHz
	#define	KERNEL_RTC_STATUS_REGISTER_A_update_in_progress		0b10000000

	#define	KERNEL_RTC_STATUS_REGISTER_B_daylight_savings		0b00000001
	#define	KERNEL_RTC_STATUS_REGISTER_B_24_hour_mode		0b00000010
	#define	KERNEL_RTC_STATUS_REGISTER_B_data_mode_binary		0b00000100	// no BCD
	#define	KERNEL_RTC_STATUS_REGISTER_B_square_wave_output		0b00001000
	#define	KERNEL_RTC_STATUS_REGISTER_B_update_ended_interrupt	0b00010000	// announce with interrupt when controller exited modification mode
	#define	KERNEL_RTC_STATUS_REGISTER_B_alarm_interrupt		0b00100000
	#define	KERNEL_RTC_STATUS_REGISTER_B_periodic_interrupt		0b01000000
	#define	KERNEL_RTC_STATUS_REGISTER_B_update_in_progress		0b10000000

	// external routine (assembly language)
	extern void kernel_rtc_entry( void );
#endif