/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void dhcp_option_add( uint8_t *option ) {
	// calculate new DHCP message length
	dhcp_length += option[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

	// extend area of dhcp message
	dhcp = (struct DHCP_STRUCTURE *) realloc( dhcp, dhcp_length );

	// set pointer at begining of options list
	uint8_t *options = (uint8_t *) &dhcp -> option;

	// move pointer at end of all options
	while( *options != DHCP_OPTION_TYPE_END ) options += options[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

	// insert option at end of options list
	for( uint16_t i = 0; i < option[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT ); i++ ) options[ i ] = option[ i ];

	// close options list
	options[ option[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT ) ] = DHCP_OPTION_TYPE_END;
}

void dhcp_option_remove( uint8_t type ) {
	// next option offset
	uint16_t offset = EMPTY;

	// new options length
	uint16_t length = dhcp_length - sizeof( struct DHCP_STRUCTURE );

	// set pointer at begining of options list
	uint8_t *option = (uint8_t *) &dhcp -> option;

	// search for option to remove
	while( *option != DHCP_OPTION_TYPE_END ) {
		// next option offset
		offset = option[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );

		// length of remaining options list
		length -= offset;

		// selected option found?
		if( *option == type ) break;	// yes
		
		// move pointer to next option
		option += offset;
	}

	// if found, remove option from options list
	if( offset ) for( uint16_t i = 0; i <= length; i++ ) option[ i ] = option[ i + offset ];
}

uint8_t *dhcp_option( uint8_t *options, uint8_t type ) {
	// search for option
	while( *options != DHCP_OPTION_TYPE_END ) {
		// selected option found?
		if( *options == type ) return options;	// yes
		
		// move pointer to next option
		options += options[ 1 ] + sizeof( struct DHCP_STRUCTURE_OPTION_DEFAULT );
	}

	// not found
	return EMPTY;
}