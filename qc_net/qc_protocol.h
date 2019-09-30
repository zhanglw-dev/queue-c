#ifndef QC_PROTOCOL_H
#define QC_PROTOCOL_H


#define QC_PROTOCOL_MQ    55555
#define QC_PROTOCOL_VERSION 1

#define QC_TYPE_REGISTER  100
#define QC_TYPE_PRODUCER  110
#define QC_TYPE_CONSUMER  111
#define QC_TYPE_MSGGET    120
#define QC_TYPE_MSGPUT    121
#define QC_TYPE_REPLY     130

#pragma pack(push)
#pragma pack(1)


typedef struct {
	unsigned short protocol;
	unsigned short version;
	unsigned short type;
	unsigned short subtype;
	unsigned int packsn;
	unsigned int body_len;
	char reserve[16];
}QcPrtclHead;


typedef struct {
	char qname[32];
}QcPrtclRegister;


typedef struct {
	int wait_msec;
	unsigned short msg_prioriy;
	unsigned int msg_len;
}QcPrtclProduce;


typedef struct {
	int wait_msec;
	//unsigned int msg_len;
}QcPrtclConsume;


typedef struct {
	int result;
	unsigned int msg_len;
}QcPrtclReply;


#pragma pack(pop)


void qc_prtcl_head_hton(QcPrtclHead *head);
void qc_prtcl_head_ntoh(QcPrtclHead *head);

void qc_prtcl_register_hton(QcPrtclRegister *regist);
void qc_prtcl_register_ntoh(QcPrtclRegister *regist);

void qc_prtcl_produce_hton(QcPrtclProduce *produce);
void qc_prtcl_produce_ntoh(QcPrtclProduce *produce);

void qc_prtcl_consume_hton(QcPrtclConsume *consume);
void qc_prtcl_consume_ntoh(QcPrtclConsume *consume);

void qc_prtcl_reply_hton(QcPrtclReply *reply);
void qc_prtcl_reply_ntoh(QcPrtclReply *reply);


#endif /*QC_PROTOCOL_H*/
