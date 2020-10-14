#include "stprint.h"
stprint::stprint()
{
  printdata="";
}
int stprint::printf(String *data,const char *format, ...)
{
  data->replace("%d",String());
  va_list list;
  va_start(list,format);
  *data="";
  int acc=5;
  while(*format != '\0') 
  {
    if(*format=='%')
    {
      format++;
      if(*format=='.')
      {
        format++;
        acc=(int)(*format-'0');
        format++;
      }
      switch(*format) 
      {
        case 's':
          *data=String(*data+String(va_arg(list, char*)));
          break;
        case 'c':
          *data=String(*data+String((char)va_arg(list, int)));
          break;
        case 'd':
          *data=String(*data+String(va_arg(list, int)));
          break;
        case 'x':
          *data=String(*data+String(va_arg(list, int),HEX));
          break;
        case 'b':
          *data=String(*data+String(va_arg(list, int),BIN));
          break;
        case 'f':
          *data=String(*data+String((float)va_arg(list,double),acc));
          break;
        case 'l':
          format++;
          switch(*format)
          {
            case 'd':
              *data=String(*data+String(va_arg(list,long)));
              break;
            case 'f':
              *data=String(*data+String(va_arg(list,double),acc));
              break;
          }
          break;
        case '%':
          *data=String(*data+"%");
          break;
      }
    }
    else *data=String(*data+String(*format));
    format++;
  }
  va_end(list);
  return data->length();
}
int stprint::addprintf(String *data,const char *format, ...)
{
  data->replace("%d",String());
  va_list list;
  va_start(list,format);
  //*data="";
  int acc=5;
  while(*format != '\0') 
  {
    if(*format=='%')
    {
      format++;
      if(*format=='.')
      {
        format++;
        acc=(int)(*format-'0');
        format++;
      }
      switch(*format) 
      {
        case 's':
          *data=String(*data+String(va_arg(list, char*)));
          break;
        case 'c':
          *data=String(*data+String((char)va_arg(list, int)));
          break;
        case 'd':
          *data=String(*data+String(va_arg(list, int)));
          break;
        case 'x':
          *data=String(*data+String(va_arg(list, int),HEX));
          break;
        case 'b':
          *data=String(*data+String(va_arg(list, int),BIN));
          break;
        case 'f':
          *data=String(*data+String((float)va_arg(list,double),acc));
          break;
        case 'l':
          format++;
          switch(*format)
          {
            case 'd':
              *data=String(*data+String(va_arg(list,long)));
              break;
            case 'f':
              *data=String(*data+String(va_arg(list,double),acc));
              break;
          }
          break;
        case '%':
          *data=String(*data+"%");
          break;
      }
    }
    else *data=String(*data+String(*format));
    format++;
  }
  va_end(list);
  return data->length();
}
int stprint::printf(const char *format, ...)
{
  String *data=&printdata;
  data->replace("%d",String());
  va_list list;
  va_start(list,format);
  *data="";
  int acc=5;
  while(*format != '\0') 
  {
    if(*format=='%')
    {
      format++;
      if(*format=='.')
      {
        format++;
        acc=(int)(*format-'0');
        format++;
      }
      switch(*format) 
      {
        case 's':
          *data=String(*data+String(va_arg(list, char*)));
          break;
        case 'c':
          *data=String(*data+String((char)va_arg(list, int)));
          break;
        case 'd':
          *data=String(*data+String(va_arg(list, int)));
          break;
        case 'x':
          *data=String(*data+String(va_arg(list, int),HEX));
          break;
        case 'b':
          *data=String(*data+String(va_arg(list, int),BIN));
          break;
        case 'f':
          *data=String(*data+String((float)va_arg(list,double),acc));
          break;
        case 'l':
          format++;
          switch(*format)
          {
            case 'd':
              *data=String(*data+String(va_arg(list,long)));
              break;
            case 'f':
              *data=String(*data+String(va_arg(list,double),acc));
              break;
          }
          break;
        case '%':
          *data=String(*data+"%");
          break;
      }
    }
    else *data=String(*data+String(*format));
    format++;
  }
  va_end(list);
  return data->length();
}
int stprint::addprintf(const char *format, ...)
{
  String *data=&printdata;
  data->replace("%d",String());
  va_list list;
  va_start(list,format);
  //*data="";
  int acc=5;
  while(*format != '\0') 
  {
    if(*format=='%')
    {
      format++;
      if(*format=='.')
      {
        format++;
        acc=(int)(*format-'0');
        format++;
      }
      switch(*format) 
      {
        case 's':
          *data=String(*data+String(va_arg(list, char*)));
          break;
        case 'c':
          *data=String(*data+String((char)va_arg(list, int)));
          break;
        case 'd':
          *data=String(*data+String(va_arg(list, int)));
          break;
        case 'x':
          *data=String(*data+String(va_arg(list, int),HEX));
          break;
        case 'b':
          *data=String(*data+String(va_arg(list, int),BIN));
          break;
        case 'f':
          *data=String(*data+String((float)va_arg(list,double),acc));
          break;
        case 'l':
          format++;
          switch(*format)
          {
            case 'd':
              *data=String(*data+String(va_arg(list,long)));
              break;
            case 'f':
              *data=String(*data+String(va_arg(list,double),acc));
              break;
          }
          break;
        case '%':
          *data=String(*data+"%");
          break;
      }
    }
    else *data=String(*data+String(*format));
    format++;
  }
  va_end(list);
  return data->length();
}
stprint St;