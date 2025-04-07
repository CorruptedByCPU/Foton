/*===============================================================================
 Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
===============================================================================*/

void moko_init( uint64_t argc, uint8_t *argv[] );
void moko_interface( void );
void moko_document_refresh( void );
void moko_document_line_refresh( uint8_t current );
void moko_document_line_restore( void );
void moko_document_parse( void );
uint8_t moko_key_ctrl( uint16_t key );
uint8_t moko_key( uint16_t key );
void moko_event( void );