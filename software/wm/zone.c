/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void wm_zone( void ) {
	// properties of first entry inside zone list
	struct WM_STRUCTURE_ZONE *zone = wm -> zone;

	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// parse zones on list
	for( uint64_t i = 0; i < wm -> zone_limit; i++ ) {
		// object already assigned to zone?
		if( zone[ i ].object ) continue;	// yes

		// analyze zone against each object
		for( uint64_t j = 0; j < wm -> list_limit; j++ ) {
			// ignore cursor object
			if( list[ j ] -> descriptor -> flags & LIB_WINDOW_FLAG_cursor ) continue;

			// invisible object?
			if( ! (list[ j ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible) ) continue;	// yes

			// zone and object share area?
			if( list[ j ] -> x + list[ j ] -> width < zone[ i ].x ) continue;	// no
			if( list[ j ] -> y + list[ j ] -> height < zone[ i ].y ) continue;	// no
			if( list[ j ] -> x > zone[ i ].x + zone[ i ].width ) continue;	// no
			if( list[ j ] -> y > zone[ i ].y + zone[ i ].height ) continue;	// no

			// modify zone up to object boundaries
			struct WM_STRUCTURE_ZONE parse = zone[ i ];

			// left edge
			if( parse.x < list[ j ] -> x ) {
				parse.width -= list[ j ] -> x - parse.x;
				parse.x = list[ j ] -> x;
			}

			// top edge
			if( parse.y < list[ j ] -> y ) {
				parse.height -= list[ j ] -> y - parse.y;
				parse.y = list[ j ] -> y;
			}

			// right edge
			if( (parse.x + parse.width) > (list[ j ] -> x + list[ j ] -> width) ) {
				parse.width -= (parse.x + parse.width) - (list[ j ] -> x + list[ j ] -> width);
			}

			// bottom edge
			if( (parse.y + parse.height) > (list[ j ] -> y + list[ j ] -> height) ) {
				parse.height -= (parse.y + parse.height) - (list[ j ] -> y + list[ j ] -> height);
			}

			// fill the zone with the given object
			parse.object = list[ j ];
			wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &parse, TRUE );
		}
	}
}

void wm_zone_insert( struct WM_STRUCTURE_ZONE *current, uint8_t object ) {
	// discard zone if outside of cache area
	if( current -> x > wm -> canvas.width - 1 ) return;
	if( current -> y > wm -> canvas.height - 1 ) return;
	if( current -> x + current -> width < 0 ) return;
	if( current -> y + current -> height < 0 ) return;

	// inset new zone

	// properties of last entry inside zone list
	struct WM_STRUCTURE_ZONE *zone = &wm -> zone[ wm -> zone_limit ];

	// truncate X axis
	if( current -> x < 0 ) {
		// left side
		zone -> width = current -> width - (~current -> x + TRUE);
		zone -> x = 0;
	} else if( current -> x + current -> width > wm -> canvas.width ) {
		// right side
		zone -> x = current -> x;
		zone -> width = current -> width - ((current -> x + current -> width) - (int16_t) wm -> canvas.width);
	} else {
		// whole zone
		zone -> x = current -> x;
		zone -> width = current -> width;
	}

	// truncate Y axis
	if( current -> y < 0 ) {
		// up side
		zone -> height = current -> height - (~current -> y + TRUE);
		zone -> y = 0;
	} else if( current -> y + current -> height > wm -> canvas.height ) {
		// bottom side
		zone -> y = current -> y;
		zone -> height = current -> height - ((current -> y + current -> height) - (int16_t) wm -> canvas.height);
	} else {
		// whole zone
		zone -> y = current -> y;
		zone -> height = current -> height;
	}

	// object selected for zone?
	if( object ) zone -> object = current -> object;
	else	// no
		zone -> object = EMPTY;

	// zone inserted
	wm -> zone_limit++;
}
