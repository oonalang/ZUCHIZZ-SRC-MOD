
#ifndef HEADER_CT_H
# define HEADER_CT_H

# include <openssl/opensslconf.h>

# ifndef OPENSSL_NO_CT
# include <openssl/ossl_typ.h>
# include <openssl/safestack.h>
# include <openssl/x509.h>
# ifdef  __cplusplus
extern "C" {
# endif

# define SCT_MIN_RSA_BITS 2048

# define CT_V1_HASHLEN SHA256_DIGEST_LENGTH

typedef enum {
    CT_LOG_ENTRY_TYPE_NOT_SET = -1,
    CT_LOG_ENTRY_TYPE_X509 = 0,
    CT_LOG_ENTRY_TYPE_PRECERT = 1
} ct_log_entry_type_t;

typedef enum {
    SCT_VERSION_NOT_SET = -1,
    SCT_VERSION_V1 = 0
} sct_version_t;

typedef enum {
    SCT_SOURCE_UNKNOWN,
    SCT_SOURCE_TLS_EXTENSION,
    SCT_SOURCE_X509V3_EXTENSION,
    SCT_SOURCE_OCSP_STAPLED_RESPONSE
} sct_source_t;

typedef enum {
    SCT_VALIDATION_STATUS_NOT_SET,
    SCT_VALIDATION_STATUS_UNKNOWN_LOG,
    SCT_VALIDATION_STATUS_VALID,
    SCT_VALIDATION_STATUS_INVALID,
    SCT_VALIDATION_STATUS_UNVERIFIED,
    SCT_VALIDATION_STATUS_UNKNOWN_VERSION
} sct_validation_status_t;

DEFINE_STACK_OF(SCT)
DEFINE_STACK_OF(CTLOG)

CT_POLICY_EVAL_CTX *CT_POLICY_EVAL_CTX_new(void);

void CT_POLICY_EVAL_CTX_free(CT_POLICY_EVAL_CTX *ctx);

X509* CT_POLICY_EVAL_CTX_get0_cert(const CT_POLICY_EVAL_CTX *ctx);

int CT_POLICY_EVAL_CTX_set1_cert(CT_POLICY_EVAL_CTX *ctx, X509 *cert);

X509* CT_POLICY_EVAL_CTX_get0_issuer(const CT_POLICY_EVAL_CTX *ctx);

int CT_POLICY_EVAL_CTX_set1_issuer(CT_POLICY_EVAL_CTX *ctx, X509 *issuer);

const CTLOG_STORE *CT_POLICY_EVAL_CTX_get0_log_store(const CT_POLICY_EVAL_CTX *ctx);

void CT_POLICY_EVAL_CTX_set_shared_CTLOG_STORE(CT_POLICY_EVAL_CTX *ctx,
                                               CTLOG_STORE *log_store);

SCT *SCT_new(void);

SCT *SCT_new_from_base64(unsigned char version,
                         const char *logid_base64,
                         ct_log_entry_type_t entry_type,
                         uint64_t timestamp,
                         const char *extensions_base64,
                         const char *signature_base64);

void SCT_free(SCT *sct);

void SCT_LIST_free(STACK_OF(SCT) *a);

sct_version_t SCT_get_version(const SCT *sct);

__owur int SCT_set_version(SCT *sct, sct_version_t version);

ct_log_entry_type_t SCT_get_log_entry_type(const SCT *sct);

__owur int SCT_set_log_entry_type(SCT *sct, ct_log_entry_type_t entry_type);

size_t SCT_get0_log_id(const SCT *sct, unsigned char **log_id);

__owur int SCT_set0_log_id(SCT *sct, unsigned char *log_id, size_t log_id_len);

__owur int SCT_set1_log_id(SCT *sct, const unsigned char *log_id,
                           size_t log_id_len);

uint64_t SCT_get_timestamp(const SCT *sct);

void SCT_set_timestamp(SCT *sct, uint64_t timestamp);

int SCT_get_signature_nid(const SCT *sct);

__owur int SCT_set_signature_nid(SCT *sct, int nid);

size_t SCT_get0_extensions(const SCT *sct, unsigned char **ext);

void SCT_set0_extensions(SCT *sct, unsigned char *ext, size_t ext_len);

__owur int SCT_set1_extensions(SCT *sct, const unsigned char *ext,
                               size_t ext_len);

size_t SCT_get0_signature(const SCT *sct, unsigned char **sig);

void SCT_set0_signature(SCT *sct, unsigned char *sig, size_t sig_len);

__owur int SCT_set1_signature(SCT *sct, const unsigned char *sig,
                              size_t sig_len);

sct_source_t SCT_get_source(const SCT *sct);

__owur int SCT_set_source(SCT *sct, sct_source_t source);

const char *SCT_validation_status_string(const SCT *sct);

void SCT_print(const SCT *sct, BIO *out, int indent, const CTLOG_STORE *logs);

void SCT_LIST_print(const STACK_OF(SCT) *sct_list, BIO *out, int indent,
                    const char *separator, const CTLOG_STORE *logs);

sct_validation_status_t SCT_get_validation_status(const SCT *sct);

__owur int SCT_validate(SCT *sct, const CT_POLICY_EVAL_CTX *ctx);

__owur int SCT_LIST_validate(const STACK_OF(SCT) *scts,
                             CT_POLICY_EVAL_CTX *ctx);

__owur int i2o_SCT_LIST(const STACK_OF(SCT) *a, unsigned char **pp);

STACK_OF(SCT) *o2i_SCT_LIST(STACK_OF(SCT) **a, const unsigned char **pp,
                            size_t len);

__owur int i2d_SCT_LIST(const STACK_OF(SCT) *a, unsigned char **pp);

STACK_OF(SCT) *d2i_SCT_LIST(STACK_OF(SCT) **a, const unsigned char **pp,
                            long len);

__owur int i2o_SCT(const SCT *sct, unsigned char **out);

SCT *o2i_SCT(SCT **psct, const unsigned char **in, size_t len);

CTLOG *CTLOG_new(EVP_PKEY *public_key, const char *name);

int CTLOG_new_from_base64(CTLOG ** ct_log,
                          const char *pkey_base64, const char *name);

void CTLOG_free(CTLOG *log);

const char *CTLOG_get0_name(const CTLOG *log);

void CTLOG_get0_log_id(const CTLOG *log, const uint8_t **log_id,
                       size_t *log_id_len);

EVP_PKEY *CTLOG_get0_public_key(const CTLOG *log);

CTLOG_STORE *CTLOG_STORE_new(void);

void CTLOG_STORE_free(CTLOG_STORE *store);

const CTLOG *CTLOG_STORE_get0_log_by_id(const CTLOG_STORE *store,
                                        const uint8_t *log_id,
                                        size_t log_id_len);

__owur int CTLOG_STORE_load_file(CTLOG_STORE *store, const char *file);

__owur int CTLOG_STORE_load_default_file(CTLOG_STORE *store);

int ERR_load_CT_strings(void);

# define CT_F_CTLOG_NEW                                   117
# define CT_F_CTLOG_NEW_FROM_BASE64                       118
# define CT_F_CTLOG_NEW_FROM_CONF                         119
# define CT_F_CTLOG_NEW_NULL                              120
# define CT_F_CTLOG_STORE_LOAD_CTX_NEW                    122
# define CT_F_CTLOG_STORE_LOAD_FILE                       123
# define CT_F_CTLOG_STORE_LOAD_LOG                        130
# define CT_F_CTLOG_STORE_NEW                             131
# define CT_F_CT_BASE64_DECODE                            124
# define CT_F_CT_POLICY_EVAL_CTX_NEW                      133
# define CT_F_CT_V1_LOG_ID_FROM_PKEY                      125
# define CT_F_I2O_SCT                                     107
# define CT_F_I2O_SCT_LIST                                108
# define CT_F_I2O_SCT_SIGNATURE                           109
# define CT_F_O2I_SCT                                     110
# define CT_F_O2I_SCT_LIST                                111
# define CT_F_O2I_SCT_SIGNATURE                           112
# define CT_F_SCT_CTX_NEW                                 126
# define CT_F_SCT_NEW                                     100
# define CT_F_SCT_NEW_FROM_BASE64                         127
# define CT_F_SCT_SET0_LOG_ID                             101
# define CT_F_SCT_SET1_EXTENSIONS                         114
# define CT_F_SCT_SET1_LOG_ID                             115
# define CT_F_SCT_SET1_SIGNATURE                          116
# define CT_F_SCT_SET_LOG_ENTRY_TYPE                      102
# define CT_F_SCT_SET_SIGNATURE_NID                       103
# define CT_F_SCT_SET_VERSION                             104
# define CT_F_SCT_CTX_VERIFY                              128

# define CT_R_BASE64_DECODE_ERROR                         108
# define CT_R_INVALID_LOG_ID_LENGTH                       100
# define CT_R_LOG_CONF_INVALID                            109
# define CT_R_LOG_CONF_INVALID_KEY                        110
# define CT_R_LOG_CONF_MISSING_DESCRIPTION                111
# define CT_R_LOG_CONF_MISSING_KEY                        112
# define CT_R_LOG_KEY_INVALID                             113
# define CT_R_SCT_INVALID                                 104
# define CT_R_SCT_INVALID_SIGNATURE                       107
# define CT_R_SCT_LIST_INVALID                            105
# define CT_R_SCT_LOG_ID_MISMATCH                         114
# define CT_R_SCT_NOT_SET                                 106
# define CT_R_SCT_UNSUPPORTED_VERSION                     115
# define CT_R_UNRECOGNIZED_SIGNATURE_NID                  101
# define CT_R_UNSUPPORTED_ENTRY_TYPE                      102
# define CT_R_UNSUPPORTED_VERSION                         103

#  ifdef  __cplusplus
}
#  endif
# endif
#endif
