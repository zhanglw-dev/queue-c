#ifndef QC_PROTOCOL_H
#define QC_PROTOCOL_H


#define QC_TYPE_PRODUCER  11
#define QC_TYPE_CONSUMER  12


#pragma pack(push)
#pragma pack(1)

typedef struct {
	unsigned short protocol;
	unsigned short version;
	unsigned short type;
	unsigned short reserve;
	unsigned int packsn;
	unsigned int bodysize;
}QcPrtclHead;


typedef struct {
	char qname[32];
	unsigned short prioriy;
	int wait_msec;
}QcProduceInfo;


typedef struct {
	char qname[32];
	unsigned short prioriy;
	int wait_msec;
}QcConsumeInfo;


#pragma pack(pop)


#endif /*QC_PROTOCOL_H*/
