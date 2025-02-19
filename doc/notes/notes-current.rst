.. Copyright (C) Internet Systems Consortium, Inc. ("ISC")
..
.. SPDX-License-Identifier: MPL-2.0
..
.. This Source Code Form is subject to the terms of the Mozilla Public
.. License, v. 2.0.  If a copy of the MPL was not distributed with this
.. file, you can obtain one at https://mozilla.org/MPL/2.0/.
..
.. See the COPYRIGHT file distributed with this work for additional
.. information regarding copyright ownership.

Notes for BIND 9.19.6
---------------------

Security Fixes
~~~~~~~~~~~~~~

- None.

Known Issues
~~~~~~~~~~~~

- None.

New Features
~~~~~~~~~~~~

- None.

- A new configuration option ``require-cookie`` has been introduced, it
  specifies if there should be a DNS COOKIE in the response for a given
  prefix and if not named falls back to TCP.  This is useful if you know
  a given server support DNS COOKIE.  It can also be used to force all
  non DNS COOKIE responses to fall back to TCP.  :gl:`#2295`

Removed Features
~~~~~~~~~~~~~~~~

- None.

Feature Changes
~~~~~~~~~~~~~~~

- When an international domain name is not valid according to IDNA2008,
  :program:`dig` will now try to convert it according to IDNA2003 rules,
  or pass it through unchanged, instead of stopping with an error message.
  You can use the ``idna2`` utility for checking IDNA syntax. :gl:`#3485`.

Bug Fixes
~~~~~~~~~

- None.
