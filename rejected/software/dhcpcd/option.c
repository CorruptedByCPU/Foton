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