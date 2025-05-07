#ifndef _IPC_H
#define _IPC_H

#include <kernel.h>

/*
 * Message contents
 */

// Basic data
struct msg_uint8  { uint8_t data[56]; };
struct msg_uint16 { uint16_t data[28]; };
struct msg_uint32 { uint32_t data[14]; };
struct msg_uint64 { uint64_t data[7]; };

/*
 * Message content types
 */
enum {
	MSG_UINT8,
	MSG_UINT16,
	MSG_UINT32,
	MSG_UINT64,
};

/*
 * Message struct
 */
typedef struct {
	pid_t sender;
	uint32_t type; 
	union {
		struct msg_uint8	m_uint8;
		struct msg_uint16	m_uint16;
		struct msg_uint32	m_uint32;
		struct msg_uint64	m_uint64;

		// Makes sure that ipc_msg_t is at least 64 bytes
		uint8_t size[56];	
	};
} ipc_msg_t __align(16);
// Compile time check that ipc_msg_t is 64 bytes
typedef int __ASSERT_ipc_msg_t[(sizeof(ipc_msg_t) == 64) ? 1 : -1];

/*
 * IPC handlers
 */
int ipc_send(pid_t dst, ipc_msg_t* msg);
int ipc_recv(pid_t src, ipc_msg_t* msg);

#endif
