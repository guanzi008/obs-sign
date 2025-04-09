#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned char byte;

#define HASH_SHA1       0
#define HASH_SHA256     1
#define HASH_SHA512     2

#define PUB_DSA         0
#define PUB_RSA         1
#define PUB_EDDSA       2
#define PUB_ECDSA       3
#define PUB_MLDSA65     4

#define SOCKPROTO_UNPROTECTED	0
#define SOCKPROTO_SSL		1

/* sign.c */
extern int hashalgo;

/* hash.c */
typedef struct {
    u32  h0,h1,h2,h3,h4;
    u32  nblocks;
    byte buf[64];
    int  count;
} SHA1_CONTEXT;

typedef struct {
    u32  h0,h1,h2,h3,h4,h5,h6,h7;
    u32  nblocks;
    byte buf[64];
    int  count;
} SHA256_CONTEXT;

typedef struct {
    u64  h0,h1,h2,h3,h4,h5,h6,h7;
    u64  nblocks;
    byte buf[128];
    int  count;
} SHA512_CONTEXT;

typedef struct MD5Context {
        u32 buf[4];
        u32 bits[2];
        byte in[64];
} MD5_CTX;

typedef union {
  SHA1_CONTEXT sha1;
  SHA256_CONTEXT sha256;
  SHA512_CONTEXT sha512;
} HASH_CONTEXT;

void sha1_init(SHA1_CONTEXT *hd);
void sha1_write(SHA1_CONTEXT *hd, const byte *inbuf, size_t inlen);
void sha1_final(SHA1_CONTEXT *hd);
byte *sha1_read(SHA1_CONTEXT *hd);

void sha256_init(SHA256_CONTEXT *hd);
void sha256_write(SHA256_CONTEXT *hd, const byte *inbuf, size_t inlen);
void sha256_final(SHA256_CONTEXT *hd);
byte *sha256_read(SHA256_CONTEXT *hd);

void sha512_init(SHA512_CONTEXT *hd);
void sha512_write(SHA512_CONTEXT *hd, const byte *inbuf, size_t inlen);
void sha512_final(SHA512_CONTEXT *hd);
byte *sha512_read(SHA512_CONTEXT *hd);

void md5_init(struct MD5Context *ctx);
void md5_write(struct MD5Context *ctx, byte const *buf, u32 len);
void md5_final(byte *digest, struct MD5Context *ctx);

void hash_init(HASH_CONTEXT *c);
void hash_write(HASH_CONTEXT *c, const unsigned char *b, size_t l);
void hash_final(HASH_CONTEXT *c);
unsigned char *hash_read(HASH_CONTEXT *c);
int hash_len(void);

/* base64.c */
void printr64(FILE *f, const byte *str, int len);
char *r64dec1(char *p, u32 *vp, int *eofp);
char *r64dec(char *p, unsigned char **bpp);

/* pgp.c */
void write_armored_pubkey(FILE *fp, const byte *pubkey, int length);
void write_armored_signature(FILE *fp, const byte *signature, int length);
char *get_armored_signature(const byte *signature, int length);
unsigned char *unarmor_pubkey(char *pubkey, int *pktlp);

unsigned char *genv4sigtrail(int clearsign, int pubalgo, int hashalgo, u32 signtime, unsigned char *fp, int *v4sigtraillen);
int fixupsig(unsigned char *sigtrail, unsigned char *v4sigtrail, unsigned char *sigpk, int sigpklen, int tail, int left);
unsigned char *nextpkg(int *tagp, int *pkgl, unsigned char **pp, int *ppl);
unsigned char *findsubpkg(unsigned char *q, int l, int type, int *slp, int fixedsl);
unsigned char *addpkg(unsigned char *to, unsigned char *p, int l, int tag, int newformat);
byte *pkg2sig(byte *pk, int pkl, int *siglp);
byte *findsigissuer(byte *sig, int sigl);
int findsigmpioffset(byte *sig, int sigl);
int findsigpubalgo(byte *sig, int sigl);
int findkeympioffset(byte *key, int keyl);
int findkeypubalgo(byte *key, int keyl);
void calculatekeyfingerprint(byte *key, int keyl, byte *fingerprintp);
int pkg2sigpubalgo(byte *pk, int pkl);
int setmpis(byte *p, int l, int nmpi, byte **mpi, int *mpil, int withcurve);

/* x509.c */
struct x509 {
  byte *buf;
  int len; 
  int alen;	/* allocated length */
};

static inline void x509_init(struct x509 *cb) { memset(cb, 0, sizeof(*cb)); }
static inline void x509_free(struct x509 *cb) { if (cb->buf) free(cb->buf); }
void x509_insert(struct x509 *cb, int offset, const byte *blob, int blobl);
void x509_signature(struct x509 *cb, int pubalgo, byte **mpi, int *mpil);
void x509_tbscert(struct x509 *cb, const char *cn, const char *email, time_t start, time_t end, int pubalgo, byte **mpi, int *mpil);
void x509_finishcert(struct x509 *cb, int pubalgo, struct x509 *sigcb);
void certsizelimit(char *s, int l);

int x509_addpem(struct x509 *cb, char *buf, char *type);
void x509_signedattrs(struct x509 *cb, unsigned char *digest, int digestlen, time_t signtime);
void x509_pkcs7_signed_data(struct x509 *cb, struct x509 *contentinfo, struct x509 *signedattrs, int pubalgo, struct x509 *sigcb, struct x509 *cert, struct x509 *othercerts, int flags);
int x509_cert2pubalgo(struct x509 *cert);

int x509_appx_contentinfo(struct x509 *cb, unsigned char *digest, int digestlen);
void x509_appx_signedattrs(struct x509 *cb, unsigned char *digest, int digestlen, time_t signtime);
int x509_pe_contentinfo(struct x509 *cb, unsigned char *digest, int digestlen);
void x509_pe_signedattrs(struct x509 *cb, unsigned char *digest, int digestlen, time_t signtime);

#define X509_PKCS7_USE_KEYID (1 << 0)
#define X509_PKCS7_NO_CERTS  (1 << 1)

/* zip.c */
struct zip {
  unsigned char *eocd;
  u64 eocd_size;
  u64 size;
  u64 cd_offset;
  u64 cd_size;
  unsigned char *cd;
  u64 num;
  unsigned char *appended;
  u64 appendedsize;
};

void zip_read(struct zip *zip, int fd);
void zip_free(struct zip *zip);
unsigned char *zip_iterentry(struct zip *zip, unsigned char **iterp);
unsigned char *zip_findentry(struct zip *zip, char *fn);

char *zip_entry_name(unsigned char *entry, int *namel);
u64 zip_entry_fhpos(unsigned char *entry);
u32 zip_entry_datetime(unsigned char *entry);

u64 zip_seekdata(struct zip *zip, int fd, unsigned char *entry);
void zip_appendfile(struct zip *zip, char *fn, unsigned char *file, u64 filelen, int comp, u32 datetime);
void zip_write(struct zip *zip, int zipfd, int fd);

/* rpm.c */
struct rpmdata {
  byte rpmlead[96];
  byte rpmsighead[16];
  int rpmsigcnt;
  int rpmsigdlen;
  int rpmsigsize;
  byte *rpmsig;         /* signature data (with room for two new signatures) */

  u32 rpmdataoff;	/* header+payload offset in unsigned rpm */
  u64 hdrin_size;
  byte *hdrin_md5;      /* points into rpmsig */
  int gotsha1;          /* did we see a RPMSIGTAG_SHA1 tag? */
  int gotsha256;        /* did we see a RPMSIGTAG_SHA256 tag? */

  u32 buildtime;
  byte rpmmd5sum[16];   /* md5sum over header+payload */

  byte chksum_leadmd5[16];
  byte chksum_md5[16];
  byte chksum_sha1[20];
  byte chksum_sha256[32];
};

int rpm_insertsig(struct rpmdata *rd, int hdronly, byte *newsig, int newsiglen);
int rpm_read(struct rpmdata *rd, int fd, char *filename, HASH_CONTEXT *ctx, HASH_CONTEXT *hctx, int getbuildtime);
void rpm_write(struct rpmdata *rd, int foutfd, int fd, int chksumfilefd);
void rpm_free(struct rpmdata *rd);
void rpm_writechecksums(struct rpmdata *rd, int chksumfilefd);

/* appimage.c */
int appimage_read(char *filename, HASH_CONTEXT *ctx);
void appimage_write_signature(char *filename, byte *signature, int length);

/* appx.c */
struct appxdata {
  struct x509 cb_content;
  struct x509 cb_signedattrs;
  struct zip zip;
  u32 datetime;
};

int appx_read(struct appxdata *appxdata, int fd, char *filename, HASH_CONTEXT *ctx, time_t t);
void appx_write(struct appxdata *appxdata, int outfd, int fd, struct x509 *cert, int pubalgo, struct x509 *sigcb, struct x509 *othercerts);
void appx_free(struct appxdata *appxdata);

/* sock.c */
void opensocket(void);
void closesocket(void);
int doreq_raw(byte *buf, int inbufl, int bufl);
int doreq_old(const char *user, const char *digest, const char *digestalgo, byte *buf, int bufl);
int doreq(int argc, const char **argv, byte *buf, int bufl, int nret);
int doreq_12(int argc, const char **argv, byte *buf, int bufl, int *outl2p);

/* clearsign.c */
int clearsign(int fd, char *filename, char *outfilename, HASH_CONTEXT *ctx, const char *hname, int isfilter, int force, FILE **foutp);

/* pe.c */

struct pedata {
  struct x509 cb_content;
  struct x509 cb_signedattrs;
  byte hdr[4096];
  u32 headersize;
  u32 c_off;
  u32 csum_off;
  u32 filesize;
  u32 csum;
};

int pe_read(struct pedata *pedata, int fd, char *filename, HASH_CONTEXT *hctx, time_t t);
void pe_write(struct pedata *pedata, int outfd, int fd, struct x509 *cert, int pubalgo, struct x509 *sigcb, struct x509 *othercerts);
void pe_free(struct pedata *pedata);

/* ko.c */
int ko_read(int fd, char *filename, HASH_CONTEXT *ctx);
void ko_write(int outfd, int fd, struct x509 *cb);

/* util.c */
void *doalloc(size_t sz);
void *dorealloc(void *p, size_t sz);
void dodie(const char *msg);
void dodie_errno(const char *msg);
size_t doread_eof(int fd, unsigned char *buf, size_t len);
void doread(int fd, unsigned char *buf, size_t len);
void dowrite(int fd, const unsigned char *buf, size_t len);
void doseek(int fd, u64 pos);
u64 doseek_eof(int fd, u64 pos);
void docopy(int infd, int outfd, u64 len);


/* cpio.c */
#define CPIO_TYPE_TRAILER 0
#define CPIO_TYPE_FILE    1
#define CPIO_TYPE_OTHER   2

byte *cpio_read(int fd, int *typep, int reserve);
u32 cpio_name_append(byte *cpio, char *suf);
u32 cpio_size_set(byte *cpio, u32 size);
u32 cpio_size_get(byte *cpio, u32 *padp);
u32 cpio_headnamesize(byte *cpio);

