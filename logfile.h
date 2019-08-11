#ifndef LOGFILE_H
#define LOGFILE_H

#include <fstream>
#include <string>

using namespace std;

class logfile
{
    public:

        logfile();

        bool init(void);
        bool closeFile(void);
        bool log(const char*);
        bool log(int,const char*);
        bool log(const char*,int);
        bool log(const char*,int,int);
        bool log(int,const char*,int);
        bool log(const char*,int,const char*);
        bool log(const char*,const char*);
        bool log(const char*,const char*,const char*);
        bool log(const char*,const char*,int);
        bool log(const char*,int,const char*,int);
        bool log(const char*,float);

    private:
        ofstream m_file;
        bool m_working;
        int  m_iLineCounter;

        int    getValueLength(int);

};

#endif // LOGFILE_H
