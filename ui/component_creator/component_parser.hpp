// component_parser.hpp

#pragma once

#include "interface_data.hpp"

#include <QStringList>
#include <QVariant>

class QDomNode;
class QFile;
class QDir;

/**\brief this class parse xml files with description of interfaces, and create
 * interface_data sturture*/
class component_parser
{
public:
    component_parser();
    /**\except if couldn't load file
     * \param file_name file name with path to this
     * \warning delete all datas which was be taken last parsing
     * \brief generate interface_data object
     * */
    const interface_data &read_description_file(const QString &file_name);

private:
    bool add_class(const QDomNode &root);
    bool add_inheritance(const QDomNode &root);
    bool add_headers(const QDomNode &root);
    /**\warning in string, which describes method set '%1' - it is argument for
     * QString (method.arg(" ") - for example). I did it for adding name of new
     * class in generated cpp file*/
    bool add_methods(const QDomNode &root);
    bool add_packages(const QDomNode &root);

private:
    interface_data data_;
};
