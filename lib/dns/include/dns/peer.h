/*
 * Copyright (C) 1999, 2000  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef DNS_PEER_H
#define DNS_PEER_H 1

/*****
 ***** Module Info
 *****/

/*
 * Data structures for peers (e.g. a 'server' config file statement)
 */

/***
 *** Imports
 ***/

#include <config.h>

#include <sys/types.h>

#include <isc/mem.h>
#include <isc/net.h>
#include <isc/netaddr.h>

#include <dns/types.h>
#include <dns/confcommon.h>
#include <dns/confkeys.h>


#define DNS_PEERLIST_MAGIC	0x7365524c /* seRL */
#define DNS_PEER_MAGIC		0x53457276 /* SErv */

#define DNS_PEERLIST_VALID(ptr)	ISC_MAGIC_VALID(ptr, DNS_PEERLIST_MAGIC)
#define DNS_PEER_VALID(ptr)	ISC_MAGIC_VALID(ptr, DNS_PEER_MAGIC)


/***
 *** Types
 ***/

struct dns_peerlist
{
	isc_uint32_t		magic;
	isc_uint32_t		refs;
	
	isc_mem_t	       *mem;

	ISC_LIST(dns_peer_t) elements;
};


struct dns_peer 
{
	isc_uint32_t		magic;
	isc_uint32_t		refs;

	isc_mem_t	       *mem;

	isc_netaddr_t		address;
	isc_boolean_t		bogus;
	dns_transfer_format_t	transfer_format;
	int			transfers;
	isc_boolean_t		support_ixfr;
	dns_name_t	       *key;

	isc_uint32_t		bitflags;
	
	ISC_LINK(dns_peer_t)	next;
};


/***
 *** Functions
 ***/

isc_result_t	dns_peerlist_new(isc_mem_t *mem,
				 dns_peerlist_t **list);
void		dns_peerlist_attach(dns_peerlist_t *source,
				    dns_peerlist_t **target);
void		dns_peerlist_detach(dns_peerlist_t **list);

/* After return caller still holds a reference to peer. */
void		dns_peerlist_addpeer(dns_peerlist_t *peers,
				     dns_peer_t *peer);

/* ditto */
isc_result_t	dns_peerlist_peerbyaddr(dns_peerlist_t *peers,
					isc_netaddr_t *addr,
					dns_peer_t **retval);

/* what he said. */
isc_result_t	dns_peerlist_currpeer(dns_peerlist_t *peers,
				      dns_peer_t **retval);



isc_result_t	dns_peer_new(isc_mem_t *mem,
			     isc_netaddr_t *ipaddr,
			     dns_peer_t **peer);
isc_result_t	dns_peer_attach(dns_peer_t *source, dns_peer_t **target);
isc_result_t	dns_peer_detach(dns_peer_t **list);

isc_result_t	dns_peer_setbogus(dns_peer_t *peer,
				  isc_boolean_t newval);
isc_result_t	dns_peer_getbogus(dns_peer_t *peer,
				  isc_boolean_t *retval);
isc_result_t	dns_peer_setsupportixfr(dns_peer_t *peer,
					isc_boolean_t newval);
isc_result_t	dns_peer_getsupportixfr(dns_peer_t *peer,
					isc_boolean_t *retval);
isc_result_t	dns_peer_settransfers(dns_peer_t *peer,
				      isc_int32_t newval);
isc_result_t	dns_peer_gettransfers(dns_peer_t *peer,
				      isc_int32_t *retval);
isc_result_t	dns_peer_settransferformat(dns_peer_t *peer,
					   dns_transfer_format_t newval);
isc_result_t	dns_peer_gettransferformat(dns_peer_t *peer,
					   dns_transfer_format_t *retval);
isc_result_t	dns_peer_setkeybycharp(dns_peer_t *peer,
				       const char *keyval);
isc_result_t	dns_peer_getkey(dns_peer_t *peer, dns_name_t **retval);
isc_result_t	dns_peer_setkey(dns_peer_t *peer, dns_name_t **keyval);


#endif /* DNS_CONFIG_CONFSERV_H */
