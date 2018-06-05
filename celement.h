#ifndef CELEMENT_H
#define CELEMENT_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QUuid>
#include <QDataStream>
#include <list>
#include <stdlib.h>
#include <iostream>
using namespace std;

class celement
{
protected:
    QString header, footer;
    list<celement*> elems;
    int count_tabs;
    int limit_children;

public:
    celement() : count_tabs(0), limit_children(-1) {}
    virtual ~celement()
    {
        for(list<celement*>::iterator i = elems.begin(); i != elems.end(); ++i) delete *i;
    }

    void add_tab(int count) { count_tabs = count; }
    void add_element(celement * e)
    {
        if(limit_children > -1 && limit_children == elems.size())
        {
            delete e;
            return;
        }
        e->add_tab(count_tabs + 1);
        elems.push_back(e);
    }

    QString get_content()
    {
        QString tab;
        for(int i = 0; i < count_tabs; ++i) tab += "\t";

        QString s = tab + header;
        for(list<celement*>::iterator e = elems.begin(); e != elems.end(); ++e) s += tab + (*e)->get_content();
        s += tab + footer;
        return s;
    }

    virtual void save(const QString &path)
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream out(&file);
        out << get_content();
        file.close();
    }

    int get_limit_children() { return limit_children; }

    virtual celement * clone() = 0;
};

class celement_auto : public celement
{
public:
    celement_auto(celement_auto &o)
    {
        header = o.header;
        footer = o.footer;
        count_tabs = o.count_tabs;
        limit_children = o.limit_children;
        for(list<celement*>::iterator i = o.elems.begin(); i != o.elems.end(); ++i) elems.push_back((*i)->clone());
    }

    celement_auto(QStringList &l)
    {
        header = "<chatbot:autocount caption=\"" + l.at(1) + "\" size=\"" + l.at(2) + "\">\n";
        footer = "</chatbot:autocount>\n";

//        cout << l.size() << " here" << endl;
        if(l.size() > 3)
        {
            int min = l.at(3).toInt(), max = l.at(4).toInt();
//            cout << min << " min " << max << " max" << endl;
            limit_children = min + (max > min ? rand() % (max - min) : 0 );
//            cout << limit_children << " limit_children" << endl;
        }
    }

    virtual celement_auto * clone()
    {
        return new celement_auto(*this);
    }
};

class celement_prop : public celement
{
private:
    QStringList l;
    void init()
    {
        QString d;
        if(l.size() == 6)
        {
            int _min = l.at(4).toInt();
            int _max = l.at(5).toInt();
            d = QString::number(_min + rand() % (_max - _min));
        }
        else d = l.at(6);

        header = "<chatbot:property caption=\"" + l.at(1) + "\" size=\"" + l.at(2) + "\" type=\"" + l.at(3) + "\" default=\"" + d + "\" min=\"" + l.at(4) + "\" max=\"" + l.at(5) + "\"/>\n";
    }

public:
    celement_prop(celement_prop &o)
    {
        count_tabs = o.count_tabs;
        l = o.l;
        init();
    }

    celement_prop(QStringList &_l) : l(_l)
    {
        init();
    }

    virtual celement_prop * clone()
    {
        return new celement_prop(*this);
    }
};

class celement_text : public celement
{
private:
    QStringList l;
    void init()
    {
        header = "<chatbot:autosize caption=\"" + l.at(1) + "\" size=\"" + l.at(2) + "\">\n";
        QString text; for(int i = 0; i < l.at(2).toInt(); ++i) text += "T";
        header += "<chatbot:property caption=\"Текст\" type=\"string\" size=\"0\" default=\"" + text + "\" />\n";
        footer = "</chatbot:autosize>\n";
    }

public:
    celement_text(celement_text &o) { l = o.l; init(); }
    celement_text(QStringList &_l) { init(); }

};

#include <QByteArray>
class celement_uuid : public celement
{
private:
    QStringList l;

    void init()
    {
        QUuid u = QUuid::createUuid();
        QString s = u.toString();
        s.replace("-","");
        s = s.left(s.length() - 1);
        s = s.right(s.length() - 1);
//        cout << s.toStdString() << " s" << endl;

        int part = 2;
        unsigned char m[16];
        int i = 0;
        while(s.length())
        {
            bool ok = false;
            QString left = s.left(part); s = s.right(s.length() - part);
            m[i++] = (char)left.toUInt(&ok,16);
        }

//        for(int i = 0; i < 16; ++i)
//        {
//            cout << QString::number((int)m[i],16).toStdString() << endl;
//        }

        for(int i = 0; i < 4; ++i)
        {
            unsigned int v;
            memcpy(&v,m + i*4,4);
//            cout << v << " v " << endl;
            header += "<chatbot:property caption=\"uuid_" + QString::number(i) + "\" size=\"4\" type=\"unsigned integer\" default=\"" + QString::number(v) + "\"/>\n";
        }

//        cout << header.toStdString() << endl;
//        int t; cin >> t;
    }

public:
    celement_uuid(celement_uuid &o)
    {
        count_tabs = o.count_tabs;
        l = o.l;
        init();
    }

    celement_uuid(QStringList &_l) : l(_l) { init(); }

    virtual celement_uuid * clone()
    {
        return new celement_uuid(*this);
    }
};

class celement_mes : public celement
{
public:
    celement_mes(celement_mes &o)
    {
        header = o.header;
        footer = o.footer;
        count_tabs = o.count_tabs;
        limit_children = o.limit_children;
        for(list<celement*>::iterator i = o.elems.begin(); i != o.elems.end(); ++i) elems.push_back((*i)->clone());
    }

    celement_mes(QStringList &l)
    {
        header = "<?xml version='1.0' encoding='UTF-8'?>\n";
        header += "<root xmlns:chatbot=\"http://chatbot.com/typeinfo/\">\n";
        header += "\t<chatbot:message title=\"" + l.at(1) + "\" description=\"" + l.at(2) + "\" encoding=\"UTF-8\">\n";
        header += "\t<chatbot:property caption=\"Номер кодограммы\" size=\"" + l.at(3) + "\" type=\"integer\" default=\"" + l.at(4) + "\" readonly=\"true\" />\n";
        footer = "</chatbot:message>\n";
        footer += "</root>";
    }

    virtual celement_mes * clone()
    {
        return new celement_mes(*this);
    }
};

class celement_block : public celement
{
public:
    celement_block(celement_block &o)
    {
        header = o.header;
        footer = o.footer;
        count_tabs = o.count_tabs;
        limit_children = o.limit_children;
        for(list<celement*>::iterator i = o.elems.begin(); i != o.elems.end(); ++i) elems.push_back((*i)->clone());
    }

    celement_block(QStringList &l)
    {
        header = "<chatbot:block caption=\"" + l.at(1) + "\">\n";
        footer = "</chatbot:block>\n";
    }

    virtual celement_block * clone()
    {
        return new celement_block(*this);
    }
};



#endif // CELEMENT_H
