/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

int64_t socket;

struct STD_NETWORK_STRUCTURE_INTERFACE eth0;

struct DHCP_STRUCTURE *dhcp_discover = EMPTY;
struct DHCP_STRUCTURE *dhcp_request = EMPTY;

uint16_t dhcp_discover_length = sizeof( struct DHCP_STRUCTURE );
uint16_t dhcp_request_length = sizeof( struct DHCP_STRUCTURE );

uint8_t dhcp_action = Discover;
uint32_t dhcp_ipv4_server = EMPTY;
uint32_t dhcp_ipv4_client = EMPTY;