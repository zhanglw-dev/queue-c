#ifndef QC_PROTOCOL_H
#define QC_PROTOCOL_H

#define QC_PROTOCOL_MQ    55555
#define QC_PROTOCOL_VERSION 1

#define QC_TYPE_MSGPUT    120
#define QC_TYPE_MSGGET    121
#define QC_TYPE_REPLY     130

#pragma pack(push)
#pragma pack(1)


typedef struct {
	unsigned short protocol;
	unsigned short version;
	unsigned short type;
	unsigned int packsn;
	unsigned int body_len;
	char reserve[16];
}QcPrtclHead;


typedef struct {
	char qname[32];
	int wait_msec;
	unsigned short msg_prioriy;
	unsigned int msg_len;
}QcPrtclMsgPut;


typedef struct {
	char qname[32];
	int wait_msec;
}QcPrtclMsgGet;


typedef struct {
	int result;
	unsigned int msg_len;
}QcPrtclReply;


#pragma pack(pop)


void qc_prtcl_head_hton(QcPrtclHead *head);
void qc_prtcl_head_ntoh(QcPrtclHead *head);

void qc_prtcl_msgget_hton(QcPrtclMsgGet *msgget);
void qc_prtcl_msgget_ntoh(QcPrtclMsgGet *msgget);

void qc_prtcl_msgput_hton(QcPrtclMsgPut *msgput);
void qc_prtcl_msgput_ntoh(QcPrtclMsgPut *msgput);

void qc_prtcl_reply_hton(QcPrtclReply *reply);
void qc_prtcl_reply_ntoh(QcPrtclReply *reply);


#endif /*QC_PROTOCOL_H*/
