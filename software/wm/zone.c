/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_zone_insert( struct WM_STRUCTURE_ZONE *zone, uint8_t object ) {
	// discard zone if outside of workbench area
	if( zone -> x > wm_object_workbench -> width - 1 ) return;
	if( zone -> y > wm_object_workbench -> height - 1 ) return;
	if( zone -> x + zone -> width < 0 ) return;
	if( zone -> y + zone -> height < 0 ) return;

	// expand zone list if needed
	wm_zone_base_address = (struct WM_STRUCTURE_ZONE *) realloc( wm_zone_base_address, sizeof( struct WM_STRUCTURE_ZONE ) * (wm_zone_limit + 1) );

	// inset zone

	// truncate X axis
	if( zone -> x < 0 ) {
		// left side
		wm_zone_base_address[ wm_zone_limit ].width = zone -> width - (~zone -> x + 1);
		wm_zone_base_address[ wm_zone_limit ].x = 0;
	} else if( zone -> x + zone -> width > wm_object_workbench -> width ) {
		// right side
		wm_zone_base_address[ wm_zone_limit ].x = zone -> x;
		wm_zone_base_address[ wm_zone_limit ].width = zone -> width - ((zone -> x + zone -> width) - (int16_t) wm_object_workbench -> width);
	} else {
		// whole zone
		wm_zone_base_address[ wm_zone_limit ].x = zone -> x;
		wm_zone_base_address[ wm_zone_limit ].width = zone -> width;
	}

	// truncate Y axis
	if( zone -> y < 0 ) {
		// up side
		wm_zone_base_address[ wm_zone_limit ].height = zone -> height - (~zone -> y + 1);
		wm_zone_base_address[ wm_zone_limit ].y = 0;
	} else if( zone -> y + zone -> height > wm_object_workbench -> height ) {
		// bottom side
		wm_zone_base_address[ wm_zone_limit ].y = zone -> y;
		wm_zone_base_address[ wm_zone_limit ].height = zone -> height - ((zone -> y + zone -> height) - (int16_t) wm_object_workbench -> height);
	} else {
		// whole zone
		wm_zone_base_address[ wm_zone_limit ].y = zone -> y;
		wm_zone_base_address[ wm_zone_limit ].height = zone -> height;
	}

	// object selected for zone?
	if( object ) wm_zone_base_address[ wm_zone_limit ].object = zone -> object;
	else	// no
		wm_zone_base_address[ wm_zone_limit ].object = EMPTY;


	// zone inserted
	wm_zone_limit++;
}

void wm_zone( void ) {
	// parse zones on list
	for( uint64_t i = 0; i < wm_zone_limit; i++ ) {
		// object assigned to zone?
		if( wm_zone_base_address[ i ].object ) continue;	// yes

		// analyze zone against each object
		for( uint64_t j = 0; j < wm_list_limit; j++ ) {
			// ignore cursor object if exist
			if( wm_list_base_address[ j ] -> descriptor -> flags & WM_OBJECT_FLAG_cursor ) continue;

			// invisible object?
			if( ! (wm_list_base_address[ j ] -> descriptor -> flags & WM_OBJECT_FLAG_visible) ) continue;	// yes

			// zone and object share area?
			if( wm_list_base_address[ j ] -> x + wm_list_base_address[ j ] -> width < wm_zone_base_address[ i ].x ) continue;	// no
			if( wm_list_base_address[ j ] -> y + wm_list_base_address[ j ] -> height < wm_zone_base_address[ i ].y ) continue;	// no
			if( wm_list_base_address[ j ] -> x > wm_zone_base_address[ i ].x + wm_zone_base_address[ i ].width ) continue;	// no
			if( wm_list_base_address[ j ] -> y > wm_zone_base_address[ i ].y + wm_zone_base_address[ i ].height ) continue;	// no

			// modify zone up to object boundaries
			struct WM_STRUCTURE_ZONE zone = wm_zone_base_address[ i ];

			// left edge
			if( zone.x < wm_list_base_address[ j ] -> x ) {
				zone.width -= wm_list_base_address[ j ] -> x - zone.x;
				zone.x = wm_list_base_address[ j ] -> x;
			}

			// top edge
			if( zone.y < wm_list_base_address[ j ] -> y ) {
				zone.height -= wm_list_base_address[ j ] -> y - zone.y;
				zone.y = wm_list_base_address[ j ] -> y;
			}

			// right edge
			if( (zone.x + zone.width) > (wm_list_base_address[ j ] -> x + wm_list_base_address[ j ] -> width) ) {
				zone.width -= (zone.x + zone.width) - (wm_list_base_address[ j ] -> x + wm_list_base_address[ j ] -> width);
			}

			// bottom edge
			if( (zone.y + zone.height) > (wm_list_base_address[ j ] -> y + wm_list_base_address[ j ] -> height) ) {
				zone.height -= (zone.y + zone.height) - (wm_list_base_address[ j ] -> y + wm_list_base_address[ j ] -> height);
			}

			// fill the zone with the given object
			zone.object = wm_list_base_address[ j ];
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone, TRUE );
		}
	}
}