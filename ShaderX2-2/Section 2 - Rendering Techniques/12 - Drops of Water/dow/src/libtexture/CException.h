#ifndef __EXCEPT__
#define __EXCEPT__

#include <stdarg.h>
#include <stdio.h>

class CException
{
 protected:
  char m_szMsg[512];
 public:

  CException(){m_szMsg[0]='\0';}

  CException(char *msg,...)
    {
      va_list args;
      va_start(args, msg);
      
      vsprintf(m_szMsg,msg,args);
      
      va_end(args);
    }

  const char *getMsg() const {return (m_szMsg);}
};

#endif
