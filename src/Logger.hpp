/*******************************************
 *
 *******************************************/
#ifndef __DEBUG_LOG_H__
#define __DEBUG_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>


#define logI(...) printf("[I]\t%s\t[+%d]\t[%s] : ", __FILE__, __LINE__, __FUNCTION__), printf(__VA_ARGS__), printf("\n");
// #define logW(...) do{if(LOG_LEVEL_FLAGS & LOG_WARN)printf("[W]\t%s\t[+%d]\t[%s] : ", __FILE__, __LINE__, __FUNCTION__), printf(__VA_ARGS__), printf("\n");}while(0);
// #define logE(...) do{if(LOG_LEVEL_FLAGS & LOG_ERROR)printf("[E]\t%s\t[+%d]\t[%s] : ", __FILE__, __LINE__, __FUNCTION__), printf(__VA_ARGS__), printf("\n");}while(0);


class MyTimer
{
public:
    MyTimer() : m_iStart(clock()), m_iInit(clock()), m_bReset(false) {};
    
    void reset() { m_bReset = true; m_iStart = clock();}
    double time()
    {
        if( m_bReset ) {
        	m_bReset = false; return (double(clock() - m_iStart) * 1000.0f / double(CLOCKS_PER_SEC));
        }
        else
            return (double(clock() - m_iInit) * 1000.0f / double(CLOCKS_PER_SEC));
    }
    
private:
    clock_t m_iStart, m_iInit;
    bool	m_bReset;
};


#endif
