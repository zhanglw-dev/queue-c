#ifndef QC_NETCONF_H
#define QC_NETCONF_H


typedef struct __QcNetConfig QcNetConfig;


QcNetConfig* qc_netconf_create();

void qc_netconf_destory(QcNetConfig *netConf);

void qc_netconf_setip(QcNetConfig *netConf, const char *ip);

char* qc_netconf_getip(QcNetConfig *netConf);

void qc_netconf_setport(QcNetConfig *netConf, int port);

int qc_netconf_getport(QcNetConfig *netConf);


#endif //QC_NETCONF_H
