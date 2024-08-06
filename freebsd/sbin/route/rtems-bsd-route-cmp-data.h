/* generated by userspace-header-gen.py */
#include <rtems/linkersets.h>
#include "rtems-bsd-route-data.h"
/* cmp.c */
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static CONF *conf);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static OSSL_CMP_CTX *cmp_ctx);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static X509_VERIFY_PARAM *vpm);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_cacertsout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_cert);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_certform_s);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_certout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_chainout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_cmd_s);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_config);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_csr);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_digest);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_engine);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_expect_sender);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_extracerts);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_extracertsout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_geninfo);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_infotype_s);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_issuer);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_key);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_keyform_s);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_keypass);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_mac);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_newkey);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_newkeypass);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_no_proxy);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_oldcert);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_otherpass);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_out_trusted);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_own_trusted);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_path);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_policies);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_policy_oids);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_port);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_proxy);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_recipient);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_ref);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_reqexts);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_reqin);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_reqout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_rsp_capubs);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_rsp_cert);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_rsp_extracerts);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_rspin);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_rspout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_sans);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_secret);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_section);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_server);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srv_cert);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srv_key);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srv_keypass);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srv_ref);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srv_secret);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srv_trusted);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srv_untrusted);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_srvcert);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_statusstring);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_subject);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_tls_cert);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_tls_extra);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_tls_host);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_tls_key);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_tls_keypass);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_tls_trusted);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_trusted);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *opt_untrusted);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char *prog);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static char opt_item[]);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_accept_raverified);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_accept_unprot_err);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_accept_unprotected);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_batch);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_certform);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_check_after);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_cmd);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_days);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_disable_confirm);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_failure);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_failurebits);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_grant_implicitconf);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_ignore_keyusage);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_implicit_confirm);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_infotype);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_keep_alive);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_keyform);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_max_msgs);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_msg_timeout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_pkistatus);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_policy_oids_critical);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_poll_count);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_popo);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_repeat);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_reqin_new_tid);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_revreason);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_san_nodefault);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_send_error);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_send_unprot_err);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_send_unprotected);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_tls_used);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_total_timeout);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_unprotected_errors);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_unprotected_requests);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_use_mock_srv);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int opt_verbosity);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static int rspin_in_use);
RTEMS_LINKER_RWSET_CONTENT(bsd_prog_route, static varref cmp_vars[]);
