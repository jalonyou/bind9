/*
 * Copyright (C) 1999  Internet Software Consortium.
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

#ifndef DNS_ADB_H
#define DNS_ADB_H

/***** 
 ***** Module Info 
 *****/ 


/*
 * DNS Address Database
 *
 * This module implements an address database (ADB) for mapping an
 * NS rdata record to an isc_sockaddr_t. It also provides statistical
 * information on how good that address might be.
 *
 * A client will pass in a dns_name_t, and the ADB will walk through
 * the rdataset looking up addresses associated with the name.  If it
 * is found on the internal lists, a structure is filled in with the
 * address information and stats for found addresses.
 *
 * If the name cannot be found on the internal lists, a new entry will
 * be created for an name if all the information needed can be found
 * in the zone table or cache.  This new address will then be returned.
 *
 * If a request must be made to remote servers to satisfy a name lookup,
 * this module will start fetches to try to complete these addresses.  When
 * at least one more completes, an event is sent to the caller.  If none of
 * them resolve before the fetch times out, an event indicating this is
 * sent instead.
 *
 * Records are stored internally until a timer expires. The timer is the
 * smaller of the TTL or signature validity period. For A6 records, the timer
 * is the smallest of all the TTL or signature validity periods in the A6
 * chain.
 *
 * Lameness is stored per-zone, and this data hangs off each address field.
 * When an address is marked lame for a given zone the address will not
 * be returned to a caller.
 *
 *
 * MP:
 *
 *	The ADB takes care of all necessary locking. 
 *
 *	Only the task which initiated the name lookup can cancel the lookup.
 *
 *
 * Security:
 *
 *	None, since all data stored is required to be pre-filtered.
 *	(Cache needs to be sane, fetches return bounds-checked and sanity-
 *	checked data, caller passes a good dns_name_t for the zone, etc)
 */

/***
 *** IMPORTS
 ***/

#include <stdio.h>

#include <isc/event.h>
#include <isc/lang.h>
#include <isc/list.h>
#include <isc/magic.h>
#include <isc/mem.h>
#include <isc/result.h>
#include <isc/sockaddr.h>
#include <isc/stdtime.h>
#include <isc/task.h>

#include <dns/types.h>
#include <dns/rdataset.h>
#include <dns/view.h>

ISC_LANG_BEGINDECLS

/***
 *** Magic number checks
 ***/

#define DNS_ADBFIND_MAGIC	  0x61646248	/* adbH. */
#define DNS_ADBFIND_VALID(x)	  ISC_MAGIC_VALID(x, DNS_ADBFIND_MAGIC)
#define DNS_ADBADDRINFO_MAGIC	  0x61644149	/* adAI. */
#define DNS_ADBADDRINFO_VALID(x)  ISC_MAGIC_VALID(x, DNS_ADBADDRINFO_MAGIC)


/***
 *** TYPES
 ***/

typedef struct dns_adbname		dns_adbname_t;

/* dns_adbfind_t
 *
 * The find into our internal state of what is going on, where, when...
 * This is returned to the user as a find, so requests can be canceled,
 * etc.
 *
 * On return, the client can safely use "list", and can reorder the list.
 * Items may not be _deleted_ from this list, however, or added to it
 * other than by using the dns_adb_*() API.
 */
struct dns_adbfind {
	/* Public */
	unsigned int			magic;		/* RO: magic */
	dns_adbaddrinfolist_t		list;		/* RO: list of addrs */
	unsigned int			query_pending;	/* RO: partial list */
	unsigned int			partial_result;	/* RO: addrs missing */
	unsigned int			options;	/* RO: options */
	ISC_LINK(dns_adbfind_t)		publink;	/* RW: client use */

	/* Private */
	isc_mutex_t			lock;		/* locks all below */
	int				name_bucket;
	unsigned int			flags;
	dns_adbname_t		       *adbname;
	dns_adb_t		       *adb;
	isc_event_t			event;
	ISC_LINK(dns_adbfind_t)	plink;
};

#define DNS_ADBFIND_INET		0x00000001
#define DNS_ADBFIND_INET6		0x00000002
#define DNS_ADBFIND_ADDRESSMASK		0x00000003

#define DNS_ADBFIND_EMPTYEVENT		0x00000004
#define DNS_ADBFIND_WANTEVENT		0x00000008

/* dns_adbaddr_t
 *
 * The answers to queries come back as a list of these.
 */
struct dns_adbaddrinfo {
	unsigned int			magic;		/* private */

	isc_sockaddr_t		       *sockaddr;	/* read only */
	int				goodness;
	unsigned int			srtt;		/* microseconds */
	unsigned int			flags;
	dns_adbentry_t		       *entry;		/* private */
	ISC_LINK(dns_adbaddrinfo_t)	publink;
};

/*
 * The event sent to the caller task is just a plain old isc_event_t.  It
 * contains no data other than a simple status, passed in the "type" field
 * to indicate that another address resolved, or all partially resolved
 * addresses have failed to resolve.
 *
 * "sender" is the dns_adbfind_t used to issue this query.
 *
 * This is simply a standard event, with the "type" set to:
 *
 *	DNS_EVENT_ADBMOREADDRESSES   -- another address resolved.
 *	DNS_EVENT_ADBNOMOREADDRESSES -- all pending addresses failed,
 *					were canceled, or otherwise will
 *					not be usable.
 *	DNS_EVENT_ADBCANCELED	     -- The request was canceled by a
 *					3rd party.
 *	DNS_EVENT_ADBNAMEDELETED     -- The name was deleted, so this request
 *					was canceled.
 *
 * In each of these cases, the addresses returned by the initial call
 * to dns_adb_createfind() can still be used until they are no longer needed.
 */

/****
 **** FUNCTIONS
 ****/


isc_result_t
dns_adb_create(isc_mem_t *mem, dns_view_t *view, isc_timermgr_t *tmgr,
	       isc_taskmgr_t *taskmgr, dns_adb_t **newadb);
/*
 * Create a new ADB.
 *
 * Requires:
 *
 *	'mem' must be a pointer to a valid memory manager that all internal
 *	allocations will happen through (and so must remain valid at least
 *	until the new isc_addrtable_t is deleted)
 *
 *	'view' be a pointer to a valid view.
 *
 *	'tmgr' be a pointer to a valid timer manager.
 *
 *	'newadb' != NULL && '*newadb' == NULL.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS	after happiness
 *	ISC_R_NOMEMORY	after resource allocation failure.
 */


void
dns_adb_detach(dns_adb_t **adb);
/*
 * Delete the ADB. Sets *ADB to NULL. Cancels any outstanding requests.
 *
 * Requires:
 *
 *	'adb' be non-NULL and '*adb' be a valid dns_adb_t, created via
 *	dns_adb_create().
 */


isc_result_t
dns_adb_createfind(dns_adb_t *adb, isc_task_t *task, isc_taskaction_t action,
		   void *arg, dns_name_t *name, dns_name_t *zone,
		   unsigned int families, isc_stdtime_t now,
		   dns_adbfind_t **find);
/*
 * Main interface for clients. The adb will look up the name given in
 * "name" and will build up a list of found addresses, and perhaps start
 * internal fetches to resolve names that are unknown currently.
 *
 * If other addresses resolve after this call completes, an event will
 * be sent to the <task, taskaction, arg> with the sender of that event
 * set to a pointer to the dns_adbfind_t returned by this function.
 *
 * The list of addresses returned is unordered.  The caller must impose
 * any ordering required.  The list will not contain "known bad" addresses,
 * however.  For instance, it will not return hosts that are known to be
 * lame for the zone in question.
 *
 * The caller cannot (directly) modify the contents of the address list's
 * fields other than the "link" field.  All values can be read at any
 * time, however.
 *
 * The "now" parameter is used only for determining which entries that
 * have a specific time to live or expire time should be removed from
 * the running database.  If specified as zero, the current time will
 * be retrieved and used.
 *
 * Requires:
 *
 *	*adb be a valid isc_adb_t object.
 *
 *	If events are to be sent, *task be a valid task,
 *	and isc_taskaction_t != NULL.
 *
 *	*name is a valid dns_name_t.
 *
 *	zone != NULL and *zone be a valid dns_name_t.
 *
 *	find != NULL && *find == NULL.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS	Addresses might have been returned, and events will be
 *			delivered for unresolved addresses.
 *	ISC_R_NOMORE	Addresses might have been returned, but no events
 *			will ever be posted for this context.  This is only
 *			returned if task != NULL.
 *	ISC_R_NOMEMORY	insufficient resources
 *
 * Calls, and returns error codes from:
 *
 *	isc_stdtime_get()
 *
 * Notes:
 *
 *	No internal reference to "name" exists after this function
 *	returns.
 */


isc_result_t
dns_adb_deletename(dns_adb_t *adb, dns_name_t *host);
/*
 * Deletes the name and drops reference counts on all subordinate
 * addresses.
 *
 * Requires:
 *
 *	'adb' must be valid.
 *
 *	'host' contains the name of the host to be deleted.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS	-- it's gone.
 *	ISC_R_NOTFOUND	-- the host is not in the database
 */


isc_result_t
dns_adb_insert(dns_adb_t *adb, dns_name_t *host, isc_sockaddr_t *addr,
	       dns_ttl_t ttl, isc_stdtime_t now);
/*
 * Insert a host name and address into the database.  A new (blank, no
 * badness) record is inserted.
 *
 * This function should be used with caution, since it may not exist
 * for more than testing purposes.
 *
 * Requires:
 *
 *	'adb' be valid.
 *
 *	'host' contain the name of the host to be inserted.
 *
 *	'addr' point to the address of the host to insert.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS	-- all is well.
 *	ISC_R_NOMEMORY	-- no memory
 *	ISC_R_EXISTS	-- the <host, address> tuple exists already.
 */

void
dns_adb_cancelfind(dns_adbfind_t *find);
/*
 * Cancels the find, and sends the event off to the caller.
 *
 * It is an error to call dns_adb_cancelfind() on a find where
 * no event is wanted, or will ever be sent.
 *
 * Requires:
 *
 *	'find' be a valid dns_adbfind_t pointer.
 *
 *	events would have been posted to the task.  This can be checked
 *	with (find->options & DNS_ADBFIND_WANTEVENT).
 *
 * Ensures:
 *
 *	The event was posted to the task.
 *
 * Note:
 *
 *	It is possible that the real completion event was posted just
 *	before the dns_adb_cancelfind() call was made.  In this case,
 *	dns_adb_cancelfind() will do nothing.  The event findr needs
 *	to be prepared to find this situation.
 */

void
dns_adb_destroyfind(dns_adbfind_t **find);
/*
 * Destroys the find reference.
 *
 * Requires:
 *
 *	'find' != NULL and *find be valid dns_adbfind_t pointer.
 *
 * Ensures:
 *
 *	No "address found" events will be posted to the originating task
 *	after this function returns.
 *
 * Note:
 *
 *	This can only be called after the event was delivered for a
 *	find.  Additionally, the event MUST have been freed via
 *	isc_event_free() BEFORE this function is called.
 */

void
dns_adb_dump(dns_adb_t *adb, FILE *f);
/*
 * This function is used only for debugging.  It will dump as much of the
 * state of the running system as possible.
 *
 * Requires:
 *
 *	adb be valid.
 *
 *	f != NULL, and be a file open for writing.
 */

void
dns_adb_dumpfind(dns_adbfind_t *find, FILE *f);
/*
 * Dump the data associated with a find.
 *
 * Requires:
 *
 *	adb be valid.
 *
 *	adbfind be valid.
 *
 *	f != NULL, and be a file open for writing.
 */

isc_result_t
dns_adb_marklame(dns_adb_t *adb, dns_adbaddrinfo_t *addr, dns_name_t *zone,
		 isc_stdtime_t expire_time);
/*
 * Mark the given address as lame for the zone "zone".  expire_time should
 * be set to the time when the entry should expire.  That is, if it is to
 * expire 10 minutes in the future, it should set it to (now + 10 * 60).
 *
 * Requires:
 *
 *	adb be valid.
 *
 *	addr be valid.
 *
 *	zone be the zone used in the dns_adb_createfind() call.
 *
 * Returns:
 *
 *	ISC_R_SUCCESS		-- all is well.
 *	ISC_R_NOMEMORY		-- could not mark address as lame.
 */

void
dns_adb_adjustgoodness(dns_adb_t *adb, dns_adbaddrinfo_t *addr,
		       int goodness_adjustment);
/*
 * Increase or decrease the address's goodness value.
 *
 * Requires:
 *
 *	adb be valid.
 *
 *	addr be valid.
 *
 * Note:
 *
 *	Goodness values are silently clamped to INT_MAX and INT_MIN.
 *
 *	The goodness in addr will be updated to reflect the new global
 *	goodness value.  This may include changes made by others.
 */

void
dns_adb_adjustsrtt(dns_adb_t *adb, dns_adbaddrinfo_t *addr,
		   unsigned int rtt, unsigned int factor);
/*
 * Mix the round trip time into the existing smoothed rtt.  The formula used
 * (where srtt is the existing rtt value, and rtt and factor are arguments to
 * this function):
 *
 *	new_srtt = (srtt * (factor - 1) + rtt) / factor
 *
 * Requires:
 *
 *	adb be valid.
 *
 *	addr be valid.
 *
 * Note:
 *
 *	If factor is zero, 4 will be used.
 *
 *	The srtt in addr will be updated to reflect the new global
 *	srtt value.  This may include changes made by others.
 */


/*
 * XXX Need functions/macros to:
 *
 *	Remove an address from a find's linked list.  This is needed
 *	because the data pointed to by a dns_adbaddr_t is reference counted.
 *
 *	set/clear various flags.  (Which flags?)
 */

ISC_LANG_ENDDECLS

#endif /* DNS_ADB_H */
