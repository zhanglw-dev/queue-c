#include "qc_prelude.h"
#include "qc_protocol.h"



void qc_prtcl_head_hton(QcPrtclHead *head)
{
	head->protocol = htons(head->protocol);
	head->version = htons(head->version);
	head->type = htons(head->type);
	head->subtype = htons(head->subtype);
	head->packsn = htonl(head->packsn);
	head->bodysize = htonl(head->bodysize);
}


void qc_prtcl_head_ntoh(QcPrtclHead *head)
{
	head->protocol = ntohs(head->protocol);
	head->version = ntohs(head->version);
	head->type = ntohs(head->type);
	head->subtype = ntohs(head->subtype);
	head->packsn = ntohl(head->packsn);
	head->bodysize = ntohl(head->bodysize);
}


void qc_prtcl_produce_hton(QcPrtclProduce *produce)
{
	produce->msg_prioriy = htons(produce->msg_prioriy);
	produce->wait_msec = htonl(produce->wait_msec);
}


void qc_prtcl_produce_ntoh(QcPrtclProduce *produce)
{
	produce->msg_prioriy = ntohs(produce->msg_prioriy);
	produce->wait_msec = ntohl(produce->wait_msec);
}


void qc_prtcl_consume_hton(QcPrtclConsume *consume)
{
	consume->wait_msec = htonl(consume->wait_msec);
}


void qc_prtcl_consume_ntoh(QcPrtclConsume *consume)
{
	consume->wait_msec = ntohl(consume->wait_msec);
}


void qc_prtcl_reply_hton(QcPrtclReply *reply)
{
	reply->result = htonl(reply->result);
}


void qc_prtcl_reply_ntoh(QcPrtclReply *reply)
{
	reply->result = ntohl(reply->result);
}
