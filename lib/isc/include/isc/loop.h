/*
 * Copyright (C) Internet Systems Consortium, Inc. ("ISC")
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * See the COPYRIGHT file distributed with this work for additional
 * information regarding copyright ownership.
 */

#pragma once

#include <inttypes.h>

#include <isc/lang.h>
#include <isc/mem.h>
#include <isc/refcount.h>
#include <isc/types.h>

typedef void (*isc_job_cb)(void *);

ISC_LANG_BEGINDECLS

void
isc_loopmgr_create(isc_mem_t *mctx, uint32_t nloops, isc_loopmgr_t **loopmgrp);
/*%<
 * Create a loop manager supporting 'nloops' loops.
 *
 * Requires:
 *\li	'nloops' is greater than 0.
 */

void
isc_loopmgr_destroy(isc_loopmgr_t **loopmgrp);
/*%<
 * Destroy the loop manager pointed to by 'loopmgrp'.
 *
 * Requires:
 *\li	'loopmgr' points to a valid loop manager.
 */

void
isc_loopmgr_shutdown(isc_loopmgr_t *loopmgr);
/*%<
 * Request shutdown of the loop manager 'loopmgr'.
 *
 * This will stop all signal handlers and send shutdown events to
 * all active loops. As a final action on shutting down, each loop
 * will run the function (or functions) set by isc_loopmgr_teardown()
 * or isc_loop_teardown().
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 */

void
isc_loopmgr_run(isc_loopmgr_t *loopmgr);
/*%<
 * Run the loops in 'loopmgr'. Each loop will start by running the
 * function (or functions) set by isc_loopmgr_setup() or isc_loop_setup().
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 */

void
isc_loopmgr_pause(isc_loopmgr_t *loopmgr);
/*%<
 * Send pause events to all running loops in 'loopmgr' except the
 * current one. This can only be called from a running loop.
 * All the paused loops will wait until isc_loopmgr_resume() is
 * run in the calling loop before continuing.
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 *\li	We are in a running loop.
 */

void
isc_loopmgr_resume(isc_loopmgr_t *loopmgr);
/*%<
 * Send resume events to all paused loops in 'loopmgr'. This can
 * only be called by a running loop (which must therefore be the
 * loop that called isc_loopmgr_pause()).
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 *\li	We are in a running loop.
 */

uint32_t
isc_loopmgr_nloops(isc_loopmgr_t *loopmgr);

isc_job_t *
isc_loop_setup(isc_loop_t *loop, isc_job_cb cb, void *cbarg);
isc_job_t *
isc_loop_teardown(isc_loop_t *loop, isc_job_cb cb, void *cbarg);
/*%<
 * Schedule actions to be run when starting, and when shutting down,
 * one of the loops in a loop manager.
 *
 * Requires:
 *\li	'loop' is a valid loop.
 *\li	The loop manager associated with 'loop' is paused or has not
 *	yet been started.
 */

void
isc_loop_nosetup(isc_loop_t *loop, isc_job_t *job);
void
isc_loop_noteardown(isc_loop_t *loop, isc_job_t *job);

void
isc_loopmgr_setup(isc_loopmgr_t *loopmgr, isc_job_cb cb, void *cbarg);
void
isc_loopmgr_teardown(isc_loopmgr_t *loopmgr, isc_job_cb cb, void *cbarg);
/*%<
 * Schedule actions to be run when starting, and when shutting down,
 * *all* of the loops in loopmgr.
 *
 * This is the same as running isc_loop_setup() or
 * isc_loop_teardown() on each of the loops in turn.
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 *\li	'loopmgr' is paused or has not yet been started.
 */

isc_mem_t *
isc_loop_getmctx(isc_loop_t *loop);
/*%<
 * Return a pointer to the a memory context that was created for
 * 'loop' when it was initialized.
 *
 * Requires:
 *\li	'loop' is a valid loop.
 */

isc_loop_t *
isc_loop_main(isc_loopmgr_t *loopmgr);
/*%<
 * Returns the main loop for the 'loopmgr' (which is 'loopmgr->loops[0]',
 * regardless of how many loops there are).
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 */

isc_loop_t *
isc_loop_current(isc_loopmgr_t *loopmgr);
/*%<
 * Returns the loop object from which the function has been called,
 * or NULL if not called from a loop.
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 */

isc_loop_t *
isc_loop_get(isc_loopmgr_t *loopmgr, uint32_t tid);
/*%<
 * Return the loop object associated with the 'tid' threadid
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 *\li   'tid' is smaller than number of initialized loops
 */

void
isc_loopmgr_blocking(isc_loopmgr_t *loopmgr);
void
isc_loopmgr_nonblocking(isc_loopmgr_t *loopmgr);
/*%
 * isc_loopmgr_blocking() stops the SIGINT and SIGTERM signal handlers
 * during blocking operations, for example while waiting for user
 * interaction; isc_loopmgr_nonblocking() restarts them.
 *
 * Requires:
 *\li	'loopmgr' is a valid loop manager.
 */
ISC_LANG_ENDDECLS
