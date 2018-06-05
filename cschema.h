#ifndef CSCHEMA_H
#define CSCHEMA_H

#include <QFile>
#include <QTextStream>
#include "celement.h"
#include <vector>
using namespace std;

class cschema
{
private:
    vector<QString> strings;
    celement * create(int&, vector<QString>&);

public:
    cschema(QFile&);

    celement * get_result();
};

#endif // CSCHEMA_H
