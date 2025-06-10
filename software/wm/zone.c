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
		// zone already assigned?
		if( zone[ i ].z ) continue;	// yeah!

		// analyze zone against each object
		for( uint64_t j = 0; j < wm -> list_limit; j++ ) {
			if( ! (list[ j ] -> descriptor -> flags & LIB_WINDOW_FLAG_visible) ) { continue; };

			// zone overlapping?
			if( zone[ i ].x + zone[ i ].width <= list[ j ] -> x ) continue;		// no
			if( zone[ i ].y + zone[ i ].height <= list[ j ] -> y ) continue;	// no
			if( zone[ i ].x >= list[ j ] -> x + list[ j ] -> width ) continue;	// no
			if( zone [ i ].y >= list[ j ] -> y + list[ j ] -> height ) continue;	// no

			// left edge
			if( zone[ i ].x < list[ j ] -> x ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ i ];
				cut.width = list[ j ] -> x - zone[ i ].x;
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ i ].x = list[ j ] -> x;
				zone[ i ].width -= cut.width;
			}

			// top edge
			if( zone[ i ].y < list[ j ] -> y ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ i ];
				cut.height = list[ j ] -> y - zone[ i ].y;
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ i ].y = list[ j ] -> y;
				zone[ i ].height -= cut.height;
			}

			// right edge
			if( (zone[ i ].x + zone[ i ].width) > (list[ j ] -> x + list[ j ] -> width) ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ i ];
				cut.x = list[ j ] -> x + list[ j ] -> width;
				cut.width = (zone[ i ].x + zone[ i ].width) - (list[ j ] -> x + list[ j ] -> width);
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ i ].width -= cut.width;
			}

			// bottom edge
			if( (zone[ i ].y + zone[ i ].height) > (list[ j ] -> y + list[ j ] -> height) ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ i ];
				cut.y = list[ j ] -> y + list[ j ] -> height;
				cut.height = (zone[ i ].y + zone[ i ].height) - (list[ j ] -> y + list[ j ] -> height);
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ i ].height -= cut.height;
			}

			// by default, no inheritage
			zone[ i ].z = FALSE;

			// fill zone with given object
			zone[ i ].object = list[ j ];

			// next object?
			if( list[ j ] -> descriptor -> flags & LIB_WINDOW_FLAG_transparent ) {
				// inherite!
				zone[ i ].z = TRUE;

				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &zone[ i ], TRUE );
			
				// next object
				continue;
			}
			
			// next zone
			break;
		}
	}
}

uint64_t wm_zone_insert( struct WM_STRUCTURE_ZONE *current, uint8_t object ) {
	// discard zone if outside of cache area
	if( current -> x > wm -> canvas.width - 1 ) return EMPTY;
	if( current -> y > wm -> canvas.height - 1 ) return EMPTY;
	if( current -> x + current -> width < 0 ) return EMPTY;
	if( current -> y + current -> height < 0 ) return EMPTY;

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

	// inherite z flag
	zone -> z = current -> z;

	// object selected for zone?
	if( object ) zone -> object = current -> object;
	else	// no
		zone -> object = EMPTY;

	// zone inserted
	wm -> zone_limit++;

	return wm -> zone_limit - 1;
}

void wm_zone_substract( void ) {
	// properties of first entry inside zone list
	struct WM_STRUCTURE_ZONE *zone = wm -> zone;

	// properties of object list
	struct WM_STRUCTURE_OBJECT **list = wm -> list;

	// parse zones on list
	uint64_t a = 0;
	while( a < wm -> zone_limit ) {
		// analyze zone against each object
		uint8_t deleted = FALSE;
		uint64_t b = 0;
		while( b < wm -> zone_limit ) {
			// interference?
			if( a == b ) { b++; continue; }

			// zone overlapping?
			if( zone[ a ].x + zone[ a ].width <= zone[ b ].x ) { b++; continue; }	// no
			if( zone[ a ].y + zone[ a ].height <= zone[ b ].y ) { b++; continue; }	// no
			if( zone[ a ].x >= zone[ b ].x + zone[ b ].width ) { b++; continue; }	// no
			if( zone[ a ].y >= zone[ b ].y + zone[ b ].height ) { b++; continue; }	// no

			// left edge
			if( zone[ a ].x < zone[ b ].x ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ a ];
				cut.width = zone[ b ].x - zone[ a ].x;
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ a ].width -= cut.width;
				zone[ a ].x = zone[ b ].x;
			}

			// top edge
			if( zone[ a ].y < zone[ b ].y ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ a ];
				cut.height = zone[ b ].y - zone[ a ].y;
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ a ].height -= cut.height;
				zone[ a ].y = zone[ b ].y;
			}

			// right edge
			if( (zone[ a ].x + zone[ a ].width) > (zone[ b ].x + zone[ b ].width) ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ a ];
				cut.x = zone[ b ].x + zone[ b ].width;
				cut.width = (zone[ a ].x + zone[ a ].width) - (zone[ b ].x + zone[ b ].width);
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ a ].width -= cut.width;
			}

			// bottom edge
			if( (zone[ a ].y + zone[ a ].height) > (zone[ b ].y + zone[ b ].height) ) {
				// cut off part of zone
				struct WM_STRUCTURE_ZONE cut = zone[ a ];
				cut.y = zone[ b ].y + zone[ b ].height;
				cut.height = (zone[ a ].y + zone[ a ].height) - (zone[ b ].y + zone[ b ].height);
				wm_zone_insert( (struct WM_STRUCTURE_ZONE *) &cut, EMPTY );

				// new dimension of zone
				zone[ a ].height -= cut.height;
			}

			for( uint64_t c = a; c < wm -> zone_limit; c++ ) zone[ c ] = zone[ c + 1 ];
			wm -> zone_limit--;

			// again
			deleted = TRUE;
			break;
		}

		if( ! deleted ) a++;
	}
}
