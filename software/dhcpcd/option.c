/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

uint16_t dhcp_option_add( struct DHCP_STRUCTURE *dhcp, uint16_t length, uint8_t *option ) {
	// option length in Bytes
	uint16_t option_length = option[ TRUE ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

	// extend area of dhcp message
	dhcp = (struct DHCP_STRUCTURE *) realloc( dhcp, length + option_length );

	// set pointer at begining of options list
	uint8_t *options = (uint8_t *) &dhcp -> options;

	// move pointer at end of all options
	while( *options != DHCP_OPTION_TYPE_END ) options += options[ TRUE ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

	// insert option at end of options list
	for( uint16_t i = 0; i < option_length; i++ ) options[ i ] = option[ i ];

	// close options list
	options[ option_length ] = DHCP_OPTION_TYPE_END;

	// new DHCP structure length
	return length + option_length;
}

uint16_t dhcp_option_remove( struct DHCP_STRUCTURE *dhcp, uint16_t length, uint8_t type ) {
	// next option offset
	uint16_t offset = EMPTY;

	// removed option length
	uint8_t option_length = EMPTY;

	// set pointer at begining of options list
	uint8_t *option = (uint8_t *) &dhcp -> options;

	// search for option to remove
	while( *option != DHCP_OPTION_TYPE_END ) {
		// next option offset
		offset = option[ TRUE ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

		// selected option, found?
		if( *option == type ) break;	// yes
		
		// move pointer to next option
		option += offset;
	}

	// if found, remove option from options list
	if( *option == type ) {
		// option length for remove
		option_length = option[ TRUE ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

		// remove option from options list
		for( uint16_t i = 0; i <= (length - sizeof( struct DHCP_STRUCTURE )) - offset; i++ ) option[ i ] = option[ i + option_length ];
	}

	// new DHCP structure length
	return length - option_length;
}

uint8_t *dhcp_option( uint8_t *options, uint8_t type ) {
	// search for option
	while( *options != DHCP_OPTION_TYPE_END ) {
		// selected option found?
		if( *options == type ) return options;	// yes
		
		// move pointer to next option
		options += options[ TRUE ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );
	}

	// not found
	return EMPTY;
}