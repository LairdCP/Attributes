#ifndef __DUMMY_SMP_H__
#define __DUMMY_SMP_H__

#include <zephyr.h>
#include <net/buf.h>
#include <mgmt/mgmt.h>
#include <mgmt/mcumgr/serial.h>
#include <mgmt/mcumgr/buf.h>

struct net_buf *smp_dummy_process_frag_outgoing(uint8_t *buffer, uint8_t buffer_size);

int mcumgr_serial_tx_pkt(const uint8_t *data, int len, mcumgr_serial_tx_cb cb,
                         void *arg);

#endif /* __DUMMY_SMP_H__ */
