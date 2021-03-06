/*
 *               Copyright (C) 2010, BroadOn Communications Corp.
 *
 *  These coded instructions, statements, and computer programs contain
 *  unpublished  proprietary information of BroadOn Communications Corp.,
 *  and  are protected by Federal copyright law. They may not be disclosed
 *  to  third  parties or copied or duplicated in any form, in whole or in
 *  part, without the prior written consent of BroadOn Communications Corp.
 *
 */

#include "vpl_string.h"
#include <estypes.h>

#ifdef _KERNEL_MODULE
#include "core_glue.h"
#endif

USING_ES_NAMESPACE

#include <esi.h>
#include "esi_utils.h"

#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

ES_NAMESPACE_START

/*
 * esi_core.c contains the core APIs that are used by the run-time ES
 * library
 */


typedef struct {
    u32                 initialized;
    IOSCPublicKeyHandle keyHandle;
    IOSCName            name;
    IOSCName            issuerName;
    IOSCHash256         hash;
} CertCache;

static CertCache caCache = { 0 };
static CertCache etCache = { 0 };
static CertCache cpCache = { 0 };


/*
 * Minimalist DER/ASN1 parser with just enough to handle x509 certs
 */

/*
 * Masks for the tag field.  Don't need to worry about tags > 1 byte.
 */
#define DER_UNIVERSAL      0x00
#define DER_APPLICATION    0x40
#define DER_CONTEXT_SPEC   0x80
#define DER_PRIVATE        0xc0
#define DER_CONSTRUCTED    0x20
#define DER_TAG_TYPE       0xe0
#define DER_PRIMITIVE_TAG  0x1f

/*
 * Recognized tag values
 */
#define DER_SEQUENCE       0x10     /* Constructed */
#define DER_SET            0x11     /* Constructed */
#define DER_OBJECT         0x06     /* Primitive */
#define DER_EOC            0x00     /* Primitive */
#define DER_INTEGER        0x02     /* Primitive */
#define DER_BIT_STRING     0x03     /* Primitive */
#define DER_OCTET_STRING   0x04     /* Primitive */
#define DER_NULL           0x05     /* Primitive */
#define DER_UTF8_STR       0x0c     /* Primitive */
#define DER_PRINTABLE_STR  0x13     /* Primitive */
#define DER_UTC_TIME       0x17     /* Primitive */

/*
 * List the OIDs we care about
 */
#define OID_SIG_SHA256_RSA 0x01
#define OID_SIG_SHA1_RSA   0x02
#define OID_PUBKEY_ECC     0x03
#define OID_ECC_SECT233R1  0x04
#define OID_PUBKEY_RSA     0x05
#define OID_PUBKEY_RSA_EXP 0x06
#define OID_NAME_COUNTRY   0x07
#define OID_NAME_STATE     0x08
#define OID_NAME_ORG       0x09
#define OID_NAME_ORGUNIT   0x0a
#define OID_NAME_COMMON    0x0b
#define OID_SUBJ_KEY_ID    0x0c
#define OID_AUTH_KEY_ID    0x0d
#define OID_UNKNOWN        0xff

static u8 oidval_sig_sha256_rsa[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b };
static u8 oidval_sig_sha1_rsa[]   = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05 };
static u8 oidval_pubkey_ecc[]     = { 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01 };
static u8 oidval_ecc_sect233r1[]  = { 0x2b, 0x81, 0x04, 0x00, 0x1b };
static u8 oidval_pubkey_rsa[]     = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01 };
static u8 oidval_pubkey_rsa_exp[] = { 0x00 };
static u8 oidval_name_country[]   = { 0x55, 0x04, 0x06 };
static u8 oidval_name_state[]     = { 0x55, 0x04, 0x08 };
static u8 oidval_name_org[]       = { 0x55, 0x04, 0x0a };
static u8 oidval_name_orgunit[]   = { 0x55, 0x04, 0x0b };
static u8 oidval_name_common[]    = { 0x55, 0x04, 0x03 };
static u8 oidval_subj_key_id[]    = { 0x55, 0x1d, 0x0e };
static u8 oidval_auth_key_id[]    = { 0x55, 0x1d, 0x23 };

/*
 * OID lookup table
 */
static struct {
    u8 oidTag;
    u8 *oidValue;
    u32 oidLen;
} oidTable[] = {
    { OID_SIG_SHA256_RSA, oidval_sig_sha256_rsa, sizeof(oidval_sig_sha256_rsa) },
    { OID_SIG_SHA1_RSA,   oidval_sig_sha1_rsa,   sizeof(oidval_sig_sha1_rsa) },
    { OID_PUBKEY_ECC,     oidval_pubkey_ecc,     sizeof(oidval_pubkey_ecc) },
    { OID_ECC_SECT233R1,  oidval_ecc_sect233r1,  sizeof(oidval_ecc_sect233r1) },
    { OID_PUBKEY_RSA,     oidval_pubkey_rsa,     sizeof(oidval_pubkey_rsa) },
    { OID_PUBKEY_RSA_EXP, oidval_pubkey_rsa_exp, sizeof(oidval_pubkey_rsa_exp) },
    { OID_NAME_COUNTRY,   oidval_name_country,   sizeof(oidval_name_country) },
    { OID_NAME_STATE,     oidval_name_state,     sizeof(oidval_name_state) },
    { OID_NAME_ORG,       oidval_name_org,       sizeof(oidval_name_org) },
    { OID_NAME_ORGUNIT,   oidval_name_orgunit,   sizeof(oidval_name_orgunit) },
    { OID_NAME_COMMON,    oidval_name_common,    sizeof(oidval_name_common) },
    { OID_SUBJ_KEY_ID,    oidval_subj_key_id,    sizeof(oidval_subj_key_id) },
    { OID_AUTH_KEY_ID,    oidval_auth_key_id,    sizeof(oidval_auth_key_id) },
};


/*
 * Parse state structure for x509 certificates
 */
typedef struct {
    u32 state;
    u8 *_current;  // renamed so that it won't clash with kernel macro
    u8 *certStart;
    u32 certLen;
    u8 *certSignStart;
    u32 certSignLen;
    u8 *issuerName;
    u8 *subjectName;
    u32 pubKeyType;
    u32 pubKeySize;
    u32 pubExpSize;
    u32 signatureType;
    u32 signatureSize;
    u8 *pubKey;
    u8 *pubExp;
    u8 *signature;
} parseState;

/*
 * Values for parseState.state
 */
#define XPS_INVALID        0xffffffff
#define XPS_INIT           0
#define XPS_LENGTHS        1
#define XPS_GETISSUERNAME  2
#define XPS_GOTISSUERNAME  3
#define XPS_GETSUBJECTNAME 4
#define XPS_GOTSUBJECTNAME 5
#define XPS_GETPUBKEY      6
#define XPS_GOTPUBKEY      7
#define XPS_GETPUBEXP      8
#define XPS_GOTPUBEXP      9
#define XPS_GETSIGNATURE   10
#define XPS_GOTSIGNATURE   11

#define SIGTYPE_SHA1_RSA    1
#define SIGTYPE_SHA256_RSA  2
#define PUBKEY_ECC          10
#define PUBKEY_RSA          20


/*
 * Convert a (possibly misaligned) big-endian byte representation of an 
 * integer to a 32 bit word
 */
static u32
__bytes2word(u8 *bp)
{
    u32 val;
    int i;

    val = *bp & 0xff;
    for (i = 1; i < sizeof(u32); i++) {
        val <<= 8;
        val |= bp[i] & 0xff;
    }

    return val;
}


static u8
oidLookup(u8 *oid, u32 len)
{
    u8 retVal = OID_UNKNOWN;
    u32 i;

    for (i = 0; i < sizeof(oidTable)/sizeof(oidTable[0]); i++) {
        if (len != oidTable[i].oidLen) {
            continue;
        }
        if (memcmp(oid, oidTable[i].oidValue, len) == 0) {
            return oidTable[i].oidTag;
        }
    }

    return retVal;
}


/*
 * Parses one ASN1 descriptor and returns the new parse buffer position
 */
static u8 *
__parseASN1Desc(u8 *cp, u32 *tagType, u32 *tag, u32 *len)
{
    u32 i;
    u32 wlen;

    *tagType = *cp & DER_TAG_TYPE;
    *tag = *cp++ & DER_PRIMITIVE_TAG;

    /*
     * After the tag is the length indicator, which is either one
     * byte or multiple bytes, indicated by the high bit of the first
     * byte.
     */
    if ((*cp & 0x80) == 0) {
        *len = *cp++ & 0x7f;    /* single byte length up to 127 bytes */
    } else {
        /*
         * If the high bit was set, then the low 7 bits give the number
         * of following bytes that specify the length in big-endian form.
         */
        i = *cp++ & 0x7f;
        wlen = 0;
        while (i-- > 0) {
            wlen = (wlen << 8) | (*cp++ & 0xff);
        }
        *len = wlen;
    }

    return cp;
}


/*
 * Interpret the contents of the descriptor
 */
static u8 *
__evalASN1Desc(parseState *ps, u32 tagType, u32 tag, u32 descLen)
{
    u8 oidTag;

    if (!(tagType & DER_CONSTRUCTED)) {
        if (tag == DER_OBJECT) {
            oidTag = oidLookup(ps->_current, descLen);
            if (oidTag == OID_NAME_COMMON) {
                if (ps->state < XPS_GETISSUERNAME) {
                    ps->state = XPS_GETISSUERNAME;
                } else if (ps->state < XPS_GETSUBJECTNAME) {
                    ps->state = XPS_GETSUBJECTNAME;
                }
            } else if (oidTag == OID_SIG_SHA256_RSA) {
                ps->signatureType = SIGTYPE_SHA256_RSA;
                if (ps->_current >= ps->certSignStart + ps->certSignLen) {
                    ps->state = XPS_GETSIGNATURE;
                }
            } else if (oidTag == OID_SIG_SHA1_RSA) {
                ps->signatureType = SIGTYPE_SHA1_RSA;
                if (ps->_current >= ps->certSignStart + ps->certSignLen) {
                    ps->state = XPS_GETSIGNATURE;
                }
            } else if (oidTag == OID_PUBKEY_ECC) {
                ps->pubKeyType = PUBKEY_ECC;
                ps->state = XPS_GETPUBKEY;
            } else if (oidTag == OID_PUBKEY_RSA) {
                ps->pubKeyType = PUBKEY_RSA;
                ps->state = XPS_GETPUBKEY;
            }
        } else if (tag == DER_BIT_STRING || tag == DER_OCTET_STRING) {
            if (ps->state == XPS_GETPUBKEY) {
                if (ps->pubKeyType == PUBKEY_ECC) {
                    ps->pubKeySize = descLen - 2;
                    ps->pubKey = &ps->_current[2];
                } else if (ps->pubKeyType == PUBKEY_RSA) {
                    u32 innerTagType;
                    u32 innerTag;
                    u32 innerDescLen;
                    u8 *endp;

                    /*
                     * This is actually a constructed sequence, but it gets labelled as a
                     * primitive bitstring for some reason.
                     */
                    endp = ps->_current + descLen;
                    ps->_current++;  // skip extra zero byte
                    while (ps->_current < endp) {
                        ps->_current = __parseASN1Desc(ps->_current, &innerTagType, &innerTag, &innerDescLen);
                        ps->_current = __evalASN1Desc(ps, innerTagType, innerTag, innerDescLen);
                    }

                    /*
                     * The inner loop consumes the size of this descriptor, so don't double count the length
                     */
                    descLen = 0;
                }
                ps->state = XPS_GOTPUBKEY;
            } else if (ps->state == XPS_GETSIGNATURE) {
                /* discard extra byte that x509 inserts for unknown reasons */
                ps->signatureSize = descLen-1;
                ps->signature = &ps->_current[1];
                ps->state = XPS_GOTSIGNATURE;
            }
        } else if (tag == DER_INTEGER) {
            if (ps->pubKeyType == PUBKEY_RSA) {
                if (ps->state == XPS_GETPUBKEY) {
                    ps->pubKeySize = descLen - 1;
                    ps->pubKey = &ps->_current[1];
                    ps->state = XPS_GETPUBEXP;
                } else if (ps->state == XPS_GETPUBEXP) {
                    ps->pubExpSize = descLen;
                    ps->pubExp = ps->_current;
                    ps->state = XPS_GOTPUBEXP;
                }
            }
        } else if (tag == DER_UTC_TIME) {
        } else if (tag == DER_PRINTABLE_STR || tag == DER_UTF8_STR) {
            if (ps->state == XPS_GETISSUERNAME) {
                static u8 __issuerName[128];
                /*
                 * Can't point into an x509 cert for string values, since
                 * they are not null terminated.  This is ugly, since it
                 * is not consistent with the usage model for iGware certs
                 * and adds to the concurrency problems.  The higher level
                 * code ends up returning a pointer to this static buffer
                 * which may get clobbered by another cert parsing operation.
                 * It is up to the caller to copy the string to a persistent
                 * buffer.
                 */
                //ps->issuerName = ps->_current; /* won't work ... */
                memset(__issuerName, 0, sizeof(__issuerName));
                memcpy(__issuerName, ps->_current, MIN(descLen, sizeof(__issuerName) - 1));
                ps->issuerName = __issuerName;
                ps->state = XPS_GOTISSUERNAME;
            } else if (ps->state == XPS_GETSUBJECTNAME) {
                static u8 __subjectName[128];
                /*
                 * Can't point into an x509 cert for string values, since
                 * they are not null terminated. Same comment as above.
                 */
                //ps->subjectName = ps->_current; /* won't work ... */
                memset(__subjectName, 0, sizeof(__subjectName));
                memcpy(__subjectName, ps->_current, MIN(descLen, sizeof(__subjectName) - 1));
                ps->subjectName = __subjectName;
                ps->state = XPS_GOTSUBJECTNAME;
            }
        }
        ps->_current += descLen;
    } 

    return ps->_current;
}


/*
 * Get the overall length and sign length for an x509 cert
 */
static u32
__getX509Lengths(parseState *ps)
{
    u32 len;
    u32 tag;
    u32 tagType;

    ps->_current = __parseASN1Desc(ps->_current, &tagType, &tag, &len);

    if (tagType != DER_CONSTRUCTED || tag != DER_SEQUENCE) {
        esLog(ES_DEBUG_ERROR, "x509: tag %x, type %x does not look like cert\n", tag, tagType);
        goto end;
    }

    /*
     * Include the size of the tag portion
     */
    ps->certLen = len + (ps->_current - ps->certStart);

    /*
     * The next sequence is the portion that is signed
     */
    ps->certSignStart = ps->_current;
    ps->_current = __parseASN1Desc(ps->_current, &tagType, &tag, &len);

    if (tagType != DER_CONSTRUCTED || tag != DER_SEQUENCE) {
        esLog(ES_DEBUG_ERROR, "x509: tag %x, type %x does not look like cert\n", tag, tagType);
        ps->certLen = 0;
        ps->state = XPS_INVALID;
        goto end;
    }

    /*
     * Include the size of the tag portion in the signed area
     */
    ps->certSignLen = len + (ps->_current - ps->certSignStart);
    ps->state = XPS_LENGTHS;

end:
    return ps->certLen;
}


/*
 * Main parse routine for an X509 cert
 */
static u32
__parseX509Cert(const void *cert, parseState *ps)
{
    IOSCError rv = IOSC_ERROR_OK;
    u32 len;
    u32 tag;
    u32 tagType;
    u8 *certEndP;

    memset((void *) ps, 0, sizeof(parseState));
    ps->certStart = (u8 *) cert;
    ps->_current = (u8 *) cert;

    len =  __getX509Lengths(ps);
    if (len == 0) {
        rv = IOSC_ERROR_INVALID;
        goto end;
    }

    certEndP = ps->certStart + ps->certLen;

    while (ps->_current < certEndP) {
        ps->_current = __parseASN1Desc(ps->_current, &tagType, &tag, &len);
        ps->_current = __evalASN1Desc(ps, tagType, tag, len);
    }

end:
    return rv;
}


/*
 * Return the overall length of an X509 certificate
 */
static u32
__getX509CertSize(const void *cert)
{
    parseState ps;
    u32 certLen = 0;

    memset((void *) &ps, 0, sizeof(parseState));
    ps.certStart = (u8 *) cert;
    ps._current = (u8 *) cert;

    certLen =  __getX509Lengths(&ps);

    return certLen;
}


/*
 * Extract name fields and public keys from an x509 certificate
 */
static ESError
__getX509CertFields(const void *cert, u8 **issuerName, u8 **subjectName,
                u8 **pubKey, u32 *pubKeySize, u32 *exponent)
{
    ESError rv = ES_ERR_OK;
    parseState ps;

    rv = __parseX509Cert(cert, &ps);
    if (rv != ES_ERR_OK) {
        goto end;
    }

    if (issuerName != NULL) *issuerName = ps.issuerName;
    if (subjectName != NULL) *subjectName = ps.subjectName;
    if (pubKey != NULL) *pubKey = ps.pubKey;
    if (pubKeySize != NULL) *pubKeySize = ps.pubKeySize;
    if (exponent != NULL) *exponent = (ps.pubExp == NULL) ? 0 : __bytes2word(ps.pubExp);

end:
    return rv;
}


static ESError
__getCertSize(const void *cert, u32 *certSize)
{
    ESError rv = ES_ERR_OK;
    IOSCCertSigType sigType;
    IOSCCertPubKeyType keyType;

    if (cert == NULL || certSize == NULL) {
        esLog(ES_DEBUG_ERROR, "Invalid arguments\n");
        rv = ES_ERR_INVALID;
        goto end;
    }

    sigType = (IOSCCertSigType) ntohl(((IOSCSigRsa4096 *) cert)->sigType);

    switch (sigType) {
        case IOSC_SIG_RSA4096:
        case IOSC_SIG_RSA4096_H256:
            *certSize = sizeof(IOSCRsa4096RsaCert);
            break;

        case IOSC_SIG_RSA2048:
        case IOSC_SIG_RSA2048_H256:
                keyType = (IOSCCertPubKeyType) ntohl(((IOSCRsa2048RsaCert *) cert)->head.pubKeyType);

            switch (keyType) {
                case IOSC_PUBKEY_RSA2048:
                    *certSize = sizeof(IOSCRsa2048RsaCert);
                    break;

                case IOSC_PUBKEY_ECC:
                    *certSize = sizeof(IOSCRsa2048EccCert);
                    break;

                default:
                    esLog(ES_DEBUG_ERROR, "Invalid key type, %u\n", keyType);
                    rv = ES_ERR_INCORRECT_PUBKEY_TYPE;
                    goto end;
            }
            break;

        case IOSC_SIG_ECC:
        case IOSC_SIG_ECC_H256:
            *certSize = sizeof(IOSCEccEccCert);
            break;

        default:
            if ((*certSize = __getX509CertSize(cert)) != 0) {
                goto end;
            }
            esLog(ES_DEBUG_ERROR, "Invalid signature type, %x\n", sigType);
            rv = ES_ERR_INCORRECT_SIG_TYPE;
            goto end;
    }

end:
    return rv;
}


static ESError
__getCertFields(const void *cert, u8 **issuerName, u8 **subjectName,
                u8 **pubKey, u32 *pubKeySize, u32 *exponent)
{
    ESError rv = ES_ERR_OK;
    u8 *ptr;
    u8 *pIssuer, *pCert;
    u8 *pPubKey, *pExp;
    u32 keySize;
    IOSCCertSigType sigType;
    IOSCCertPubKeyType keyType;

    ptr = (u8 *) cert;
    sigType = (IOSCCertSigType) ntohl(((IOSCSigRsa4096 *) ptr)->sigType);

    switch (sigType) {
        case IOSC_SIG_RSA4096:
        case IOSC_SIG_RSA4096_H256:
            pIssuer = (u8 *) ((IOSCSigRsa4096 *)ptr)->issuer;
            keyType = (IOSCCertPubKeyType) ntohl(((IOSCRsa4096RsaCert *)ptr)->head.pubKeyType);

            switch (keyType) {
#if 0
                /*
                 * It is not necessary to handle the iGware root certs (either dev or prod):
                 * the appropriate root public key must be baked into the client code, since any
                 * attacker with access to OpenSSL libraries and tools can create a self-signed
                 * certificate with the appropriate subject name and format.  The only safe way
                 * to handle this case is to ignore the key in the certificate and use the baked
                 * in key to verify the cert, but then what's the point of the cert?
                 */
                case IOSC_PUBKEY_RSA4096:
                    pCert =  (u8 *) ((IOSCRootCert*)ptr)->head.name.serverId;
                    pPubKey = (u8 *) ((IOSCRootCert *)ptr)->pubKey;
                    pExp = (u8 *) ((IOSCRootCert *)ptr)->exponent;
                    keySize = sizeof(IOSCRsaPublicKey4096);
                    break;
#endif

                case IOSC_PUBKEY_RSA2048:
                    pCert =  (u8 *) ((IOSCRsa4096RsaCert*)ptr)->head.name.serverId;
                    pPubKey = (u8 *) ((IOSCRsa4096RsaCert *)ptr)->pubKey;
                    pExp = (u8 *) ((IOSCRsa4096RsaCert *)ptr)->exponent;
                    keySize = sizeof(IOSCRsaPublicKey2048);
                    break;

                default:
                    esLog(ES_DEBUG_ERROR, "Invalid key type, %u\n", keyType);
                    rv = ES_ERR_INCORRECT_PUBKEY_TYPE;
                    goto end;
            }
            break;

        case IOSC_SIG_RSA2048:
        case IOSC_SIG_RSA2048_H256:
            pIssuer = (u8 *) ((IOSCSigRsa2048 *)ptr)->issuer;
            keyType = (IOSCCertPubKeyType) ntohl(((IOSCRsa2048RsaCert *)ptr)->head.pubKeyType);

            switch (keyType) {
                case IOSC_PUBKEY_RSA2048:
                    pCert =  (u8 *) ((IOSCRsa2048RsaCert*)ptr)->head.name.serverId;
                    pPubKey = (u8 *) ((IOSCRsa2048RsaCert *)ptr)->pubKey;
                    pExp = (u8 *) ((IOSCRsa2048RsaCert *)ptr)->exponent;
                    keySize = sizeof(IOSCRsaPublicKey2048);
                    break;

                case IOSC_PUBKEY_ECC:
                    pCert =  (u8 *) ((IOSCRsa2048EccCert*)ptr)->head.name.serverId;
                    pPubKey = (u8 *) ((IOSCRsa2048EccCert *)ptr)->pubKey;
                    pExp = NULL;
                    keySize = sizeof(IOSCEccPublicKey);
                    break;

                default:
                    esLog(ES_DEBUG_ERROR, "Invalid key type, %u\n", keyType);
                    rv = ES_ERR_INCORRECT_PUBKEY_TYPE;
                    goto end;
            }
            break;

        case IOSC_SIG_ECC:
        case IOSC_SIG_ECC_H256:
            pIssuer = (u8 *) ((IOSCSigEcc *)ptr)->issuer;
            pCert = (u8 *) ((IOSCEccEccCert *)ptr)->head.name.deviceId;
            pPubKey = (u8 *) ((IOSCEccEccCert *)ptr)->pubKey;
            pExp = NULL;
            keySize = sizeof(IOSCEccPublicKey);
            break;

        default:
            /*
             * Check for an x509 certificate
             */
            if (__getX509CertSize(cert) != 0) {
                rv = __getX509CertFields(cert, issuerName, subjectName, pubKey, pubKeySize, exponent);
            } else {
                esLog(ES_DEBUG_ERROR, "Invalid signature type, %x\n", sigType);
                rv = ES_ERR_INCORRECT_SIG_TYPE;
            }
            goto end;
    }

    if (issuerName != NULL) *issuerName = pIssuer;
    if (subjectName != NULL) *subjectName = pCert;
    if (pubKey != NULL) *pubKey = pPubKey;
    if (pubKeySize != NULL) *pubKeySize = keySize;
    if (exponent != NULL) {
        *exponent = (pExp == NULL) ? 0 : __bytes2word(pExp);
    }

end:
    return rv;
}


static ESError
__findCert(const char *id, u32 completeId, const void *certs[], u32 nCerts,
           void **cert, u32 *certSize, char **certIssuer)
{
    ESError rv = ES_ERR_ISSUER_NOT_FOUND;
    u32 i;
    u8 *pIssuer = NULL;
    u8 *pCert = NULL;
    IOSCName name;

    for (i = 0; i < nCerts; i++) {
        rv = __getCertFields(certs[i], &pIssuer, &pCert, NULL, NULL, NULL);
        if (rv != ES_ERR_OK) {
            goto end;
        }


        if (completeId) {
            VPL_snprintf((char *) name, sizeof(IOSCName), "%s%s%s", pIssuer, "-", pCert);
        } else {
            VPL_snprintf((char *) name, sizeof(IOSCName), "%s", pCert);
        }

        if (!strncmp(id, (const char *) name, sizeof(IOSCName))) {
            *cert = (void *)certs[i];
            *certIssuer = (char *)pIssuer;
            __getCertSize(certs[i], certSize);
            rv = ES_ERR_OK; /* redundant, but defensive */
            goto end;
        }
    }

    /*
     * No match!
     */
    rv = ES_ERR_ISSUER_NOT_FOUND;

end:
    if (rv != ES_ERR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to find cert, %s, rv=%d\n", id, rv);
        *cert = NULL;
        *certSize = 0;
        *certIssuer = NULL;
    }

    return rv;
}


static ESError
__clearCertCacheEntry(CertCache *certCache)
{
    ESError rv = ES_ERR_OK;

    if (certCache->initialized && certCache->keyHandle != 0) {
        /*
         * If the delete fails, there is something wrong.
         * Clearing the entry by force seems like a better solution
         * than printing an error and bailing.
         */
        (void) IOSC_DeleteObject(certCache->keyHandle);
    }
    certCache->initialized = 0;
    certCache->keyHandle = 0;

    return rv;
}


ESError
ESI_ClearCertCache()
{
    ESError rv;

    rv = __clearCertCacheEntry(&caCache);
    rv = __clearCertCacheEntry(&etCache);
    rv = __clearCertCacheEntry(&cpCache);
    
    return rv;
} 


static ESError
__certHash(void *cert, u32 certSize, IOSCHash256 hash)
{
    ESError rv = ES_ERR_OK;
    IOSCHashContext hashCtx;

    if ((rv = IOSC_GenerateHash(hashCtx, NULL, 0, IOSC_SHA256_INIT, NULL)) != IOSC_ERROR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to generate hash - init, rv=%d\n", rv);
        rv = ESI_Translate_IOSC_Error(rv);
        goto end;
    }

    if ((rv = IOSC_GenerateHash(hashCtx, (u8 *) cert, certSize, IOSC_SHA256_FINAL, hash)) != IOSC_ERROR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to generate hash - final, rv=%d\n", rv);
        rv = ESI_Translate_IOSC_Error(rv);
        goto end;
    }

end:
    return rv;
}


/*
 * This verification is only for certs that are potentially cacheable.
 */
static ESError
__verifyCert(CertCache *certCache, IOSCPublicKeyHandle hIssuerPubKey,
             void *cert, IOSCPublicKeyHandle *hPubKey)
{
    ESError rv = ES_ERR_OK;
    IOSCCertSigType sigType;
    IOSCCertPubKeyType keyType;
    char *pName;
    char *pIssuerName;
    u32 certSize, keyHandleCreated = 0;
    IOSCHash256 hash;

    sigType = (IOSCCertSigType) ntohl(((IOSCSigRsa4096 *) cert)->sigType);

    switch (sigType) {
        case IOSC_SIG_RSA4096:
        case IOSC_SIG_RSA4096_H256:
            pName = (char *) ((IOSCRsa4096RsaCert *)cert)->head.name.serverId;
            pIssuerName = (char *) ((IOSCRsa4096RsaCert *)cert)->sig.issuer;
            certSize = sizeof(IOSCRsa4096RsaCert);
            break;

        case IOSC_SIG_RSA2048:
        case IOSC_SIG_RSA2048_H256:
            keyType = (IOSCCertPubKeyType) ntohl(((IOSCRsa2048RsaCert *)cert)->head.pubKeyType);

            switch (keyType) {
                case IOSC_PUBKEY_RSA2048:
                    pName = (char *) ((IOSCRsa2048RsaCert *)cert)->head.name.serverId;
                    pIssuerName = (char *) ((IOSCRsa2048RsaCert *)cert)->sig.issuer;
                    certSize = sizeof(IOSCRsa2048RsaCert);
                    break;

                default:
                    esLog(ES_DEBUG_ERROR, "Invalid key type, %u\n", keyType);
                    rv = ES_ERR_INCORRECT_PUBKEY_TYPE;
                    goto end;
            }
            break;

        default:
            esLog(ES_DEBUG_ERROR, "Invalid signature type, %x\n", sigType);
            rv = ES_ERR_INCORRECT_SIG_TYPE;
            goto end;
    }

    /*
     * If the cert is in the cache, compare hash with cached cert.  If
     * not, do RSA check, and add cert to the cache.
     *
     * The cache retains the most recent cert of each type that has been
     * verified (CP, XS or CA).  If a different one of the particular type
     * is verified, the new cert replaces the previous cached entry.
     */
    if (certCache->initialized && strncmp((char *) certCache->name, pName, sizeof(IOSCName)) == 0 &&
        strncmp((char *) certCache->issuerName, pIssuerName, sizeof(IOSCName)) == 0) {
        if ((rv = __certHash(cert, certSize, hash)) != ES_ERR_OK) {
            goto end;
        }

        if (memcmp(hash, certCache->hash, sizeof(IOSCHash256)) != 0) {
            esLog(ES_DEBUG_ERROR, "Failed to match cached cert\n");
            rv = ES_ERR_VERIFICATION;
            goto end;
        }

        // Retrieve key handle from the cert cache
        *hPubKey = certCache->keyHandle;
    } else {
        // Create the key handle and import the certificate
        if ((rv = IOSC_CreateObject(hPubKey, IOSC_PUBLICKEY_TYPE, IOSC_RSA2048_SUBTYPE)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to create pubKey handle, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }
        keyHandleCreated = 1;

        if ((rv = IOSC_ImportCertificate((u8 *) cert, hIssuerPubKey, *hPubKey)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to verify cert, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }

        // Set the cert cache
        if ((rv = __certHash(cert, certSize, hash)) != ES_ERR_OK) {
            goto end;
        }

        if (certCache->initialized && certCache->keyHandle != 0) {
            if ((rv = IOSC_DeleteObject(certCache->keyHandle)) != IOSC_ERROR_OK) {
                esLog(ES_DEBUG_ERROR, "Failed to delete cached key handle, rv=%d\n", rv);
                rv = ESI_Translate_IOSC_Error(rv);
                goto end;
            }
        }

        certCache->keyHandle = *hPubKey;
        memcpy(certCache->name, pName, sizeof(IOSCName));
        memcpy(certCache->issuerName, pIssuerName, sizeof(IOSCName));
        memcpy(certCache->hash, hash, sizeof(IOSCHash256));

        certCache->initialized = 1;
    }

end:
    if (rv != ES_ERR_OK && keyHandleCreated) {
        (void) IOSC_DeleteObject(*hPubKey);
        *hPubKey = 0;
    }

    return rv;
}


static ESError
__personalizeTicket(ESTicket *ticket, u32 reverse)
{
    ESError rv = ES_ERR_OK;
    ESTicketId tid;
    ESDeviceId devId;
    IOSCPublicKeyHandle hPubKey = 0;
    IOSCSecretKeyHandle hSharedKey = 0;

    // Declared static here to enforce alignment constraint
    static u8 keyBuf[SIZE_AES_ALIGN(sizeof(IOSCAesKey))] ATTR_AES_ALIGN;
    static u8 ivBuf[SIZE_AES_ALIGN(sizeof(IOSCAesIv))] ATTR_AES_ALIGN;

    // Check device ID
    if ((rv = IOSC_GetData(IOSC_DEV_ID_HANDLE, &devId)) != IOSC_ERROR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to get device ID, rv=%d\n", rv);
        rv = ESI_Translate_IOSC_Error(rv);
        goto end;
    }

    if (ntohl(ticket->deviceId) != devId) {
        esLog(ES_DEBUG_ERROR, "Device ID mismatch, 0x%x:0x%x\n", devId, ntohl(ticket->deviceId));
        rv = ES_ERR_DEVICE_ID_MISMATCH;
        goto end;
    }

    // Compute shared key
    if ((rv = IOSC_CreateObject(&hPubKey, IOSC_PUBLICKEY_TYPE, IOSC_ECC233_SUBTYPE)) != IOSC_ERROR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to create pubkey object, rv=%d\n", rv);
        rv = ESI_Translate_IOSC_Error(rv);
        goto end;
    }

    if ((rv = IOSC_ImportPublicKey(ticket->serverPubKey, 0, hPubKey)) != IOSC_ERROR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to import pubkey, rv=%d\n", rv);
        rv = ESI_Translate_IOSC_Error(rv);
        goto end;
    }

    if ((rv = IOSC_CreateObject(&hSharedKey, IOSC_SECRETKEY_TYPE, IOSC_ENC_SUBTYPE)) != IOSC_ERROR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to create hmackey object, rv=%d\n", rv);
        rv = ESI_Translate_IOSC_Error(rv);
        goto end;
    }

    if ((rv = IOSC_ComputeSharedKey(IOSC_DEV_SIGNING_KEY_HANDLE, hPubKey, hSharedKey)) != IOSC_ERROR_OK) {
        esLog(ES_DEBUG_ERROR, "Failed to compute shared key, rv=%d\n", rv);
        rv = ESI_Translate_IOSC_Error(rv);
        goto end;
    }

    // Encrypt or decrypt title key with shared key
    tid = ticket->ticketId;
    memset(ivBuf, 0, sizeof(IOSCAesIv));
    memcpy(ivBuf, (u8 *) &tid, sizeof(ESTicketId));

    memcpy(keyBuf, ticket->titleKey, sizeof(IOSCAesKey));
    if (reverse) {
        if ((rv = IOSC_Decrypt(hSharedKey, ivBuf, keyBuf, sizeof(IOSCAesKey), keyBuf)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to decrypt title key, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }
    } else {
        if ((rv = IOSC_Encrypt(hSharedKey, ivBuf, keyBuf, sizeof(IOSCAesKey), keyBuf)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to encrypt title key, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }
    }

    memcpy(ticket->titleKey, keyBuf, sizeof(IOSCAesKey));

end:
    if (hPubKey) IOSC_DeleteObject(hPubKey);
    if (hSharedKey) IOSC_DeleteObject(hSharedKey);

    return rv;
}


ESError
ESI_Translate_IOSC_Error(IOSCError err)
{
    ESError rv = ES_ERR_OK;

    switch (err) {
    case IOSC_ERROR_OK:
        rv = ES_ERR_OK;
        break;

    case IOSC_ERROR_FAIL_CHECKVALUE:
        rv = ES_ERR_VERIFICATION;
        break;

    case IOSC_ERROR_INVALID_SIZE:
    case IOSC_ERROR_INVALID_ADDR:
    case IOSC_ERROR_INVALID_ALIGN:
        rv = ES_ERR_ALIGNMENT;
        break;

    default:
        rv = ES_ERR_CRYPTO;
        break;
    }

    return rv;
}


ESError
ESI_VerifyContainer(ESContainerType containerType,
                    const void *data, u32 dataSize,
                    const void *sig, const char *issuer,
                    const void *certs[], u32 nCerts,
                    IOSCPublicKeyHandle hPubKey,
                    IOSCPublicKeyHandle *outIssuerPubKeyHandle)
{
    ESError rv = ES_ERR_OK;
    char *caId, *rootId;
    IOSCRsa4096RsaCert *caCert;
    IOSCRsa2048RsaCert *issuerCert;
    u32 issuerCertSize, caCertSize, hSize, dSize;
    u32 hashType = IOSC_HASH_SHA256, hashSize = sizeof(IOSCHash256);
    IOSCPublicKeyHandle hCaPubKey = 0, hIssuerPubKey = 0;
    IOSCCertSigType sigType;
    IOSCHashContext hashCtx;
    IOSCHash256 hash;
    CertCache *certCache = NULL;

    if (sig == NULL || issuer == NULL || certs == NULL || nCerts == 0) {
        esLog(ES_DEBUG_ERROR, "Invalid arguments\n");
        rv = ES_ERR_INVALID;
        goto end;
    }

    // Search issuer cert
    if ((rv = __findCert(issuer, 1, certs, nCerts, (void **) &issuerCert, &issuerCertSize, &caId)) != ES_ERR_OK) {
        goto end;
    }

    // Search CA cert
    if ((rv = __findCert(caId, 1, certs, nCerts, (void **) &caCert, &caCertSize, &rootId)) != ES_ERR_OK) {
        goto end;
    }

    // Check issuer cert name
    switch (containerType) {
        case ES_CONTAINER_TMD:
            certCache = &cpCache;

            if ( (strncmp((const char *) issuerCert->head.name.serverId, ES_CP_PREFIX, 2) != 0) &&
            	 (strncmp((const char *) issuerCert->head.name.serverId, ES_SP_PREFIX, 2) != 0) ) {
                esLog(ES_DEBUG_ERROR, "Issuer prefix incorrect %s\n",
                      issuerCert->head.name.serverId);
                rv = ES_ERR_INVALID;
                goto end;
            }
            break;

        case ES_CONTAINER_TKT:
            certCache = &etCache;

            if (strncmp((const char *) issuerCert->head.name.serverId, ES_XS_PREFIX, 2) != 0) {
                esLog(ES_DEBUG_ERROR, "Issuer prefix incorrect %s\n",
                      issuerCert->head.name.serverId);
                rv = ES_ERR_INVALID;
                goto end;
            }
            break;

        case ES_CONTAINER_DEV:
            if (strncmp((const char *) ((IOSCRsa2048EccCert *)issuerCert)->head.name.serverId, ES_MS_PREFIX, 2) != 0) {
                esLog(ES_DEBUG_ERROR, "Issuer prefix incorrect %s\n",
                      issuerCert->head.name.serverId);
                rv = ES_ERR_INVALID;
                goto end;
            }
            break;

        default:
            esLog(ES_DEBUG_ERROR, "Invalid container type %u\n", containerType);
            rv = ES_ERR_INVALID;
            goto end;
    }

    // Verify CA cert
    if ((rv = __verifyCert(&caCache, IOSC_ROOT_KEY_HANDLE, caCert, &hCaPubKey)) != ES_ERR_OK) {
        goto end;
    }

    // Verify the issuer cert
    if (containerType == ES_CONTAINER_DEV) {
        if ((rv = IOSC_CreateObject(&hIssuerPubKey, IOSC_PUBLICKEY_TYPE, IOSC_ECC233_SUBTYPE)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to create issuer pubKey handle, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }

        if ((rv = IOSC_ImportCertificate((u8 *) issuerCert, hCaPubKey, hIssuerPubKey)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to verify issuer cert, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }
    } else {
        if ((rv = __verifyCert(certCache, hCaPubKey, issuerCert, &hIssuerPubKey)) != ES_ERR_OK) {
            goto end;
        }

        if (outIssuerPubKeyHandle) {
            *outIssuerPubKeyHandle = hIssuerPubKey;
        }
    }

    /*
     * In some cases, the data needs to be streamed and therefore can't
     * be passed in here
     */
    if (data != NULL) {
        /*
         * Verifying a TMD is more complicated since there are multiple
         * levels of hashes.  The code here only allows verifying the
         * header portion of the TMD.  Since no one is using the code
         * here to verify the rest of the TMD, this code is sufficient.
         * If the caller attempts to use this code to verify an entire
         * TMD, a verification error will be returned.
         */
        if (containerType == ES_CONTAINER_DEV) {
            hSize = sizeof(IOSCCertSigType) + sizeof(IOSCEccSig) + sizeof(IOSCEccPublicPad) + sizeof(IOSCSigDummy);
        
            // Allow the test device cert to use SHA1
            sigType = (IOSCCertSigType) ntohl(((IOSCSigEcc *) data)->sigType);
            if (sigType == IOSC_SIG_ECC) {
                hashType = IOSC_HASH_SHA1;
                hashSize = sizeof(IOSCHash);
            }
        } else {
            hSize = sizeof(IOSCCertSigType) + sizeof(IOSCRsaSig2048) + sizeof(IOSCSigDummy);
        }
        dSize = dataSize - hSize;

        if ((rv = IOSC_GenerateHash(hashCtx, NULL, 0, IOSC_HASH_FIRST | hashType, NULL)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to generate hash - init, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }

        if ((rv = IOSC_GenerateHash(hashCtx, ((u8 *) data) + hSize, dSize, IOSC_HASH_LAST | hashType, hash)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to generate hash - final, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }

        if ((rv = IOSC_VerifyPublicKeySign(hash, hashSize, hIssuerPubKey, (u8 *) sig)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "verify publickey sign failed: %d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }
    }

    // Input data is a cert itself, export public key handle
    if (hPubKey) {
        if ((rv = IOSC_ImportCertificate((u8 *) data, hIssuerPubKey, hPubKey)) != IOSC_ERROR_OK) {
            esLog(ES_DEBUG_ERROR, "Failed to import input cert, rv=%d\n", rv);
            rv = ESI_Translate_IOSC_Error(rv);
            goto end;
        }
    }

end:
    if (hIssuerPubKey && (hIssuerPubKey != etCache.keyHandle) && (hIssuerPubKey != cpCache.keyHandle)) {
        (void) IOSC_DeleteObject(hIssuerPubKey);
    }
    if (hCaPubKey && (hCaPubKey != caCache.keyHandle)) {
        (void) IOSC_DeleteObject(hCaPubKey);
    }

    return rv;
}


ESError
ESI_PersonalizeTicket(ESTicket *ticket)
{
    return __personalizeTicket(ticket, 0);
}


ESError
ESI_UnpersonalizeTicket(ESTicket *ticket)
{
    return __personalizeTicket(ticket, 1);
}


ESError
ESI_GetCertSize(const void *cert, u32 *certSize)
{
    return __getCertSize(cert, certSize);
}


ESError
ESI_GetCertNames(const void *cert, u8 **issuerName, u8 **subjectName)
{
    ESError rv = ES_ERR_OK;

    if (cert == NULL) {
        esLog(ES_DEBUG_ERROR, "Invalid arguments\n");
        rv = ES_ERR_INVALID;
        goto end;
    }

    rv =  __getCertFields(cert, issuerName, subjectName, NULL, NULL, NULL);

end:
    return rv;
}


ESError
ESI_GetCertPubKey(const void *cert, u8 **pubKey, u32 *pubKeySize, u32 *exponent)
{
    ESError rv = ES_ERR_OK;

    if (cert == NULL || pubKey == NULL) {
        esLog(ES_DEBUG_ERROR, "Invalid arguments\n");
        rv = ES_ERR_INVALID;
        goto end;
    }

    rv =  __getCertFields(cert, NULL, NULL, pubKey, pubKeySize, exponent);

end:
    return rv;
}


ESError
ESI_FindCert(const char *id, u32 completeId, const void *certs[], u32 nCerts,
             void **cert, u32 *certSize, char **certIssuer)
{
    ESError rv = ES_ERR_OK;

    if (id == NULL || certs == NULL || nCerts == 0 || cert == NULL || certSize == NULL || certIssuer == NULL) {
        esLog(ES_DEBUG_ERROR, "Invalid arguments\n");
        rv = ES_ERR_INVALID;
        goto end;
    }

    rv = __findCert(id, completeId, certs, nCerts, cert, certSize, certIssuer);

end:
    return rv;
}


ESError
ESI_ParseCertList(const void *certList, u32 certListSize,
                  void *outCerts[], u32 *outCertSizes, u32 *outNumCerts)
{
    ESError rv = ES_ERR_OK;
    u8 *ptr = (u8 *) certList;
    u32 max = 0, certSize;

    if (certList == NULL || certListSize == 0 || outNumCerts == NULL) {
        esLog(ES_DEBUG_ERROR, "Invalid arguments\n");
        rv = ES_ERR_INVALID;
        goto end;
    }

    if (outCerts || outCertSizes) {
        max = *outNumCerts;
    }
    *outNumCerts = 0;

    while (ptr < (((u8 *) certList) + certListSize)) {
        if ((rv = __getCertSize(ptr, &certSize)) != ES_ERR_OK) {
            goto end;
        }

        if (outCerts || outCertSizes) {
            if (*outNumCerts == max) {
                esLog(ES_DEBUG_ERROR, "Input cert array too small\n");
                rv = ES_ERR_INVALID;
                goto end;
            }
        }

        if (outCerts) {
            outCerts[*outNumCerts] = ptr;
        }

        if (outCertSizes) {
            outCertSizes[*outNumCerts] = certSize;
        }

        (*outNumCerts)++;
        ptr += certSize;
    }

end:
    return rv;
}


ESError
ESI_MergeCerts(const void *certs1[], u32 nCerts1,
               const void *certs2[], u32 nCerts2,
               void *outCerts[], u32 *outCertSizes, u32 *outNumCerts)
{
    ESError rv = ES_ERR_OK;
    u32 i, j, certSize1, certSize2, found, nCerts = 0;
    const void *cert;

    if (certs1 == NULL || nCerts1 == 0 || certs2 == NULL || nCerts2 == 0 || outCerts == NULL || outNumCerts == NULL || outCertSizes == NULL) {
        esLog(ES_DEBUG_ERROR, "Invalid arguments\n");
        rv = ES_ERR_INVALID;
        goto end;
    }

    for (i = 0; i < (nCerts1 + nCerts2); i++) {
        if (i < nCerts1) {
            cert = certs1[i];
        } else {
            cert = certs2[i - nCerts1];
        }

        if ((rv = __getCertSize(cert, &certSize1)) != ES_ERR_OK) {
            goto end;
        }

        found = 0;
        for (j = 0; j < nCerts; j++) {
            if ((rv = __getCertSize(outCerts[j], &certSize2)) != ES_ERR_OK) {
                goto end;
            }

            if (certSize1 == certSize2 && memcmp(cert, outCerts[j], certSize1) == 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            if (nCerts >= *outNumCerts) {
                esLog(ES_DEBUG_ERROR, "Input cert array too small\n");
                rv = ES_ERR_INVALID;
                goto end;
            }

            outCerts[nCerts] = (void *) cert;
            outCertSizes[nCerts] = certSize1;

            nCerts++;
        }
    }
    *outNumCerts = nCerts;

end:
    return rv;
}


static CertCache *certCache[] = { &caCache, &etCache, &cpCache };

#define ESI_MAX_CHAIN_DEPTH 8
#define ESI_CD_CERT     0
#define ESI_CD_CACHE    1
#define ESI_CD_IGWROOT  2

struct certDepends {
    int entryType;  /* may be the root, a cert cache entry or a real cert */
    void *entryPtr;
};

typedef struct {
    IOSCName fullName;  /* qualified cert name:  "Issuer-Subject" */
    IOSCName issuerName;
} ESI_VerifyCert_big_locals;

ESError
ESI_VerifyCert(const void *cert, const void *verifyChain[], u32 nChainCerts)
{
    ESError rv = ES_ERR_OK;
    u8 *issuer;
    u8 *subject;
    u32 pubKeySize;
    char *issuerId;
    void *issuerCert;
    u32 issuerCertSize;
    struct certDepends depCerts[ESI_MAX_CHAIN_DEPTH];
    IOSCPublicKeyHandle deleteHandles[ESI_MAX_CHAIN_DEPTH];
    u32 numDeleteHandles = 0;
    IOSCPublicKeyHandle verifyHandle = 0;
    IOSCPublicKeyHandle certKeyHandle = 0;
    int level;
    int i;

    ESI_VerifyCert_big_locals *big_locals;
#ifdef __KERNEL__
    big_locals = kmalloc(sizeof(*big_locals), GFP_KERNEL);
    if (!big_locals)
        return ES_ERR_NO_MEMORY;
#else
    ESI_VerifyCert_big_locals _big_locals;
    big_locals = &_big_locals;
#endif

    rv = ESI_GetCertNames(cert, &issuer, &subject);
    if (rv != ES_ERR_OK) {
        goto end;
    }

    depCerts[0].entryType = ESI_CD_CERT;
    depCerts[0].entryPtr = (void *) cert;

    /*
     * Construct dependency chain from the verifyChain
     */
    for (level = 1; level < ESI_MAX_CHAIN_DEPTH; level++) {

        if (strncmp((const char *)issuer, "Root", sizeof(IOSCName)) == 0) {

            depCerts[level].entryType = ESI_CD_IGWROOT;
            depCerts[level].entryPtr = NULL;
            /* Search terminates at the Root */
            break;
        }

        /*
         * Check if the issuer is already in the cert cache.
         */
        for (i = 0; i < sizeof(certCache)/sizeof(certCache[0]); i++) {
            if (certCache[i]->initialized == 0) continue;
            /*
             * The cert cache currently only supports iGware certs, so the dash versus
             * underscore issue does not arise here.
             */
            VPL_snprintf((char *)big_locals->fullName, sizeof(big_locals->fullName), "%s-%s", certCache[i]->issuerName, certCache[i]->name);
            if (strncmp((const char *)big_locals->fullName, (const char *)issuer, sizeof(big_locals->fullName)) != 0) continue;
            /*
             * Found a name match
             */
            depCerts[level].entryType = ESI_CD_CACHE;
            depCerts[level].entryPtr = certCache[i];
            issuer = (u8 *) &certCache[i]->issuerName;
            break;
        }
        if (i < sizeof(certCache)/sizeof(certCache[0])) {
            continue;
        }

        /*
         * Copy the issuer string to a safe place before invoking another certificate parsing operation.
         * In the x509 case, the data may get overwritten.
         */
        memset(big_locals->issuerName, 0, sizeof(big_locals->issuerName));
        strncpy((char *) big_locals->issuerName, (const char *) issuer, sizeof(big_locals->issuerName));

        /*
         * Lastly check the supplied verify chain.
         */
        rv = __findCert((const char *)big_locals->issuerName, 1, verifyChain, nChainCerts, (void **) &issuerCert, &issuerCertSize, &issuerId);
        if (rv == ES_ERR_OK) {
            /*
             * Found the next level issuer on the supplied list
             */
            depCerts[level].entryType = ESI_CD_CERT;
            depCerts[level].entryPtr = issuerCert;
            issuer = (u8 *) issuerId;
            continue;
        }
        rv = ES_ERR_ISSUER_NOT_FOUND;
        goto end;
    }

    /*
     * Check that the path is within the limits and goes all the way to the Root
     */
    if (level == ESI_MAX_CHAIN_DEPTH) {
        esLog(ES_DEBUG_ERROR, "VerifyCert fails: verify chain too long\n");
        rv = ES_ERR_ISSUER_NOT_FOUND;
        goto end;
    } 
    if (depCerts[level].entryType != ESI_CD_IGWROOT) {
        esLog(ES_DEBUG_ERROR, "VerifyCert fails: verify chain incomplete\n");
        rv = ES_ERR_ISSUER_NOT_FOUND;
        goto end;
    }

    /*
     * Do the verifications in the reverse order:  from the top of the tree back down to the leaf
     */
    for (i = level - 1; i >= 0; i--) {
        CertCache *myCertCache;

        switch (depCerts[i + 1].entryType) {
        case ESI_CD_IGWROOT:
            verifyHandle = IOSC_ROOT_KEY_HANDLE;
            break;
        case ESI_CD_CACHE:
            verifyHandle = ((CertCache *) depCerts[i+1].entryPtr)->keyHandle;
            break;
        case ESI_CD_CERT:
            if (certKeyHandle == 0) {
                /* "Can't happen" */
                esLog(ES_DEBUG_ERROR, "VerifyCerts internal error, bad key handle\n");
                rv = ES_ERR_INVALID;
                goto end;
            }
            verifyHandle = certKeyHandle;
            break;
        default:
            rv = ES_ERR_INVALID;
            goto end;
        }

        /*
         * The current entry is at least one down from the Root, so we can
         * check for this as an error.
         */
        if (depCerts[i].entryType == ESI_CD_IGWROOT) {
            rv = ES_ERR_INVALID;
            goto end;
        }

        /*
         * The cache entries are not reverified
         */
        if (depCerts[i].entryType == ESI_CD_CACHE) {
            certKeyHandle = ((CertCache *) depCerts[i].entryPtr)->keyHandle;
            continue;
        }

        rv = __getCertFields(depCerts[i].entryPtr, &issuer, &subject, NULL, &pubKeySize, NULL);
        if (rv != ES_ERR_OK) {
            esLog(ES_DEBUG_ERROR, "GetCertFields failed (rv %d)\n", rv);
            goto end;
        }

        /*
         * If the cert being verified is covered by the cert cache, this logic must use __verifyCert, 
         * so that the entry ends up cached after verification. 
         *
         * If not a cache-eligible cert, then verify "by hand".
         */
        myCertCache = (CertCache *) NULL;
        if (level - i == 1) {
            /*
             * iGware second level CA is cached
             */
            if (strncmp((const char *) subject, "CA", 2) == 0) {
                myCertCache = &caCache;
            } else {
                esLog(ES_DEBUG_ERROR, "VerifyCert: CA cert %s is not one level down from ROOT!\n", subject);
                rv = ES_ERR_INVALID;
                goto end;
            }
        } else if (level - i == 2) {
            /*
             * Currently the cert cache covers only CP and XS at the second level
             */
            if (strncmp((const char *) subject, ES_CP_PREFIX, 2) == 0) {
                myCertCache = &cpCache;
            } else if (strncmp((const char *) subject, ES_XS_PREFIX, 2) == 0) {
                myCertCache = &etCache;
            } else if (strncmp((const char *) subject, ES_MS_PREFIX, 2) == 0) {
                myCertCache = NULL;
            } else if (strncmp((const char *) subject, ES_SP_PREFIX, 2) == 0) {
                myCertCache = NULL;
#if 0
            } else {
                esLog(ES_DEBUG_ERROR, "VerifyCert: %s cert is at the wrong level %d!\n", subject, level - i);
                rv = ES_ERR_INVALID;
                goto end;
#endif
            }
        }

        if (myCertCache != NULL) {
            rv = __verifyCert(myCertCache, verifyHandle, depCerts[i].entryPtr, &certKeyHandle);
            if (rv != ES_ERR_OK) {
                goto end;
            }
        } else {
            IOSCObjectSubType keySubType;
            if (pubKeySize == sizeof(IOSCRsaPublicKey2048)) keySubType = IOSC_RSA2048_SUBTYPE; 
            else if (pubKeySize == sizeof(IOSCEccPublicKey)) keySubType = IOSC_ECC233_SUBTYPE;
            else {
                esLog(ES_DEBUG_ERROR, "Invalid public key size %d for cert %d\n", pubKeySize, i);
                rv = ES_ERR_INVALID;
                goto end;
            }

            if ((rv = IOSC_CreateObject(&certKeyHandle, IOSC_PUBLICKEY_TYPE, keySubType)) != IOSC_ERROR_OK) {
                esLog(ES_DEBUG_ERROR, "Failed to create pubKey handle, rv=%d\n", rv);
                rv = ESI_Translate_IOSC_Error(rv);
                goto end;
            }
            /* Can't leak temporary key handles */
            deleteHandles[numDeleteHandles++] = certKeyHandle;

            if ((rv = IOSC_ImportCertificate((u8 *) depCerts[i].entryPtr, verifyHandle, certKeyHandle)) != IOSC_ERROR_OK) {
                esLog(ES_DEBUG_ERROR, "Failed to verify cert, rv=%d\n", rv);
                rv = ESI_Translate_IOSC_Error(rv);
                goto end;
            }
        }
    }

    rv = ES_ERR_OK;
    goto end;

end:
    for (i = 0; i < numDeleteHandles; i++) {
        /*
         * Be careful not to overwrite the value of "rv"
         */
        (void) IOSC_DeleteObject(deleteHandles[i]);
    }

#ifdef __KERNEL__
    kfree(big_locals);
#endif

    return rv;
}

ES_NAMESPACE_END
