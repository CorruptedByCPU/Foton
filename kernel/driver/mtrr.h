/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

#ifndef DRIVER_MTRR
	#define	DRIVER_MTRR

	uint64_t driver_mtrr_read( uint32_t msr );
	void driver_mtrr_write( uint32_t msr, uint64_t value );
#endif
