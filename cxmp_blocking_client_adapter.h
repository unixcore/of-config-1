/*
 * cxmp_blocking_client_adapter.h
 *
 *  Created on: Jul 22, 2014
 *      Author: tobi
 */

#ifndef CXMP_BLOCKING_CLIENT_ADAPTER_H_
#define CXMP_BLOCKING_CLIENT_ADAPTER_H_

#include <pthread.h>
#include <libxml/tree.h>

#include <xdpd/xmp/client/cxmpclient.h>

class cxmp_blocking_client_adapter : public xdpd::mgmt::protocol::cxmpobserver {
public:
	cxmp_blocking_client_adapter();
	virtual ~cxmp_blocking_client_adapter();

	virtual void
	notify(const xdpd::mgmt::protocol::cxmpmsg &msg);

	void
	add_port_info(xmlNodePtr resources);

	xdpd::mgmt::protocol::cxmpclient *xmp_client;
private:
	pthread_mutex_t client_lock;
	pthread_t worker;
	pthread_cond_t client_read_cv;
	xdpd::mgmt::protocol::cxmpmsg *msg;

};

#endif /* CXMP_BLOCKING_CLIENT_ADAPTER_H_ */