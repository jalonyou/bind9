/*
 * Copyright (C) 1998, 1999, 2000  Internet Software Consortium.
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

#include <config.h>

#include <stdlib.h>

#include <isc/buffer.h>
#include <isc/commandline.h>
#include <isc/lex.h>
#include <isc/mem.h>
#include <isc/string.h>
#include <isc/util.h>

#include <dns/rdata.h>
#include <dns/compress.h>
#include <dns/rdataclass.h>
#include <dns/rdatastruct.h>
#include <dns/rdatatype.h>
#include <dns/result.h>

isc_mem_t *mctx;
isc_lex_t *lex;

isc_lexspecials_t specials;

static void
viastruct(dns_rdata_t *rdata) {
	isc_result_t result;

	switch (rdata->type) {
	case dns_rdatatype_a6: {
		dns_rdata_in_a6_t in_a6;
		result = dns_rdata_tostruct(rdata, &in_a6, NULL);
		break;
	}
	case dns_rdatatype_a: {
		switch (rdata->rdclass) {
		case dns_rdataclass_hs: {
			dns_rdata_hs_a_t hs_a;
			result = dns_rdata_tostruct(rdata, &hs_a, NULL);
			break;
		}
		case dns_rdataclass_in: {
			dns_rdata_in_a_t in_a;
			result = dns_rdata_tostruct(rdata, &in_a, NULL);
			break;
		}
		default:
			result = ISC_R_NOTIMPLEMENTED;
			break;
		}
		break;
	}
	case dns_rdatatype_aaaa: {
		dns_rdata_in_aaaa_t in_aaaa;
		result = dns_rdata_tostruct(rdata, &in_aaaa, NULL);
		break;
	}
	case dns_rdatatype_afsdb: {
		dns_rdata_afsdb_t afsdb;
		result = dns_rdata_tostruct(rdata, &afsdb, NULL);
		break;
	}
	case dns_rdatatype_any: {
		result = ISC_R_NOTIMPLEMENTED;
		break;
	}
	case dns_rdatatype_cert: {
		dns_rdata_cert_t cert;
		result = dns_rdata_tostruct(rdata, &cert, NULL);
		break;
	}
	case dns_rdatatype_cname: {
		dns_rdata_cname_t cname;
		result = dns_rdata_tostruct(rdata, &cname, NULL);
		break;
	}
	case dns_rdatatype_dname: {
		dns_rdata_dname_t dname;
		result = dns_rdata_tostruct(rdata, &dname, NULL);
		break;
	}
	case dns_rdatatype_gpos: {
		dns_rdata_gpos_t gpos;
		result = dns_rdata_tostruct(rdata, &gpos, NULL);
		break;
	}
	case dns_rdatatype_hinfo: {
		dns_rdata_hinfo_t hinfo;
		result = dns_rdata_tostruct(rdata, &hinfo, NULL);
		break;
	}
	case dns_rdatatype_isdn: {
		dns_rdata_isdn_t isdn;
		result = dns_rdata_tostruct(rdata, &isdn, NULL);
		break;
	}
	case dns_rdatatype_key: {
		dns_rdata_key_t key;
		result = dns_rdata_tostruct(rdata, &key, NULL);
		break;
	}
	case dns_rdatatype_kx: {
		dns_rdata_in_kx_t in_kx;
		result = dns_rdata_tostruct(rdata, &in_kx, NULL);
		break;
	}
	case dns_rdatatype_loc: {
		dns_rdata_loc_t loc;
		result = dns_rdata_tostruct(rdata, &loc, NULL);
		break;
	}
	case dns_rdatatype_mb: {
		dns_rdata_mb_t mb;
		result = dns_rdata_tostruct(rdata, &mb, NULL);
		break;
	}
	case dns_rdatatype_md: {
		dns_rdata_md_t md;
		result = dns_rdata_tostruct(rdata, &md, NULL);
		break;
	}
	case dns_rdatatype_mf: {
		dns_rdata_mf_t mf;
		result = dns_rdata_tostruct(rdata, &mf, NULL);
		break;
	}
	case dns_rdatatype_mg: {
		dns_rdata_mg_t mg;
		result = dns_rdata_tostruct(rdata, &mg, NULL);
		break;
	}
	case dns_rdatatype_minfo: {
		dns_rdata_minfo_t minfo;
		result = dns_rdata_tostruct(rdata, &minfo, NULL);
		break;
	}
	case dns_rdatatype_mr: {
		dns_rdata_mr_t mr;
		result = dns_rdata_tostruct(rdata, &mr, NULL);
		break;
	}
	case dns_rdatatype_mx: {
		dns_rdata_mx_t mx;
		result = dns_rdata_tostruct(rdata, &mx, NULL);
		break;
	}
	case dns_rdatatype_naptr: {
		dns_rdata_in_naptr_t in_naptr;
		result = dns_rdata_tostruct(rdata, &in_naptr, NULL);
		break;
	}
	case dns_rdatatype_ns: {
		dns_rdata_ns_t ns;
		result = dns_rdata_tostruct(rdata, &ns, NULL);
		break;
	}
	case dns_rdatatype_nsap: {
		dns_rdata_in_nsap_t in_nsap;
		result = dns_rdata_tostruct(rdata, &in_nsap, NULL);
		break;
	}
	case dns_rdatatype_nsap_ptr: {
		dns_rdata_in_nsap_ptr_t in_nsap_ptr;
		result = dns_rdata_tostruct(rdata, &in_nsap_ptr, NULL);
		break;
	}
	case dns_rdatatype_null: {
		dns_rdata_null_t null;
		result = dns_rdata_tostruct(rdata, &null, NULL);
		break;
	}
	case dns_rdatatype_nxt: {
		dns_rdata_nxt_t nxt;
		result = dns_rdata_tostruct(rdata, &nxt, NULL);
		break;
	}
	case dns_rdatatype_opt: {
		dns_rdata_opt_t opt;
		result = dns_rdata_tostruct(rdata, &opt, NULL);
		break;
	}
	case dns_rdatatype_ptr: {
		dns_rdata_ptr_t ptr;
		result = dns_rdata_tostruct(rdata, &ptr, NULL);
		break;
	}
	case dns_rdatatype_px: {
		dns_rdata_in_px_t in_px;
		result = dns_rdata_tostruct(rdata, &in_px, NULL);
		break;
	}
	case dns_rdatatype_rp: {
		dns_rdata_rp_t rp;
		result = dns_rdata_tostruct(rdata, &rp, NULL);
		break;
	}
	case dns_rdatatype_rt: {
		dns_rdata_rt_t rt;
		result = dns_rdata_tostruct(rdata, &rt, NULL);
		break;
	}
	case dns_rdatatype_sig: {
		dns_rdata_sig_t sig;
		result = dns_rdata_tostruct(rdata, &sig, NULL);
		break;
	}
	case dns_rdatatype_soa: {
		dns_rdata_soa_t soa;
		result = dns_rdata_tostruct(rdata, &soa, NULL);
		break;
	}
	case dns_rdatatype_srv: {
		dns_rdata_in_srv_t in_srv;
		result = dns_rdata_tostruct(rdata, &in_srv, NULL);
		break;
	}
	case dns_rdatatype_tkey: {
		dns_rdata_tkey_t tkey;
		result = dns_rdata_tostruct(rdata, &tkey, NULL);
		break;
	}
	case dns_rdatatype_txt: {
		dns_rdata_txt_t txt;
		result = dns_rdata_tostruct(rdata, &txt, NULL);
		break;
	}
	case dns_rdatatype_unspec: {
		dns_rdata_unspec_t unspec;
		result = dns_rdata_tostruct(rdata, &unspec, NULL);
		break;
	}
	case dns_rdatatype_wks: {
		dns_rdata_in_wks_t in_wks;
		result = dns_rdata_tostruct(rdata, &in_wks, NULL);
		break;
	}
	case dns_rdatatype_x25: {
		dns_rdata_x25_t x25;
		result = dns_rdata_tostruct(rdata, &x25, NULL);
		break;
	}
	default:
		result = ISC_R_NOTIMPLEMENTED;
		break;
	}
	if (result != ISC_R_SUCCESS)
		printf("viastruct: tostuct %d %d return %s\n",
			rdata->type, rdata->rdclass,
			dns_result_totext(result));
}

int
main(int argc, char *argv[]) {
	isc_token_t token;
	isc_result_t result;
	int quiet = 0;
	int c;
	int stats = 0;
	unsigned int options = 0;
	dns_rdatatype_t type;
	dns_rdataclass_t class;
	dns_rdatatype_t lasttype = 0;
	char outbuf[16*1024];
	char inbuf[16*1024];
	char wirebuf[16*1024];
	isc_buffer_t dbuf;
	isc_buffer_t tbuf;
	isc_buffer_t wbuf;
	dns_rdata_t rdata;
	dns_rdata_t last;
	int need_eol = 0;
	int wire = 0;
	dns_compress_t cctx;
	dns_decompress_t dctx;
	int trunc = 0;
	int add = 0;
	int len;
	int zero = 0;
	int debug = 0;
	isc_region_t region;
	int first = 1;
	int raw = 0;
	int tostruct = 0;

	while ((c = isc_commandline_parse(argc, argv, "dqswtarzS")) != -1) {
		switch (c) {
		case 'd':
			debug = 1;
			quiet = 0;
			break;
		case 'q':
			quiet = 1;
			debug = 0;
			break;
		case 's':
			stats = 1;
			break;
		case 'w':
			wire = 1;
			break;
		case 't':
			trunc = 1;
			break;
		case 'a':
			add = 1;
			break;
		case 'z':
			zero = 1;
			break;
		case 'r':
			raw++;
			break;
		case 'S':
			tostruct++;
			break;
		}
	}

	memset(&dctx, '0', sizeof dctx);
	dctx.allowed = DNS_COMPRESS_ALL;

	RUNTIME_CHECK(isc_mem_create(0, 0, &mctx) == ISC_R_SUCCESS);
	RUNTIME_CHECK(isc_lex_create(mctx, 256, &lex) == ISC_R_SUCCESS);

	/* Set up to lex DNS master file. */

	specials['('] = 1;
	specials[')'] = 1;
	specials['"'] = 1;
	isc_lex_setspecials(lex, specials);
	options = ISC_LEXOPT_EOL;
	isc_lex_setcomments(lex, ISC_LEXCOMMENT_DNSMASTERFILE);

	RUNTIME_CHECK(isc_lex_openstream(lex, stdin) == ISC_R_SUCCESS);

	dns_rdata_init(&last);
	while ((result = isc_lex_gettoken(lex, options | ISC_LEXOPT_NUMBER,
					  &token)) == ISC_R_SUCCESS) {
		if (debug) fprintf(stdout, "token.type = %d\n", token.type);
		if (need_eol) {
			if (token.type == isc_tokentype_eol)
				need_eol = 0;
			continue;
		}
		if (token.type == isc_tokentype_eof)
			break;
	
		/* get type */
		if (token.type == isc_tokentype_number) {
			type = token.value.as_ulong;
			isc_buffer_init(&tbuf, outbuf, sizeof(outbuf));
			result = dns_rdatatype_totext(type, &tbuf);
			fprintf(stdout, "type = %.*s(%d)\n",
				(int)tbuf.used, (char*)tbuf.base, type);
		} else if (token.type == isc_tokentype_string) {
			result = dns_rdatatype_fromtext(&type,
					&token.value.as_textregion);
			if (result != ISC_R_SUCCESS) {
				fprintf(stdout,
				    "dns_rdatatype_fromtext returned %s(%d)\n",
					dns_result_totext(result), result);
				fflush(stdout);
				need_eol = 1;
				continue;
			}
			fprintf(stdout, "type = %.*s(%d)\n",
				(int)token.value.as_textregion.length,
				token.value.as_textregion.base, type);
		} else
			continue;

		if ((result = isc_lex_gettoken(lex, options | ISC_LEXOPT_NUMBER,
					  &token)) != ISC_R_SUCCESS)
				  break;
		if (token.type == isc_tokentype_eol)
				continue;
		if (token.type == isc_tokentype_eof)
				break;
		if (token.type == isc_tokentype_number) {
			class = token.value.as_ulong;
			isc_buffer_init(&tbuf, outbuf, sizeof(outbuf));
			result = dns_rdatatype_totext(class, &tbuf);
			fprintf(stdout, "class = %.*s(%d)\n",
				(int)tbuf.used, (char*)tbuf.base, class);
		} else if (token.type == isc_tokentype_string) {
			result = dns_rdataclass_fromtext(&class,
					&token.value.as_textregion);
			if (result != ISC_R_SUCCESS) {
				fprintf(stdout,
				    "dns_rdataclass_fromtext returned %s(%d)\n",
					dns_result_totext(result), result);
				fflush(stdout);
				need_eol = 1;
				continue;
			}
			fprintf(stdout, "class = %.*s(%d)\n",
				(int)token.value.as_textregion.length,
				token.value.as_textregion.base, class);
		} else
			continue;

		fflush(stdout);
		dns_rdata_init(&rdata);
		isc_buffer_init(&dbuf, inbuf, sizeof(inbuf));
		result = dns_rdata_fromtext(&rdata, class, type, lex,
					    NULL, ISC_FALSE, &dbuf, NULL);
		if (result != ISC_R_SUCCESS) {
			fprintf(stdout,
				"dns_rdata_fromtext returned %s(%d)\n",
				dns_result_totext(result), result);
			fflush(stdout);
			continue;
		}
		if (raw) {
			unsigned int i;
			for (i = 0 ; i < rdata.length ; /* */ ) {
				fprintf(stdout, "%02x", rdata.data[i]);
				if ((++i % 20) == 0)
					fputs("\n", stdout);
				else
					if (i == rdata.length)
						fputs("\n", stdout);
					else
						fputs(" ", stdout);
			}
		}

		/* Convert to wire and back? */
		if (wire) {
			result = dns_compress_init(&cctx, -1, mctx);
			if (result != ISC_R_SUCCESS) {
				fprintf(stdout,
					"dns_compress_init returned %s(%d)\n",
					dns_result_totext(result), result);
				continue;
			}
			isc_buffer_init(&wbuf, wirebuf, sizeof(wirebuf));
			result = dns_rdata_towire(&rdata, &cctx, &wbuf);
			dns_compress_invalidate(&cctx);
			if (result != ISC_R_SUCCESS) {
				fprintf(stdout,
					"dns_rdata_towire returned %s(%d)\n",
					dns_result_totext(result), result);
				continue;
			}
			len = wbuf.used - wbuf.current;
			if (raw > 2) {
				unsigned int i;
				fputs("\n", stdout);
				for (i = 0 ; i < (unsigned int)len ; /* */ ) {
					fprintf(stdout, "%02x",
				((unsigned char*)wbuf.base)[i + wbuf.current]);
					if ((++i % 20) == 0)
						fputs("\n", stdout);
					else
						if (i == wbuf.used)
							fputs("\n", stdout);
						else
							fputs(" ", stdout);
				}
			}
			if (zero)
				len = 0;
			if (trunc)
				len = (len * 3) / 4;
			if (add) {
				isc_buffer_add(&wbuf, len / 4 + 1);
				len += len / 4 + 1;
			}

			isc_buffer_setactive(&wbuf, len);
			dns_rdata_init(&rdata);
			isc_buffer_init(&dbuf, inbuf, sizeof(inbuf));
			dns_decompress_init(&dctx, -1, ISC_FALSE);
			result = dns_rdata_fromwire(&rdata, class, type, &wbuf,
						    &dctx, ISC_FALSE, &dbuf);
			dns_decompress_invalidate(&dctx);
			if (result != ISC_R_SUCCESS) {
			fprintf(stdout,
					"dns_rdata_fromwire returned %s(%d)\n",
					dns_result_totext(result), result);
				fflush(stdout);
				continue;
			}
		}
		if (raw > 1) {
			unsigned int i;
			fputs("\n", stdout);
			for (i = 0 ; i < rdata.length ; /* */ ) {
				fprintf(stdout, "%02x", rdata.data[i]);
				if ((++i % 20) == 0)
					fputs("\n", stdout);
				else
					if (i == rdata.length)
						fputs("\n", stdout);
					else
						fputs(" ", stdout);
			}
		}
		if (tostruct)
			viastruct(&rdata);

		isc_buffer_init(&tbuf, outbuf, sizeof(outbuf));
		result = dns_rdata_totext(&rdata, NULL, &tbuf);
		if (result != ISC_R_SUCCESS)
			fprintf(stdout, "dns_rdata_totext returned %s(%d)\n",
				dns_result_totext(result), result);
		else
			fprintf(stdout, "\"%.*s\"\n",
				(int)tbuf.used, (char*)tbuf.base);
		fflush(stdout);
		if (lasttype == type) {
			fprintf(stdout, "dns_rdata_compare = %d\n",
				dns_rdata_compare(&rdata, &last));

		}
		if (!first) {
			free(last.data);
		}
		dns_rdata_init(&last);
		region.base = malloc(region.length = rdata.length);
		if (region.base) {
			memcpy(region.base, rdata.data, rdata.length);
			dns_rdata_fromregion(&last, class, type, &region);
			lasttype = type;
			first = 0;
		} else
			first = 1;

	}
	if (result != ISC_R_EOF)
		printf("Result: %s\n", isc_result_totext(result));

	isc_lex_close(lex);
	isc_lex_destroy(&lex);
	if (!quiet && stats)
		isc_mem_stats(mctx, stdout);
	isc_mem_destroy(&mctx);

	return (0);
}
