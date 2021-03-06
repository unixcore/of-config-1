/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cxmpclient_wrapper.h"
#include "cxmp_blocking_client_adapter.h"
#include <xdpd/xmp/client/cxmpclient.h>

#include <list>
#include <assert.h>
#include <libnetconf/netconf.h>

static cxmp_blocking_client_adapter *xmp_client = NULL;

void*
new_xmp_client()
{
	nc_verb_verbose(__FUNCTION__);
	if (NULL != xmp_client) return xmp_client;

	rofl::logging::init();
	unsigned int dbg_level;
	switch (/*verbose_level*/NC_VERB_ERROR) { // verbose_level is currently a local symbol...
		case NC_VERB_WARNING:
			dbg_level = rofl::logging::WARN;
			break;
		case NC_VERB_VERBOSE:
			dbg_level = rofl::logging::INFO;
			break;
		case NC_VERB_DEBUG:
			dbg_level = rofl::logging::DBG;
			break;
		case NC_VERB_ERROR:
		default:
			dbg_level = rofl::logging::ERROR;
			break;
	}
	rofl::logging::set_debug_level(dbg_level);

	xmp_client = new cxmp_blocking_client_adapter();

	assert(NULL != xmp_client);

	return reinterpret_cast<void*>(xmp_client);
}

void
delete_xmp_client(void* data)
{
	nc_verb_verbose(__FUNCTION__);
	assert(NULL != data);
	assert(xmp_client == data);
	delete reinterpret_cast<cxmp_blocking_client_adapter*>(xmp_client);
	xmp_client = NULL;
}

void
get_resources(void* handle, xmlNodePtr resources)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(resources);
	assert(handle == xmp_client);

	xmp_client->get_resources(resources);
}

void
get_port_info(void* handle, xmlNodePtr resources, xmlDocPtr running)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(resources);
	assert(handle == xmp_client);

	xmp_client->add_port_info(resources, running);

}

void
get_lsi_info(void* handle, xmlNodePtr lsis, xmlDocPtr running)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(lsis);
	assert(handle == xmp_client);

	xmp_client->add_lsi_info(lsis, running);
}

void
get_lsi_config(void* handle, xmlNodePtr lsis)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(lsis);
	assert(handle == xmp_client);

	xmp_client->get_lsi_config(lsis);
}

static inline void
convert_controller_list(struct list* in, std::list<class xdpd::mgmt::protocol::controller> *_out)
{
	assert(in);

	struct controller *c;
	while ((c = (struct controller *)list_pop_head(in))) {

		xdpd::mgmt::protocol::controller c_tmp;

		if (c->id) {
			c_tmp.id.assign(c->id);
			free(c->id);
		}

		if (c->proto) {
			c_tmp.proto.assign(c->proto);
			free(c->proto);
		}

		c_tmp.ip_domain = c->ip_domain;
		if (AF_INET == c->ip_domain) {
			struct sockaddr_in sin;
			sin.sin_addr = *((struct in_addr*)c->ip);
			c_tmp.address = rofl::caddress_in4(&sin, sizeof(struct sockaddr_in));
		} else if (AF_INET6 == c->ip_domain) {
			struct sockaddr_in6 sin;
			memcpy(&sin.sin6_addr, (struct in6_addr*)c->ip, 16);
			c_tmp.address = rofl::caddress_in6(&sin, sizeof(struct sockaddr_in6));
		} else {
			assert(0);
		}
		free(c->ip);

		c_tmp.port = c->port;

		free(c); // todo maybe this should be done elsewhere?!

		_out->push_back(c_tmp);
	}
}

int
lsi_create(void* handle, struct lsi* lsi)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(lsi);
	assert(handle == xmp_client);

	std::list<class xdpd::mgmt::protocol::controller> cont;
	convert_controller_list((struct list*)lsi->controller_list_add, &cont);

	assert(cont.size());

	return xmp_client->lsi_create(lsi->dpid, std::string(lsi->dpname), cont);
}

int
lsi_destroy(void* handle, const uint64_t dpid)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(handle == xmp_client);

	return xmp_client->lsi_destroy(dpid);
}

int
lsi_connect_to_controller(void* handle, struct lsi *lsi)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(lsi);
	assert(handle == xmp_client);

	std::list<struct xdpd::mgmt::protocol::controller> controller;
	convert_controller_list((struct list*)lsi->controller_list_add, &controller);

	return xmp_client->lsi_connect_to_controller(lsi->dpid, controller);
}

int
lsi_cross_connect(void* handle, const uint64_t dpid1, const uint64_t port_no1, const uint64_t dpid2, const uint64_t port_no2)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(dpid1 != dpid2);
	assert(handle == xmp_client);

	return xmp_client->lsi_cross_connect(dpid1, port_no1, dpid2, port_no2);
}

int
port_attach(void* handle, uint64_t dpid, char* port_name)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(port_name);
	assert(handle == xmp_client);

	return xmp_client->port_attach(dpid, port_name);
}

int
port_detach(void* handle, uint64_t dpid, char* port_name)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(port_name);
	assert(handle == xmp_client);

	return xmp_client->port_detach(dpid, port_name);
}

int
port_enable(void* handle, const char* port_name)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(port_name);
	assert(handle == xmp_client);

	return xmp_client->port_enable(port_name);
}

int
port_disable(void* handle, const char* port_name)
{
	nc_verb_verbose(__PRETTY_FUNCTION__);

	assert(handle);
	assert(port_name);
	assert(handle == xmp_client);

	return xmp_client->port_disable(port_name);
}
