#ifndef QC_PROFILE_H
#define QC_PROFILE_H


#ifdef __cplusplus
extern   " C "
{
#endif

int read_profile_string(const char *section, const char *key, char *value, int size, const char *file);

int read_profile_int(const char *section, const char *key, int default_value, const char *file);

int write_profile_string(const char *section, const char *key, const char *value, const char *file);

#ifdef __cplusplus
};
#endif


#endif /* QC_PROFILE_H */

