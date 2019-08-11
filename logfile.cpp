#include "logfile.h"
#include <iostream>
#include <ctime>
#include <string>

logfile::logfile()
{
    m_working=false;
}

bool logfile::init(void)
{
    m_working=true;
    m_iLineCounter=0;
    //Create and open logfile.txt if not already there
    m_file.open ("logfile.txt", ios::trunc);

    if (!m_file) m_working=false;

    return true;
}

int logfile::getValueLength(int value)
{
    if (value>9999) return 5;
    int counter=0;
    for (float f=value;f>=1.0;f/=10)
    {
        counter++;
    }
    return counter;
}

bool logfile::closeFile(void)
{
    m_file.close();
    return true;
}

bool logfile::log(const char*msg)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg << "\n";
}

bool logfile::log(int ID,const char*msg)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << ID << ' ' << msg << "\n";
}

bool logfile::log(const char*msg,int ID2)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg << ' ' << ID2 << "\n";
    return true;
}

bool logfile::log(const char*msg,int ID1,int ID2)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg << ' ' << ID1 << ',' << ID2 << "\n";
    return true;
}

bool logfile::log(int ID,const char*msg,int ID2)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << ID << ' ' << msg << ' ' << ID2 << "\n";
    return true;
}

bool logfile::log(const char*msg1,int ID,const char*msg2)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg1 << ' ' << ID << ' ' << msg2 << "\n";
    return true;
}

bool logfile::log(const char*msg1,const char*msg2)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg1 << ' ' << msg2 << "\n";
    return true;
}

bool logfile::log(const char*msg1,const char*msg2,const char*msg3)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg1 << ' ' << msg2 << ' ' << msg3 << "\n";
    return true;
}

bool logfile::log(const char*msg1,const char*msg2,int ID)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg1 << ' ' << msg2 << ' ' << ID << "\n";
    return true;
}

bool logfile::log(const char*msg1,int ID1,const char*msg2,int ID2)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg1 << ' ' << ID1 << ' ' << msg2 << ' ' << ID2 << "\n";
    return true;
}

bool logfile::log(const char*msg1,float value)
{
    if (!m_working) return false;
    time_t now = time(0);
    struct tm* tm = localtime(&now);

    m_file << string(5-getValueLength(m_iLineCounter),'0') << ++m_iLineCounter << " - "
    << tm->tm_year+1900 << '-' << tm->tm_mon+1 << '-' << tm->tm_mday
    << ' ' << tm->tm_hour << ':' << tm->tm_min << ':' << tm->tm_sec << ": ";
    if (tm->tm_hour<10) m_file << ' ';if (tm->tm_min<10) m_file << ' ';if (tm->tm_sec<10) m_file << ' ';

    m_file << msg1 << ' ' << value << "\n";
    return true;
}
