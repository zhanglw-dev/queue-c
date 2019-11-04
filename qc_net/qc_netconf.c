#include "qc_netconf.h"
#include "qc_prelude.h"


struct __QcNetConfig {
	char ip[128 + 1];
	int port;
};



QcNetConfig* qc_netconf_create()
{
	QcNetConfig *netConf;
	qc_malloc(netConf, sizeof(QcNetConfig));
	return netConf;
}


void qc_netconf_destory(QcNetConfig *netConf)
{
	qc_free(netConf);
}


void qc_netconf_setip(QcNetConfig *netConf, const char *ip)
{
	qc_assert(ip && strlen(ip) < sizeof(netConf->ip));
	memset(netConf->ip, 0, sizeof(netConf->ip));
	strcpy(netConf->ip, ip);
}


char* qc_netconf_getip(QcNetConfig *netConf)
{
	return netConf->ip;
}


void qc_netconf_setport(QcNetConfig *netConf, int port)
{
	qc_assert(port > 0);
	netConf->port = port;
}


int qc_netconf_getport(QcNetConfig *netConf)
{
	return netConf->port;
}
