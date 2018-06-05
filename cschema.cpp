#include "cschema.h"

celement * cschema::create(int &index, vector<QString> &strs)
{
    if(index == strs.size()) return NULL;

    QStringList l = strs[index].split(":");
//    cout << index << " index" << endl;
    index++;
    if(l.at(0) == "end")
    {
//        cout << "end of auto" << endl;
        return 0;
    }
    else if(l.at(0) == "auto")
    {
        celement * e = new celement_auto(l);
//        cout << e->get_limit_children() << " limit children" << endl;
        while(1)
        {
            celement * c = create(index, strs);
//            cout << c << " c" << endl;
            if(c == 0) break;

            e->add_element(c);
            for(int i = 1; i < e->get_limit_children(); ++i)
            {
                e->add_element(c->clone());
            }
        }
//        cout << "return e" << endl;
        return e;
    }
    else if(l.at(0) == "prop") return new celement_prop(l);
    else if(l.at(0) == "uuid") return new celement_uuid(l);
    else if(l.at(0) == "text") return new celement_uuid(l);
    else if(l.at(0) == "block")
    {
        celement * e = new celement_block(l);
        while(1)
        {
            celement * c = create(index, strs);
//            cout << c << " c" << endl;
            if(c == 0) break;
            e->add_element(c);
        }
//        cout << "return e" << endl;
        return e;
    }
    else if(l.at(0) == "doc")
    {
        celement * e = new celement_mes(l);
        while(1)
        {
            celement * c = create(index, strs);
//            cout << c << " c" << endl;
            if(c == 0) break;
            e->add_element(c);
        }
//        cout << "return e" << endl;
        return e;
    }
    return NULL;
}

celement * cschema::get_result()
{
    int index = 0;
    celement * element = create(index, strings);
//    for(; index < strings.size(); )
//    element->add_element(create(index, strings));

//    cout << "exit get result" << endl;
    return element;
}

cschema::cschema(QFile &file)
{    
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if(!line.isEmpty()) strings.push_back(line);
        }
    }
}
