#!/bin/sh
#
# Copyright (C) 2004, 2007, 2011-2013, 2016  Internet Systems Consortium, Inc. ("ISC")
# Copyright (C) 2000, 2001  Internet Software Consortium.
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

SYSTEMTESTTOP=..
. $SYSTEMTESTTOP/conf.sh

status=0

DIGOPTS="-p ${PORT}"

echo_i "querying for various representations of an IN A record"
for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
	ret=0
	$DIG +short $DIGOPTS @10.53.0.1 a$i.example a in > dig.out || ret=1
	echo 10.0.0.1 | $DIFF - dig.out || ret=1
	if [ $ret != 0 ]
	then
		echo "#$i failed"
	fi
	status=`expr $status + $ret`
done

echo_i "querying for various representations of an IN TXT record"
for i in 1 2 3 4 5 6 7
do
	ret=0
	$DIG +short $DIGOPTS @10.53.0.1 txt$i.example txt in > dig.out || ret=1
	echo '"hello"' | $DIFF - dig.out || ret=1
	if [ $ret != 0 ]
	then
		echo "#$i failed"
	fi
	status=`expr $status + $ret`
done

echo_i "querying for various representations of an IN TYPE123 record"
for i in 1 2 3
do
	ret=0
	$DIG +short $DIGOPTS @10.53.0.1 unk$i.example type123 in > dig.out || ret=1
	echo '\# 1 00' | $DIFF - dig.out || ret=1
	if [ $ret != 0 ]
	then
		echo "#$i failed"
	fi
	status=`expr $status + $ret`
done

echo_i "querying for NULL record"
ret=0
$DIG +short $DIGOPTS @10.53.0.1 null.example null in > dig.out || ret=1
echo '\# 1 00' | $DIFF - dig.out || ret=1
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "querying for empty NULL record"
ret=0
$DIG +short $DIGOPTS @10.53.0.1 empty.example null in > dig.out || ret=1
echo '\# 0' | $DIFF - dig.out || ret=1
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "querying for various representations of a CLASS10 TYPE1 record"
for i in 1 2
do
	ret=0
	$DIG +short $DIGOPTS @10.53.0.1 a$i.example a class10 > dig.out || ret=1
	echo '\# 4 0A000001' | $DIFF - dig.out || ret=1
	if [ $ret != 0 ]
	then
		echo "#$i failed"
	fi
	status=`expr $status + $ret`
done

echo_i "querying for various representations of a CLASS10 TXT record"
for i in 1 2 3 4
do
	ret=0
	$DIG +short $DIGOPTS @10.53.0.1 txt$i.example txt class10 > dig.out || ret=1
	echo '"hello"' | $DIFF - dig.out || ret=1
	if [ $ret != 0 ]
	then
		echo "#$i failed"
	fi
	status=`expr $status + $ret`
done

echo_i "querying for various representations of a CLASS10 TYPE123 record"
for i in 1 2
do
	ret=0
	$DIG +short $DIGOPTS @10.53.0.1 unk$i.example type123 class10 > dig.out || ret=1
	echo '\# 1 00' | $DIFF - dig.out || ret=1
	if [ $ret != 0 ]
	then
		echo "#$i failed"
	fi
	status=`expr $status + $ret`
done

echo_i "querying for SOAs of zone that should have failed to load"
for i in 1 2 3 4
do
	ret=0
	$DIG $DIGOPTS @10.53.0.1 broken$i. soa in > dig.out || ret=1
	grep "SERVFAIL" dig.out > /dev/null || ret=1
	if [ $ret != 0 ]
	then
		echo "#$i failed"
	fi
	status=`expr $status + $ret`
done

echo_i "checking large unknown record loading on master"
ret=0
$DIG $DIGOPTS @10.53.0.1 +tcp +short large.example TYPE45234 > dig.out || { ret=1 ; echo_i "dig failed" ; }
$DIFF -s large.out dig.out > /dev/null || { ret=1 ; echo_i "$DIFF failed"; }
[ $ret = 0 ] || echo_i "failed"

echo_i "checking large unknown record loading on slave"
ret=0
$DIG $DIGOPTS @10.53.0.2 +tcp +short large.example TYPE45234 > dig.out || { ret=1 ; echo_i "dig failed ; }
$DIFF -s large.out dig.out > /dev/null || { ret=1 ; echo_i "diff failed"; }
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "stop and restart slave"
$PERL $SYSTEMTESTTOP/stop.pl . ns2
$PERL $SYSTEMTESTTOP/start.pl --noclean --restart --port ${PORT} . ns2

echo_i "checking large unknown record loading on slave"
ret=0
$DIG $DIGOPTS @10.53.0.2 +tcp +short large.example TYPE45234 > dig.out || { ret=1 ; echo_i "dig failed ; }
$DIFF -s large.out dig.out > /dev/null || { ret=1 ; echo_i "diff failed"; }
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "checking large unknown record loading on inline slave"
ret=0
$DIG $DIGOPTS @10.53.0.3 +tcp +short large.example TYPE45234 > dig.out || { ret=1 ; echo_i "dig failed ; }
$DIFF large.out dig.out > /dev/null || { ret=1 ; echo_i "diff failed"; }
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "stop and restart inline slave"
$PERL $SYSTEMTESTTOP/stop.pl . ns3
$PERL $SYSTEMTESTTOP/start.pl --noclean --restart --port ${PORT} . ns3

echo_i "checking large unknown record loading on inline slave"
ret=0
$DIG $DIGOPTS @10.53.0.3 +tcp +short large.example TYPE45234 > dig.out || { ret=1 ; echo_i "dig failed ; }
$DIFF large.out dig.out > /dev/null || { ret=1 ; echo_i "diff failed"; }
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "check that '"'"\\#"'"' is not treated as the unknown escape sequence"
ret=0
$DIG $DIGOPTS @10.53.0.1 +tcp +short txt8.example txt > dig.out
echo '"#" "2" "0145"' | $DIFF - dig.out || ret=1
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "check that 'TXT \# text' is not treated as the unknown escape sequence"
ret=0
$DIG $DIGOPTS @10.53.0.1 +tcp +short txt9.example txt > dig.out
echo '"#" "text"' | $DIFF - dig.out || ret=1
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "check that 'TYPE353 \# cat' produces 'not a valid number'"
ret=0
$CHECKZONE nan.bad zones/nan.bad > check.out 2>&1
grep "not a valid number" check.out > /dev/null || ret=1
[ $ret = 0 ] || echo_i "failed"
status=`expr $status + $ret`

echo_i "exit status: $status"
[ $status -eq 0 ] || exit 1
