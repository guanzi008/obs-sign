/*
 * Copyright (c) 2018-2023 SUSE LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 ***************************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "inc.h"

static const byte cert_version_3[] = { 0x05, 0xa0, 0x03, 0x02, 0x01, 0x02 };
static const byte oid_common_name[] = { 0x05, 0x06, 0x03, 0x55, 0x04, 0x03 };
static const byte oid_email_address[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01 };

static const byte oid_rsa_encryption[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01 };
static const byte oid_dsa_encryption[] = { 0x09, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x38, 0x04, 0x01 };
static const byte oid_ed25519[] = { 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70 };
static const byte oid_ec_public_key[] = { 0x09, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01 };
static const byte oid_mldsa65[] = { 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x03, 0x12 };

static const byte oid_prime256v1[] = { 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 };
static const byte oid_secp384r1[] = { 0x07, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22 };

static const byte sig_algo_rsa_sha1[]   = { 0x0f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00 };
static const byte sig_algo_rsa_sha256[] = { 0x0f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00 };
static const byte sig_algo_rsa_sha512[] = { 0x0f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0d, 0x05, 0x00 };
static const byte sig_algo_dsa_sha1[]   = { 0x0b, 0x30, 0x09, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x38, 0x04, 0x03 };
static const byte sig_algo_dsa_sha256[] = { 0x0d, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x03, 0x02 };
static const byte sig_algo_dsa_sha512[] = { 0x0d, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x03, 0x04 };
static const byte sig_algo_ecdsa_sha1[]   = { 0x0b, 0x30, 0x09, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x01 };
static const byte sig_algo_ecdsa_sha256[] = { 0x0c, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02 };
static const byte sig_algo_ecdsa_sha512[] = { 0x0c, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x04 };

static const byte digest_algo_sha1[] = { 0x0b, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00 };
static const byte digest_algo_sha256[] = { 0x0f, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00 };
static const byte digest_algo_sha512[] = { 0x0f, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00 };

static const byte oid_subject_key_identifier[] = { 0x05, 0x06, 0x03, 0x55, 0x1d, 0x0e };
static const byte subject_key_identifier[] = { 0x1f, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const byte authority_key_identifier[] = { 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const byte basic_constraints[] = { 0x0e, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x02, 0x30, 0x00 };
static const byte key_usage[] = { 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x02, 0x84 };
static const byte ext_key_usage[] = { 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x1d, 0x25, 0x04, 0x0c, 0x30, 0x0a, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x03 };

static const byte oid_contenttype[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x03 };
static const byte oid_messagedigest[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x04 };
static const byte oid_signingtime[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x05 };

static const byte oid_spc_indirect_data[] = { 0x0c, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x04 };
static const byte oid_spc_spopusinfo[] = { 0x0c, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x0c };
static const byte oid_spc_statementtype[] = { 0x0c, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x0b };
static const byte oid_ms_codesigning[] = { 0x0c, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x15 };

static const byte oid_pkcs7_data[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x01 };
static const byte oid_pkcs7_signed_data[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x02 };
static const byte oid_pkcs7_smime_capabilities[] = { 0x0b, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x0f };

static const byte int_1[] = { 0x03, 0x02, 0x01, 0x01 };
static const byte int_3[] = { 0x03, 0x02, 0x01, 0x03 };

static const byte gpg_ed25519[] = { 0x09, 0x2b, 0x06, 0x01, 0x04, 0x01, 0xda, 0x47, 0x0f, 0x01, 0x18 };
static const byte gpg_nistp256[] = { 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 };
static const byte gpg_nistp384[] = { 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22 };

static void x509_skip(unsigned char **bpp, int *lp, int expected);

static void
x509_room(struct x509 *cb, int l)
{
  if (l < 0 || l > 100000 || cb->len > 100000)
    dodie("x509_room: illegal size");
  if (cb->len + l > cb->alen)
    {
      cb->alen = cb->len + l + 256;
      cb->buf = dorealloc(cb->buf, cb->alen);
    }
}

static void
x509_add(struct x509 *cb, const byte *blob, int blobl)
{
  x509_room(cb, blobl);
  if (blob)
    memmove(cb->buf + cb->len, blob, blobl);
  else
    memset(cb->buf + cb->len, 0, blobl);
  cb->len += blobl;
}

void
x509_insert(struct x509 *cb, int offset, const byte *blob, int blobl)
{
  if (offset < 0 || offset > cb->len)
    abort();
  x509_room(cb, blobl);
  if (offset < cb->len)
    memmove(cb->buf + offset + blobl, cb->buf + offset, cb->len - offset);
  if (blob)
    memmove(cb->buf + offset, blob, blobl);
  else
    memset(cb->buf + offset, 0, blobl);
  cb->len += blobl;
}

/* convenience */
static inline void
x509_add_const(struct x509 *cb, const byte *c)
{
  x509_add(cb, c + 1, c[0]);
}

static inline void
x509_insert_const(struct x509 *cb, int offset, const byte *c)
{
  x509_insert(cb, offset, c + 1, c[0]);
}


/* ASN.1 primitives */

static void
x509_tag(struct x509 *cb, int offset, int tag)
{
  int ll, l = cb->len - offset;
  if (l < 0 || l >= 0x1000000)
    abort();
  ll = l < 0x80 ? 0 : l < 0x100 ? 1 : l < 0x10000 ? 2 : 3;
  x509_insert(cb, offset, 0, 2 + ll);
  if (ll)
    cb->buf[offset + 1] = 0x80 + ll;
  if (ll > 2)
    cb->buf[offset + ll - 1] = l >> 16;
  if (ll > 1)
    cb->buf[offset + ll] = l >> 8;
  cb->buf[offset + ll + 1] = l;
  cb->buf[offset] = tag;
}

static void
x509_tag_impl(struct x509 *cb, int offset, int tag)
{
  if (cb->len <= offset)
    return;
  cb->buf[offset] = tag | (cb->buf[offset] & 0x20);	/* keep CONS */
}

static void
x509_mpiint(struct x509 *cb, byte *p, int pl)
{
  int offset = cb->len;
  while (pl && !*p)
    {
      p++;
      pl--;
    }
  if (!pl || p[0] >= 128)
    x509_add(cb, 0, 1);
  if (pl)
    x509_add(cb, p, pl);
  x509_tag(cb, offset, 0x02);
}

static void
x509_octed_string(struct x509 *cb, const unsigned char *blob, int blobl)
{
  int offset = cb->len;
  x509_add(cb, blob, blobl);
  x509_tag(cb, offset, 0x04);
}

static int
x509_set_of_sort_cmp(const void *va, const void *vb)
{
  unsigned char *ab = ((unsigned char **)va)[0];
  size_t as = ((unsigned char **)va)[1] - ab;
  unsigned char *bb = ((unsigned char **)vb)[0];
  size_t bs = ((unsigned char **)vb)[1] - bb;
  size_t m = as < bs ? as : bs;
  int r = memcmp(ab, bb, m);
  return r ? r : as < bs ? -1 : as > bs ? 1 : 0;
}

/* ASN.1 DER encoding wants sorted SET OF elements */
static void
x509_set_of(struct x509 *cb, int offset)
{
  int i, n, l, len = cb->len;
  unsigned char *b, **offs;

  for (b = cb->buf + offset, l = len - offset, n = 0; l > 0; n++)
    x509_skip(&b, &l, 0);
  if (n < 2)
    {
      x509_tag(cb, offset, 0x31);
      return;
    }
  x509_room(cb, len - offset);	/* do this now so that the cb->buf does not change */
  offs = doalloc(2 * n * sizeof(unsigned char *));
  for (b = cb->buf + offset, l = len - offset, n = 0; l > 0; n++)
    {
      offs[2 * n] = b;
      x509_skip(&b, &l, 0);
      offs[2 * n + 1] = b;
    }
  qsort(offs, n, 2 * sizeof(unsigned char *), x509_set_of_sort_cmp);
  for (i = 0; i < n; i++)
    x509_add(cb, offs[2 * i], offs[2 * i + 1] - offs[2 * i]);
  memmove(cb->buf + offset, cb->buf + len, cb->len - len);
  cb->alen += len - offset;
  cb->len -= len - offset;
  free(offs);
  x509_tag(cb, offset, 0x31);
}

/* X509 helpers */

static void
x509_time(struct x509 *cb, time_t t)
{
  int offset = cb->len;
  struct tm *tm = gmtime(&t);
  char tbuf[256];
  if (!tm || tm->tm_year < 0 || tm->tm_year >= 8100)
    dodie("x509_time: time out of range");
  sprintf(tbuf, "%04d%02d%02d%02d%02d%02dZ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  if (tm->tm_year >= 50 && tm->tm_year < 150)
    {
      x509_add(cb, (byte *)tbuf + 2, strlen(tbuf + 2));
      x509_tag(cb, offset, 0x17);
    }
  else
    {
      x509_add(cb, (byte *)tbuf, strlen(tbuf));
      x509_tag(cb, offset, 0x18);
    }
}

static void
x509_random_serial(struct x509 *cb)
{
  int offset = cb->len;
  int i;
  x509_add(cb, 0, 20);
  for (i = 0; i < 20; i++)
    cb->buf[offset + i] = (byte)random();
  cb->buf[offset] &= 0x3f;
  cb->buf[offset] |= 0x40;
  x509_tag(cb, offset, 0x02);
}

static void
x509_dn(struct x509 *cb, const char *cn, const char *email)
{
  int offset = cb->len;
  if (cn && *cn)
    {
      int offset2 = cb->len;
      x509_add(cb, (byte *)cn, strlen(cn));
      x509_tag(cb, offset2, 0x0c);
      x509_insert_const(cb, offset2, oid_common_name);
      x509_tag(cb, offset2, 0x30);
      x509_tag(cb, offset2, 0x31);
    }
  if (email && *email)
    {
      int offset2 = cb->len;
      x509_add(cb, (byte *)email, strlen(email));
      for (; *email; email++)
	if (*(unsigned char *)email >= 128)
	  break;
      x509_tag(cb, offset2, *email ? 0x0c: 0x16);
      x509_insert_const(cb, offset2, oid_email_address);
      x509_tag(cb, offset2, 0x30);
      x509_tag(cb, offset2, 0x31);
    }
  x509_tag(cb, offset, 0x30);
}

static void
x509_validity(struct x509 *cb, time_t start, time_t end)
{
  int offset = cb->len;
  x509_time(cb, start);
  x509_time(cb, end);
  x509_tag(cb, offset, 0x30);
}

static void
x509_algoid_digest(struct x509 *cb, int algo)
{
  if (algo == HASH_SHA1)
    x509_add_const(cb, digest_algo_sha1);
  else if (algo == HASH_SHA256)
    x509_add_const(cb, digest_algo_sha256);
  else if (algo == HASH_SHA512)
    x509_add_const(cb, digest_algo_sha512);
  else
    abort();
}

static void
x509_algoid(struct x509 *cb, int pubalgo, byte **mpi, int *mpil)
{
  int offset = cb->len;
  if (pubalgo == PUB_RSA)
    {
      x509_add_const(cb, oid_rsa_encryption);
      x509_tag(cb, cb->len, 0x05);	/* NULL */
    }
  else if (pubalgo == PUB_DSA)
    {
      x509_add_const(cb, oid_dsa_encryption);
      if (mpi)
	{
	  int offset2 = cb->len;
	  x509_mpiint(cb, mpi[0], mpil[0]);
	  x509_mpiint(cb, mpi[1], mpil[1]);
	  x509_mpiint(cb, mpi[2], mpil[2]);
	  x509_tag(cb, offset2, 0x30);
	}
    }
  else if (pubalgo == PUB_EDDSA)
    {
      if (mpi && mpil[0] == gpg_ed25519[0] && !memcmp(mpi[0], gpg_ed25519 + 1, mpil[0]))
	x509_add_const(cb, oid_ed25519);
      else
	dodie("x509_pubkey: unsupported EdDSA curve");
    }
  else if (pubalgo == PUB_ECDSA)
    {
      if (mpi && mpil[0] == gpg_nistp256[0] && !memcmp(mpi[0], gpg_nistp256 + 1, mpil[0]))
        {
	  x509_add_const(cb, oid_ec_public_key);
	  x509_add_const(cb, oid_prime256v1);
        }
      else if (mpi && mpil[0] == gpg_nistp384[0] && !memcmp(mpi[0], gpg_nistp384 + 1, mpil[0]))
        {
	  x509_add_const(cb, oid_ec_public_key);
	  x509_add_const(cb, oid_secp384r1);
        }
      else
	dodie("x509_pubkey: unsupported ECDSA curve");
    }
  else if (pubalgo == PUB_MLDSA65)
    {
      x509_add_const(cb, oid_mldsa65);
    }
  else
    abort();
  x509_tag(cb, offset, 0x30);
}

static void
x509_algoid_sig(struct x509 *cb, int pubalgo, int algo)
{
  if (pubalgo == PUB_RSA && algo == HASH_SHA1)
    x509_add_const(cb, sig_algo_rsa_sha1);
  else if (pubalgo == PUB_RSA && algo == HASH_SHA256)
    x509_add_const(cb, sig_algo_rsa_sha256);
  else if (pubalgo == PUB_RSA && algo == HASH_SHA512)
    x509_add_const(cb, sig_algo_rsa_sha512);
  else if (pubalgo == PUB_DSA && algo == HASH_SHA1)
    x509_add_const(cb, sig_algo_dsa_sha1);
  else if (pubalgo == PUB_DSA && algo == HASH_SHA256)
    x509_add_const(cb, sig_algo_dsa_sha256);
  else if (pubalgo == PUB_DSA && algo == HASH_SHA512)
    x509_add_const(cb, sig_algo_dsa_sha512);
  else if (pubalgo == PUB_ECDSA && algo == HASH_SHA1)
    x509_add_const(cb, sig_algo_ecdsa_sha1);
  else if (pubalgo == PUB_ECDSA && algo == HASH_SHA256)
    x509_add_const(cb, sig_algo_ecdsa_sha256);
  else if (pubalgo == PUB_ECDSA && algo == HASH_SHA512)
    x509_add_const(cb, sig_algo_ecdsa_sha512);
  else
    {
      fprintf(stderr, "unsupported pubalgo/hashalgo combination: %d/%d\n", pubalgo, algo);
      exit(1);
    }
}

static void
x509_pubkey(struct x509 *cb, int pubalgo, byte **mpi, int *mpil, byte *keyid)
{
  int offset = cb->len, offset2;
  x509_algoid(cb, pubalgo, mpi, mpil);
  offset2 = cb->len;
  if (pubalgo == PUB_RSA)
    {
      x509_mpiint(cb, mpi[0], mpil[0]);
      x509_mpiint(cb, mpi[1], mpil[1]);
      x509_tag(cb, offset2, 0x30);
    }
  else if (pubalgo == PUB_DSA)
    x509_mpiint(cb, mpi[3], mpil[3]);
  else if (pubalgo == PUB_EDDSA)
    {
      if (mpil[1] < 2 || mpi[1][0] != 0x40)
	dodie("x509_pubkey: bad EdDSA point");
      x509_add(cb, mpi[1] + 1, mpil[1] - 1);
    }
  else if (pubalgo == PUB_ECDSA)
    {
      if (mpil[1] < 2 || mpi[1][0] != 0x04)
	dodie("x509_pubkey: bad ECDSA point");
      x509_add(cb, mpi[1], mpil[1]);
    }
  else
    dodie("x509_pubkey: unsupported algorithm");
  if (keyid)
    {
      SHA1_CONTEXT ctx;
      sha1_init(&ctx);
      sha1_write(&ctx, cb->buf + offset2, cb->len - offset2);
      sha1_final(&ctx);
      memcpy(keyid, sha1_read(&ctx), 20);
    }
  x509_insert(cb, offset2, 0, 1);
  x509_tag(cb, offset2, 0x03);
  x509_tag(cb, offset, 0x30);
}

void
x509_signature(struct x509 *cb, int pubalgo, byte **mpi, int *mpil)
{
  if (pubalgo == PUB_RSA)
    {
      /* zero pad to multiple of 16 */
      int nbytes = (mpil[0] + 15) & ~15;
      if (nbytes > mpil[0])
        x509_add(cb, 0, nbytes - mpil[0]);
      x509_add(cb, mpi[0], mpil[0]);
    }
  else if (pubalgo == PUB_DSA || pubalgo == PUB_ECDSA)
    {
      x509_mpiint(cb, mpi[0], mpil[0]);
      x509_mpiint(cb, mpi[1], mpil[1]);
      x509_tag(cb, 0, 0x30);
    }
  else
    {
      fprintf(stderr, "unsupported signature algo %d\n", pubalgo);
      exit(1);
    }
}

static void
x509_extensions(struct x509 *cb, byte *keyid)
{
  int offset = cb->len;
  /* basic contraints */
  x509_add_const(cb, basic_constraints);
  if (keyid)
    {
      x509_add_const(cb, subject_key_identifier);
      memcpy(cb->buf + cb->len - 20, keyid, 20);
      x509_add_const(cb, authority_key_identifier);
      memcpy(cb->buf + cb->len - 20, keyid, 20);
    }
  x509_add_const(cb, key_usage);
  x509_add_const(cb, ext_key_usage);
  x509_tag(cb, offset, 0x30);
  x509_tag(cb, offset, 0xa3);	/* CONT | CONS | 3 */
}

/* create an unsigned self-signed cert */
void
x509_tbscert(struct x509 *cb, const char *cn, const char *email, time_t start, time_t end, int pubalgo, byte **mpi, int *mpil)
{
  int offset = cb->len;
  byte keyid[20];
  x509_add_const(cb, cert_version_3);
  x509_random_serial(cb);
  x509_algoid_sig(cb, pubalgo, hashalgo);
  x509_dn(cb, cn, email);
  x509_validity(cb, start, end);
  x509_dn(cb, cn, email);
  x509_pubkey(cb, pubalgo, mpi, mpil, keyid);
  x509_extensions(cb, keyid);
  x509_tag(cb, offset, 0x30);
}

void
x509_finishcert(struct x509 *cb, int pubalgo, struct x509 *sigcb)
{
  x509_algoid_sig(cb, pubalgo, hashalgo);
  x509_add(cb, 0, 1);
  x509_add(cb, sigcb->buf, sigcb->len);
  x509_tag(cb, cb->len - (sigcb->len + 1), 0x03);
  x509_tag(cb, 0, 0x30);
}

void
certsizelimit(char *s, int l)
{
  if (strlen(s) <= l)
    return;
  s += l - 4;
  for (l = 0; l < 3; l++, s--)
    if ((*s & 0xc0) != 0x80)
      break;
  strcpy(s, "...");
}

int
x509_addpem(struct x509 *cb, char *buf, char *type)
{
  int offset = cb->len;
  size_t typel = strlen(type);
  unsigned char *bp;

  while (*buf == ' ' || *buf == '\t' || *buf == '\n' || *buf == '\r')
    buf++;
  if (strncmp(buf, "-----BEGIN ", 11) != 0 || strncmp(buf + 11, type, typel) != 0 || strncmp(buf + 11 + typel, "-----\n", 6) != 0)
    return 0;
  buf += 11 + 6 + typel;
  x509_room(cb, strlen(buf) * 3 / 4 + 16);
  bp = cb->buf + offset;
  buf = r64dec(buf, &bp);
  if (!buf)
    return 0;
  while (*buf == ' ' || *buf == '\t' || *buf == '\n' || *buf == '\r')
    buf++;
  if (strncmp(buf, "-----END ", 9) != 0 || strncmp(buf + 9, type, typel) != 0 || strncmp(buf + 9 + typel, "-----\n", 6) != 0)
    return 0;
  cb->len = bp - cb->buf;
  return 1;
}

static int
x509_unpack(unsigned char *bp, int l, unsigned char **dpp, int *dlp, int *clp, int expected)
{
  unsigned char *bporig = bp;
  int tag, tl;
  if (l < 2)
    dodie("x509_unpack: unexpected EOF");
  tag = bp[0];
  tl = bp[1];
  bp += 2;
  l -= 2;
  if (tl >= 128)
    {
      int ll = 0;
      tl -= 128;
      if (tl < 1 || tl > 3)
	{
	  fprintf(stderr, "x509_unpack: unsupported len %d\n", tl);
	  exit(1);
	}
      if (l < tl)
	dodie("x509_unpack: unexpected EOF in len");
      for (; tl > 0; tl--, l--)
	ll = (ll << 8) + *bp++;
      tl = ll;
    }
  if (tl < 0 || tl > l)
    dodie("x509_unpack: unexpected EOF");
  *dpp = bp;
  *dlp = tl;
  if (clp)
    *clp = bp - bporig + tl;
  if (expected && tag != expected)
    {
      fprintf(stderr, "x509_unpack: unexpeced tag %x, expected %x\n", tag, expected);
      exit(1);
    }
  return tag;
}

static int
x509_unpack_tag(unsigned char *bp, int l)
{
  return l < 2 ? 0 : bp[0];
}

static void
x509_skip(unsigned char **bpp, int *lp, int expected)
{
  unsigned char *dp;
  int dl, cl;
  x509_unpack(*bpp, *lp, &dp, &dl, &cl, expected);
  *bpp = dp + dl;
  *lp -= cl;
}

static void
x509_skip_optional(unsigned char **bpp, int *lp, int expected)
{
  if (x509_unpack_tag(*bpp, *lp) == expected)
    x509_skip(bpp, lp, expected);
}

static void
x509_zoom(unsigned char **bpp, int *lp, int expected)
{
  x509_unpack(*bpp, *lp, bpp, lp, 0, expected);
}

/*
 * pkcs7/CMS stuff, see RFC 2315 and RFC 5652
 *
 */

/* copy issuer and serial from cert */
static void
x509_issuerandserial(struct x509 *cb, unsigned char *cert, int certlen)
{
  int offset = cb->len;
  unsigned char *dp;
  int dl, cl;
  unsigned char *serial;
  int seriallen;

  x509_zoom(&cert, &certlen, 0x30);
  x509_zoom(&cert, &certlen, 0x30);
  x509_skip_optional(&cert, &certlen, 0xa0);	/* skip version */
  x509_unpack(cert, certlen, &dp, &dl, &cl, 0x02);
  serial = cert;
  seriallen = cl;
  cert = dp + dl;
  certlen -= cl;
  x509_skip(&cert, &certlen, 0x30);	/* skip signature algorithm */
  x509_unpack(cert, certlen, &dp, &dl, &cl, 0x30);
  x509_add(cb, cert, cl);
  x509_add(cb, serial, seriallen);
  x509_tag(cb, offset, 0x30);
}

/* copy subject key id from cert */
static void
x509_subjectkeyid(struct x509 *cb, unsigned char *cert, int certlen)
{
  unsigned char *b = cert;
  int l = certlen;
  x509_zoom(&b, &l, 0x30);
  x509_zoom(&b, &l, 0x30);
  x509_skip_optional(&b, &l, 0xa0);	/* skip version */
  x509_skip(&b, &l, 0x02);	/* skip serial */
  x509_skip(&b, &l, 0x30);	/* skip sig algo */
  x509_skip(&b, &l, 0x30);	/* skip issuer */
  x509_skip(&b, &l, 0x30);	/* skip validity */
  x509_skip(&b, &l, 0x30);	/* skip subject */
  x509_skip(&b, &l, 0x30);	/* skip public key */
  x509_skip_optional(&b, &l, 0x81);	/* skip optional issuer unique id */
  x509_skip_optional(&b, &l, 0x82);	/* skip optional subject unique id */
  if (x509_unpack_tag(b, l) == 0xa3)
    {
      x509_zoom(&b, &l, 0xa3);
      x509_zoom(&b, &l, 0x30);
    }
  else
    l = 0;
  while (l > 0)
    {
      unsigned char *b2;
      int l2, cl;
      x509_unpack(b, l, &b2, &l2, &cl, 0x30);
      b += cl;
      l -= cl;
      if (l2 < 6 || memcmp(b2, oid_subject_key_identifier + 1, 5))
	continue;
      b2 += 5;
      l2 -= 5;
      x509_skip_optional(&b2, &l2, 0x01);	/* skip critical bit */
      x509_zoom(&b2, &l2, 0x04);
      x509_zoom(&b2, &l2, 0x04);
      x509_add(cb, b2, l2);
      return;
    }
  dodie("cert does not contain the subject key identifier extension");
}

static void
x509_signerinfo(struct x509 *cb, struct x509 *signedattrs, struct x509 *cert, int pubalgo, struct x509 *sigcb, int usekeyid)
{
  int offset = cb->len;

  if (usekeyid)
    {
      int offset2;
      x509_add_const(cb, int_3);	/* version 3 */
      /* refer to cert by subject key id */
      offset2 = cb->len;
      x509_subjectkeyid(cb, cert->buf, cert->len);
      x509_tag(cb, offset2, 0x80);
    }
  else
    {
      x509_add_const(cb, int_1);	/* version 1 */
      /* issuer and serial number */
      x509_issuerandserial(cb, cert->buf, cert->len);
    }
  x509_algoid_digest(cb, hashalgo);
  if (signedattrs)
    {
      int offset2 = cb->len;
      x509_add(cb, signedattrs->buf, signedattrs->len);
      x509_tag_impl(cb, offset2, 0xa0);	/* CONT | CONS | 0 */
    }
  x509_algoid(cb, pubalgo, 0, 0);
  x509_octed_string(cb, sigcb->buf, sigcb->len);
  x509_tag(cb, offset, 0x30);
}

static int
x509_identicalcert(struct x509 *cert, unsigned char *cert2, int cert2len)
{
  int offset, r;
  struct x509 cb;
  x509_init(&cb);
  x509_issuerandserial(&cb, cert->buf, cert->len);
  offset = cb.len;
  x509_issuerandserial(&cb, cert2, cert2len);
  r = cb.len == 2 * offset && memcmp(cb.buf, cb.buf + offset, offset) == 0;
  x509_free(&cb);
  return r;
}

static void
x509_add_othercerts(struct x509 *cb, struct x509 *cert, struct x509 *othercerts)
{
  unsigned char *bp = othercerts->buf;
  int l = othercerts->len;
  while (l > 0)
    {
      unsigned char *dp;
      int dl, cl;
      x509_unpack(bp, l, &dp, &dl, &cl, 0x30);
      if (!x509_identicalcert(cert, bp, cl))
	x509_add(cb, bp, cl);
      bp += cl;
      l -= cl;
    }
}

void
x509_pkcs7_signed_data(struct x509 *cb, struct x509 *contentinfo, struct x509 *signedattrs, int pubalgo, struct x509 *sigcb, struct x509 *cert, struct x509 *othercerts, int flags)
{
  int offset = cb->len, offset2;
  int usekeyid = flags & X509_PKCS7_USE_KEYID ? 1 : 0;
  x509_algoid_digest(cb, hashalgo);
  x509_tag(cb, offset, 0x31);			/* SET of digest algos */
  x509_insert_const(cb, offset, usekeyid ? int_3 : int_1);		/* version */
  /* contentinfo */
  if (contentinfo)
    x509_add(cb, contentinfo->buf, contentinfo->len);
  else
    {
      offset2 = cb->len;
      x509_add_const(cb, oid_pkcs7_data);
      x509_tag(cb, offset2, 0x30);
    }
  /* certs */
  if (!(flags & X509_PKCS7_NO_CERTS))
    {
      offset2 = cb->len;
      x509_add(cb, cert->buf, cert->len);
      if (othercerts)
	x509_add_othercerts(cb, cert, othercerts);
      x509_tag(cb, offset2, 0xa0);	/* CONT | CONS | 0 */
    }
  /* signerinfos */
  offset2 = cb->len;
  x509_signerinfo(cb, signedattrs, cert, pubalgo, sigcb, usekeyid);
  x509_tag(cb, offset2, 0x31);
  /* finish */
  x509_tag(cb, offset, 0x30);
  x509_tag(cb, offset, 0xa0);	/* CONT | CONS | 0 */
  x509_insert_const(cb, offset, oid_pkcs7_signed_data);
  x509_tag(cb, offset, 0x30);
}

static void
x509_addsignedattr(struct x509 *cb, int offset, const unsigned char *oid)
{
  x509_tag(cb, offset, 0x31);	/* make it a set (assuming there is just one entry) */
  x509_insert_const(cb, offset, oid);	/* prepend oid */
  x509_tag(cb, offset, 0x30);	/* make sequence */
}

static void
x509_addsignedattr_contenttype(struct x509 *cb, const unsigned char *oid)
{
  int offset = cb->len;
  x509_add_const(cb, oid);
  x509_addsignedattr(cb, offset, oid_contenttype);
}

static void
x509_addsignedattr_signtime(struct x509 *cb, time_t signtime)
{
  int offset = cb->len;
  x509_time(cb, signtime);
  x509_addsignedattr(cb, offset, oid_signingtime);
}

static void
x509_addsignedattr_messagedigest(struct x509 *cb, const unsigned char *digest, int digestlen)
{
  int offset = cb->len;
  x509_octed_string(cb, digest, digestlen);
  x509_addsignedattr(cb, offset, oid_messagedigest);
}

/* simple signed attributes generator just containing the signing time
   and pkcs7-data as content type */
void
x509_signedattrs(struct x509 *cb, unsigned char *digest, int digestlen, time_t signtime)
{
  int offset = cb->len;
  x509_addsignedattr_contenttype(cb, oid_pkcs7_data);
  if (signtime)
    x509_addsignedattr_signtime(cb, signtime);
  x509_addsignedattr_messagedigest(cb, digest, digestlen);
  /* return a set */
  x509_set_of(cb, offset);
}

static int
x509_pubkey2pubalgo(unsigned char *pubkey, int pubkeylen)
{
  x509_zoom(&pubkey, &pubkeylen, 0x30);
  x509_zoom(&pubkey, &pubkeylen, 0x30);	/* get public key info */
  x509_zoom(&pubkey, &pubkeylen, 0x06);	/* get object id */
  if (pubkeylen == 9 && !memcmp(pubkey, oid_rsa_encryption + 3, 9))
    return PUB_RSA;
  if (pubkeylen == 7 && !memcmp(pubkey, oid_dsa_encryption + 3, 7))
    return PUB_DSA;
  if (pubkeylen == 7 && !memcmp(pubkey, oid_ec_public_key + 3, 7))
    return PUB_ECDSA;
  if (pubkeylen == 3 && !memcmp(pubkey, oid_ed25519 + 3, 3))
    return PUB_EDDSA;	/* Ed25519 */
  if (pubkeylen == 9 && !memcmp(pubkey, oid_mldsa65 + 3, 9))
    return PUB_MLDSA65;
  return -1;
}

int
x509_cert2pubalgo(struct x509 *cert)
{
  unsigned char *b = cert->buf;
  int l = cert->len;
  x509_zoom(&b, &l, 0x30);
  x509_zoom(&b, &l, 0x30);
  x509_skip_optional(&b, &l, 0xa0);	/* skip version */
  x509_skip(&b, &l, 0x02);	/* skip serial */
  x509_skip(&b, &l, 0x30);	/* skip sig algo */
  x509_skip(&b, &l, 0x30);	/* skip issuer */
  x509_skip(&b, &l, 0x30);	/* skip validity */
  x509_skip(&b, &l, 0x30);	/* skip subject */
  return x509_pubkey2pubalgo(b, l);
}



/*
 * Special SPC functions needed for appx signing
 *
 * info: http://download.microsoft.com/download/9/c/5/9c5b2167-8017-4bae-9fde-d599bac8184a/authenticode_pe.docx 
 */

/* SpcIndirectDataContent */
int
x509_appx_contentinfo(struct x509 *cb, unsigned char *digest, int digestlen)
{
   /* SEQUENCE          
    *  OBJECT            :1.3.6.1.4.1.311.2.1.30 [SPC_SIPINFO]
    *  SEQUENCE          
    *   INTEGER           :01010000
    *   OCTET STRING      [HEX DUMP]:4BDFC50A07CEE24DB76E23C839A09FD1
    *   INTEGER           :00
    *   INTEGER           :00
    *   INTEGER           :00
    *   INTEGER           :00
    *   INTEGER           :00
    *
    * SIP stands for "Subject Interface Package"
    * the octet string is the GUID 0AC5DF4B-CE07-4DE2-B76E-23C839A09FD1
    */
  static byte spcinfodata[] = {
    0x30, 0x35, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x1e, 0x30, 0x27,
    0x02, 0x04, 0x01, 0x01, 0x00, 0x00, 0x04, 0x10, 0x4b, 0xdf, 0xc5, 0x0a, 0x07, 0xce, 0xe2, 0x4d,
    0xb7, 0x6e, 0x23, 0xc8, 0x39, 0xa0, 0x9f, 0xd1, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01,
    0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00
  };
  int offset = cb->len;
  int contentlen;
  /* DigestInfo */
  x509_algoid_digest(cb, hashalgo);
  x509_octed_string(cb, digest, digestlen);
  x509_tag(cb, offset, 0x30);
  /* SpcAttributeTypeAndOptionalValue */
  x509_insert(cb, offset, spcinfodata, sizeof(spcinfodata));
  contentlen = cb->len - offset;

  x509_tag(cb, offset, 0x30);
  x509_tag(cb, offset, 0xa0);	/* CONT | CONS | 0 */
  x509_insert_const(cb, offset, oid_spc_indirect_data);
  x509_tag(cb, offset, 0x30);
  return cb->len - contentlen;	/* offset to content */
}

void
x509_appx_signedattrs(struct x509 *cb, unsigned char *digest, int digestlen, time_t signtime)
{
  int offset = cb->len, offset2;

  /* opusinfo attribute */
  offset2 = cb->len;
  x509_tag(cb, offset2, 0x30);
  x509_addsignedattr(cb, offset2, oid_spc_spopusinfo);
  /* contenttype attribute */
  x509_addsignedattr_contenttype(cb, oid_spc_indirect_data);
  /* signingtime attribute */
  if (signtime)
    x509_addsignedattr_signtime(cb, signtime);
  /* statementtype attribute */
  offset2 = cb->len;
  x509_add_const(cb, oid_ms_codesigning);
  x509_tag(cb, offset2, 0x30);
  x509_addsignedattr(cb, offset2, oid_spc_statementtype);
  /* message digest attribute */
  x509_addsignedattr_messagedigest(cb, digest, digestlen);
  /* return a set */
  x509_set_of(cb, offset);
}


/*
 * Special functions needed for PE authenticode signing
 *
 * info: http://download.microsoft.com/download/9/c/5/9c5b2167-8017-4bae-9fde-d599bac8184a/authenticode_pe.docx 
 */

int
x509_pe_contentinfo(struct x509 *cb, unsigned char *digest, int digestlen)
{
   /* cons:       SEQUENCE          
    * prim:        OBJECT            :1.3.6.1.4.1.311.2.1.15 [SPC_PE_IMAGE_DATAOBJ]
    * cons:        SEQUENCE          
    * prim:         BIT STRING        
    * cons:         cont [ 0 ]        
    * cons:          cont [ 2 ]        
    * prim:           cont [ 0 ]        
    */
  static byte spcpeimagedata[] = {
    0x30, 0x17, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x0f,
    0x30, 0x09, 0x03, 0x01, 0x00, 0xa0, 0x04, 0xa2, 0x02, 0x80, 0x00
  };
  int offset = cb->len;
  int contentlen;
  /* DigestInfo */
  x509_algoid_digest(cb, hashalgo);
  x509_octed_string(cb, digest, digestlen);
  x509_tag(cb, offset, 0x30);
  x509_insert(cb, offset, spcpeimagedata, sizeof(spcpeimagedata));
  contentlen = cb->len - offset;
  x509_tag(cb, offset, 0x30);
  x509_tag(cb, offset, 0xa0);	/* CONT | CONS | 0 */
  x509_insert_const(cb, offset, oid_spc_indirect_data);
  x509_tag(cb, offset, 0x30);
  return cb->len - contentlen;	/* offset to content */
}

void
x509_pe_signedattrs(struct x509 *cb, unsigned char *digest, int digestlen, time_t signtime)
{
  int offset = cb->len, offset2;

  /* smime capabilities attribute */
  offset2 = cb->len;
  x509_tag(cb, offset2, 0x30);
  x509_addsignedattr(cb, offset2, oid_pkcs7_smime_capabilities);
  /* contenttype attribute */
  x509_addsignedattr_contenttype(cb, oid_spc_indirect_data);
  /* signingtime attribute */
  if (signtime)
    x509_addsignedattr_signtime(cb, signtime);
  /* message digest attribute */
  x509_addsignedattr_messagedigest(cb, digest, digestlen);
  /* return a set */
  x509_set_of(cb, offset);
}
