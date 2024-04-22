/*==============================================================================
Copyright (C) Andrzej Adamczyk (at https://blackdev.org/). All rights reserved.
==============================================================================*/

#ifndef	DRIVER_E1000
	#define	DRIVER_E1000

        #define	DRIVER_E1000_VENDOR_AND_DEVICE		0x100E8086

        #define	DRIVER_E1000_CACHE_SIZE_page		1

        #define	DRIVER_E1000_CTRL_FD			0x00000001	// Full-Duplex
        #define	DRIVER_E1000_CTRL_LRST			0x00000008	// Link Reset
        #define	DRIVER_E1000_CTRL_ASDE			0x00000020	// Auto-Speed Detection Enable
        #define	DRIVER_E1000_CTRL_SLU			0x00000040	// Set Link Up
        #define	DRIVER_E1000_CTRL_ILOS			0x00000080	// Invert Loss-of-Signal (LOS).
        #define	DRIVER_E1000_CTRL_SPEED_BIT_8		0x00000100	// Speed selection
        #define	DRIVER_E1000_CTRL_SPEED_BIT_9		0x00000200	// Speed selection
        #define	DRIVER_E1000_CTRL_FRCSPD		0x00000800	// Force Speed
        #define	DRIVER_E1000_CTRL_FRCPLX		0x00001000	// Force Duplex
        #define	DRIVER_E1000_CTRL_SDP0_DATA		0x00040000	// SDP0 Data Value
        #define	DRIVER_E1000_CTRL_SDP1_DATA		0x00080000	// SDP1 Data Value
        #define	DRIVER_E1000_CTRL_ADVD3WUC		0x00100000	// D3Cold Wakeup Capability Advertisement Enable
        #define	DRIVER_E1000_CTRL_EN_PHY_PWR_MGMT	0x00200000	// PHY Power-Management Enable
        #define	DRIVER_E1000_CTRL_SDP0_IODIR		0x00400000	// SDP0 Pin Directionality
        #define	DRIVER_E1000_CTRL_SDP1_IODIR		0x00800000	// SDP1 Pin Directionality
        #define	DRIVER_E1000_CTRL_RST			0x04000000	// Device Reset
        #define	DRIVER_E1000_CTRL_RFCE			0x08000000	// Receive Flow Control Enable
        #define	DRIVER_E1000_CTRL_TFCE			0x10000000	// Transmit Flow Control Enable
        #define	DRIVER_E1000_CTRL_VME			0x40000000	// VLAN Mode Enable
        #define	DRIVER_E1000_CTRL_PHY_RST		0x7FFFFFFF	// NASM ERROR => 0x80000000	// PHY Reset

        #define	DRIVER_E1000_ICR_txqe			0b0000000000000010	// Transmit Queue Empty
        #define	DRIVER_E1000_ICR_rxt0			0b0000000010000000	// Receiver Timer Interrupt

        #define DRIVER_E1000_ICS_txdw			0b00000000000000001	// Transmit Descriptor Written Back
        #define DRIVER_E1000_ICS_txqe			0b00000000000000010	// Transmit Queue Empty
        #define DRIVER_E1000_ICS_lsc			0b00000000000000100	// Link Status Change
        #define DRIVER_E1000_ICS_rxseq			0b00000000000001000	// Receive Sequence Error
	#define DRIVER_E1000_ICS_rxdmt0			0b00000000000010000	// Receive Descriptor Minimum Threshold Reached
        #define DRIVER_E1000_ICS_rxo			0b00000000001000000	// Receiver FIFO Overrun
        #define DRIVER_E1000_ICS_rxt0			0b00000000010000000	// Receiver Timer Interrupt
        #define DRIVER_E1000_ICS_gpi			0b00110000000000000	// General Purpose Interrups
        #define DRIVER_E1000_ICS_txd_low		0b01000000000000000	// Transmit Descriptor Low Threshold
        #define DRIVER_E1000_ICS_srpd			0b10000000000000000	// Small Receive Packet Detection

        #define	DRIVER_E1000_RCTL_EN			0b000000000000000000000000010	// Receiver Enable
        #define	DRIVER_E1000_RCTL_SBP			0b000000000000000000000000100	// Store Bad Packets
        #define	DRIVER_E1000_RCTL_UPE			0b000000000000000000000001000	// Unicast Promiscuaus Enabled
        #define	DRIVER_E1000_RCTL_MPE			0b000000000000000000000010000	// Multicast Promiscuous Enabled
        #define	DRIVER_E1000_RCTL_LPE			0b000000000000000000000100000	// Long Packet Reception Enable
        #define	DRIVER_E1000_RCTL_LBM			0b000000000000000000011000000	// Loopback mode
        #define	DRIVER_E1000_RCTL_BAM			0b000000000001000000000000000	// Broadcast Accept Mode
	#define DRIVER_E1000_RCTL_BSIZE_4096_BYTE	0b000000000110000000000000000	// 
        #define	DRIVER_E1000_RCTL_BSEX			0b010000000000000000000000000	// Receive Buffer Size multiply by 16
        #define	DRIVER_E1000_RCTL_SECRC			0b100000000000000000000000000	// Strip Ethernet CRC from incoming packet

        #define	DRIVER_E1000_TXCW_TXCONFIGWORD_BIT_5	0x00000020	// Full Duplex
        #define	DRIVER_E1000_TXCW_TXCONFIGWORD_BIT_6	0x00000040	// Half Duplex
        #define	DRIVER_E1000_TXCW_TXCONFIGWORD_BIT_7	0x00000080	// Pause
        #define	DRIVER_E1000_TXCW_TXCONFIGWORD_BIT_8	0x00000100	// Pause
        #define	DRIVER_E1000_TXCW_TXCONFIGWORD_BIT_12	0x00001000	// Remote fault indication
        #define	DRIVER_E1000_TXCW_TXCONFIGWORD_BIT_13	0x00002000	// Remote fault indication
        #define	DRIVER_E1000_TXCW_TXCONFIGWORD_BIT_15	0x00008000	// Next page rest
        #define	DRIVER_E1000_TXCW_TXCONFIGWORD		0x40000000	// Transmit Config Control bit
        #define	DRIVER_E1000_TXCW_ANE			0x80000000	// Auto-Negotiation Enable

        #define	DRIVER_E1000_TCTL_EN			0x00000002	// Transmit Enable
        #define	DRIVER_E1000_TCTL_PSP			0x00000008	// Pad Short Packets
        #define	DRIVER_E1000_TCTL_CT			0x00000100	// Collision Threshold
        #define	DRIVER_E1000_TCTL_COLD			0x00040000	// Full-Duplex – 64-byte time
        #define	DRIVER_E1000_TCTL_SWXOFF		0x00400000	// software OFF Transmission
        #define	DRIVER_E1000_TCTL_RTLC			0x01000000	// Re-transmit on Late Collision
        #define	DRIVER_E1000_TCTL_NRTU			0x02000000	// No Re-transmit on underrun (82544GC/EI only)

        #define	DRIVER_E1000_TDESC_STATUS_TU		0x00000008	// Transmit Underrun
        #define	DRIVER_E1000_TDESC_STATUS_LC		0x00000004	// Late Collision
        #define	DRIVER_E1000_TDESC_STATUS_EC		0x00000002	// Excess Collision
        #define	DRIVER_E1000_TDESC_STATUS_DD		0x00000001	// Descriptor Done
        #define	DRIVER_E1000_TDESC_CMD_IDE		0x00000080	// Interrupt Delay Enable
        #define	DRIVER_E1000_TDESC_CMD_VLE		0x00000040	// VLAN Packet Enable
        #define	DRIVER_E1000_TDESC_CMD_DEXT		0x00000020	// Extension (0b for legacy mode)
        #define	DRIVER_E1000_TDESC_CMD_RPS		0x00000010	// Report Packet Send (reserved for 82544GC/EI only)
        #define	DRIVER_E1000_TDESC_CMD_RS		0x00000008	// Report Status
        #define	DRIVER_E1000_TDESC_CMD_IC		0x00000004	// Insert Checksum
        #define	DRIVER_E1000_TDESC_CMD_IFCS		0x00000002	// Insert IFCS
        #define	DRIVER_E1000_TDESC_CMD_EOP		0x00000001	// End Of Packet

        #define	DRIVER_E1000_TIPG_IPGT_DEFAULT		0x0000000A
        #define	DRIVER_E1000_TIPG_IPGT_BIT_0		0x00000001	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_1		0x00000002	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_2		0x00000004	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_3		0x00000008	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_4		0x00000010	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_5		0x00000020	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_6		0x00000040	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_7		0x00000080	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_8		0x00000100	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGT_BIT_9		0x00000200	// IPG Transmit Time
        #define	DRIVER_E1000_TIPG_IPGR1_DEFAULT		0x00002000
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_10		0x00000400	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_11		0x00000800	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_12		0x00001000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_13		0x00002000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_14		0x00004000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_15		0x00008000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_16		0x00010000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_17		0x00020000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_18		0x00040000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR1_BIT_19		0x00080000	// IPG Receive Time 1
        #define	DRIVER_E1000_TIPG_IPGR2_DEFAULT		0x00600000
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_20		0x00100000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_21		0x00200000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_22		0x00400000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_23		0x00800000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_24		0x01000000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_25		0x02000000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_26		0x04000000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_27		0x08000000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_28		0x10000000	// IPG Receive Time 2
        #define	DRIVER_E1000_TIPG_IPGR2_BIT_29		0x20000000	// IPG Receive Time 2

        #define	DRIVER_E1000_IP4AT_ADDR0		0x5840
        #define	DRIVER_E1000_IP4AT_ADDR1		0x5848
        #define	DRIVER_E1000_IP4AT_ADDR2		0x5850
        #define	DRIVER_E1000_IP4AT_ADDR3		0x5858

        #define	DRIVER_E1000_RDLEN_default		0x80	// rozmiar przestrzeni kolejki: 128 Bajtów
        #define DRIVER_E1000_RDLEN_SIZE_byte		DRIVER_E1000_RDLEN_default

        #define	DRIVER_E1000_TDLEN_default		0x80	// rozmiar przestrzeni kolejki: 128 Bajtów
        #define DRIVER_E1000_TDLEN_SIZE_byte		DRIVER_E1000_TDLEN_default

        struct	DRIVER_E1000_STRUCTURE_MMIO {
                uint32_t	ctrl;		// 0x0000 - 0x0003
		uint32_t	unused_0[ 1 ];	// 0x0004 - 0x0007
		uint32_t	status;		// 0x0008 - 0x000B
                uint32_t	unused_1[ 2 ];	// 0x000C - 0x0013
                uint32_t	eerd;		// 0x0014 - 0x0017
                uint32_t	unused_2[ 42 ];	// 0x0018 - 0x00BF
                uint32_t	icr;		// 0x00C0
                uint32_t	unused_3[ 3 ];
                uint32_t	ics;		// 0x00D0
                uint32_t	unused_4[ 1 ];
                uint32_t	imc;		// 0x00D8
                uint32_t	unused_5[ 9 ];
                uint32_t	rctl;		// 0x0100
                uint32_t	unused_6[ 191 ];
                uint32_t	tctl;		// 0x0400
                uint32_t	unused_7[ 3 ];
                uint32_t	tipg;		// 0x0410
                // ------------------------------
                uint32_t	unused_8[ 2299 ];
                uint32_t	rdbal;		// 0x2800
                uint32_t	rdbah;		// 0x2804
                uint32_t	rdlen;		// 0x2808
                uint32_t	unused_9[ 1 ];
                uint32_t	rdh;		// 0x2810
                uint32_t	unused_10[ 1 ];
                uint32_t	rdt;		// 0x2818
                // ------------------------------
                uint32_t	unused_11[ 1017 ];
                uint32_t	tdbal;		// 0x3800
                uint32_t	tdbah;		// 0x3804
                uint32_t	tdlen;		// 0x3808
                uint32_t	unused_12[ 1 ];
                uint32_t	tdh;		// 0x3810
                uint32_t	unused_13[ 1 ];
                uint32_t	tdt;		// 0x3818
        } __attribute__( (packed) );

        struct DRIVER_E1000_STRUCTURE_RDESC {
                uint64_t	address;
                uint16_t	length;
                uint16_t	checksum;
                uint8_t		status;
                uint8_t		errors;
                uint8_t		special;
        } __attribute__( (packed) );

        struct DRIVER_E1000_STRUCTURE_TDESC {
                uint64_t	address;
                uint16_t	length;
		uint8_t		checksum;
		uint8_t		command;
		uint32_t	status_and_reserved;
        } __attribute__( (packed) );

        struct DRIVER_E1000_STRUCTURE_FRAME_ETHERNET {
                uint8_t 	target[ 6 ];
                uint8_t		source[ 6 ];
                uint8_t		type;
        } __attribute__( (packed) );

	// external routines (assembly language)
	extern void driver_e1000_entry( void );
#endif