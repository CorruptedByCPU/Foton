/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

// kernel environment variables/functions
struct KERNEL *kernel = EMPTY;

// our limine requests

static volatile struct limine_framebuffer_request limine_framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

static volatile struct limine_memmap_request limine_memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

static volatile struct limine_rsdp_request limine_rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0
};

static volatile struct limine_kernel_file_request limine_kernel_file_request = {
	.id = LIMINE_KERNEL_FILE_REQUEST,
	.revision = 0
};

static volatile struct limine_kernel_address_request limine_kernel_address_request = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
};

static volatile struct limine_smp_request limine_smp_request = {
	.id = LIMINE_SMP_REQUEST,
	.revision = 0
};

static volatile struct limine_module_request limine_module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0
};