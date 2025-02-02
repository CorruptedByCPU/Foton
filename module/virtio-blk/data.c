/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

struct KERNEL *kernel = EMPTY;

struct MODULE_VIRTIO_BLK_STRUCTURE *module_virtio_blk;

uint8_t module_virtio_device_legacy = TRUE;
uint8_t module_virtio_device_transitional = FALSE;
uint8_t module_virito_device_modern = FALSE;