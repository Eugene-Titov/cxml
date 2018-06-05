#include <QCoreApplication>
#include "cschema.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    for(int i = 0; i < argc; ++i) cout << argv[i] << endl;
//    int t; cin >> t;

    QString file = argv[1];
    QString output = argv[2];

//    file = "/home/zhenya/2/chrome_distort.izd";
//    output = "/home/zhenya/2/chrome_distort.xsheme";

//    QFile f("/home/zhenya/2/d");
    QFile f(file);
    cschema s(f);
    celement * res = s.get_result();
//    cout << res->get_content().toStdString() << endl;
//    res->save("/home/zhenya/chat.xscheme");
//    res->save("/home/zhenya/2/d.xscheme");
    res->save(output);
    delete res;

    cout << "exit" << endl;
    return 0;//a.exec();
}
