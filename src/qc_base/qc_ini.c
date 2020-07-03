
#include "qc_ini.h"
#include "qc_prelude.h"
#include "qc_log.h"


#define MAX_FILE_SIZE 8192
#define LEFT_BRACE  '['
#define RIGHT_BRACE ']'



static int load_ini_file(const char *file, char *buf, int *file_size)
{
  FILE *in = NULL;
  int i = 0 ;
  *file_size = 0 ;

  qc_assert(file);
  qc_assert(buf);

  in = fopen(file, "r");
  if (NULL == in)
  {
   return -1 ;
  }

  while((buf[i] = fgetc(in)) != EOF)
  {
    if(' ' != buf[i])
        i++;
    qc_assert(i < MAX_FILE_SIZE);
  }

  buf[i] = '\0';
  *file_size = i;

  fclose(in);
  return 0 ;
}


static int isnewline(char c)
{
  return ('\n' == c || '\r' == c) ? 1 : 0 ;
}

static int isend(char c)
{
  return '\0' == c ? 1 : 0 ;
}

static int isleftbarce(char c)
{
  return LEFT_BRACE == c ? 1 : 0 ;
}

static int isrightbrace(char c)
{
  return RIGHT_BRACE == c ? 1 : 0 ;
}


static int parse_file(const char * section, const char * key, const char * buf, int * sec_s, int * sec_e,
      int * key_s, int * key_e, int * value_s, int * value_e)
{
  const char * p = buf;
  int i = 0 ;

  qc_assert(buf != NULL);
  qc_assert(section != NULL && strlen(section));
  qc_assert(key != NULL && strlen(key));

  * sec_e = * sec_s = * key_e = * key_s = * value_s = * value_e = - 1 ;

  while (! isend(p[i]))
  {
    // find the section
    if ((0 == i || isnewline(p[i - 1 ])) && isleftbarce(p[i]))
    {
      int section_start = i + 1 ;
      // find the ']'
    do
    {
      i ++ ;
    }while (! isrightbrace(p[i]) && ! isend(p[i]));

    if (0 == strncmp(p + section_start,section, i - section_start))
    {
      int newline_start = 0 ;
      i ++ ;

      // Skip over space char after ']'
      while (isspace(p[i]))
      {
        i ++ ;
      }

     // find the section
     * sec_s = section_start;
     * sec_e = i;

     while (! (isnewline(p[i - 1 ]) && isleftbarce(p[i])) && ! isend(p[i]))
    {
      int j = 0 ;
      // get a new line
     newline_start = i;

      while (! isnewline(p[i]) && ! isend(p[i]))
     {
      i ++ ;
     }
      // now i is equal to end of the line

     j = newline_start;

      if (';' != p[j]) // skip over comment
     {
       while (j < i && p[j] != '=')
      {
       j ++ ;
        if ('=' == p[j])
       {

         if (strncmp(key,p + newline_start,j - newline_start) == 0)
        {
          // find the key ok
          * key_s = newline_start;
          * key_e = j - 1 ;

          * value_s = j + 1 ;
          * value_e = i;

          return 0 ;
        }
       }
      }
     }

     i ++ ;
    }
   }
  }
   else
  {
   i ++ ;
  }
 }
  return -1 ;
}


int read_ini_string(const char * section, const char * key, char * value, int size, const char * file)
{
  char buf[MAX_FILE_SIZE] = { 0 };
  int file_size;
  int sec_s,sec_e,key_s,key_e, value_s, value_e;

  // check parameters
 qc_assert(section != NULL && strlen(section));
 qc_assert(key != NULL && strlen(key));
 qc_assert(value != NULL);
 qc_assert(size > 0);
 qc_assert(file != NULL && strlen(key));

  if(load_ini_file(file,buf, & file_size) < 0)
   return -1 ;

  if(parse_file(section,key,buf, & sec_s, & sec_e, & key_s, & key_e, & value_s, & value_e) < 0)
 {
   return -1 ; // not find the key
 }
  else
 {
   int cpcount = value_e - value_s;

   if (size - 1 < cpcount)
  {
   cpcount = size - 1 ;
  }
 
  memset(value, 0 , size);
  memcpy(value,buf + value_s, cpcount);
  value[cpcount] = '\0' ;

   return 0 ;
 }
}


int read_ini_int(const char * section, const char * key, int default_value, const char * file)
{
  char value[ 32 ] = { 0 };
  if (read_ini_string(section,key,value, sizeof (value),file) < 0)
 {
   return default_value;
 }
  else
 {
   return atoi(value);
 }
}

int write_ini_string(const char * section, const char * key, const char * value, const char * file)
{
  char buf[MAX_FILE_SIZE] = { 0 };
  char w_buf[MAX_FILE_SIZE] = { 0 };
  int sec_s,sec_e,key_s,key_e, value_s, value_e;
  int value_len = (int)strlen(value);
  int file_size;
 FILE * out ;

  // check parameters
 qc_assert(section != NULL && strlen(section));
 qc_assert(key != NULL && strlen(key));
 qc_assert(value != NULL);
 qc_assert(file != NULL && strlen(key));

  if (load_ini_file(file,buf, & file_size) < 0)
 {
  sec_s = - 1 ;
 }
  else
 {
  parse_file(section,key,buf, & sec_s, & sec_e, & key_s, & key_e, & value_s, & value_e);
 }

  if (- 1 == sec_s)
 {
  
   if (0 == file_size)
  {
   sprintf(w_buf + file_size, " [%s]\n%s=%s\n " ,section,key,value);
  }
   else
  {
    // not find the section, then add the new section at end of the file
   memcpy(w_buf,buf,file_size);
   sprintf(w_buf + file_size, " \n[%s]\n%s=%s\n " ,section,key,value);
  }
  
  
 }
  else if (- 1 == key_s)
 {
   // not find the key, then add the new key & value at end of the section
  memcpy(w_buf,buf,sec_e);
  sprintf(w_buf + sec_e, " %s=%s\n " ,key,value);
  sprintf(w_buf + sec_e + strlen(key) + strlen(value) + 2 ,buf + sec_e, file_size - sec_e);
 }
  else
 {
   // update value with new value
  memcpy(w_buf,buf,value_s);
  memcpy(w_buf + value_s,value, value_len);
  memcpy(w_buf + value_s + value_len, buf + value_e, file_size - value_e);
 }
 
  out = fopen(file, " w ");
  if (NULL == out)
 {
   return -1 ;
 }
 
  if (- 1 == fputs(w_buf, out))
 {
  fclose(out);
   return -1 ;
 }

 fclose(out);
 
  return 0 ;
}


