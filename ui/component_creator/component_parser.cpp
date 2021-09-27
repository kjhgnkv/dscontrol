// component_parser.cpp

#include "component_parser.hpp"
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

#include <stdexcept>

#define INTERF_SCHEMA ":/component_schema.xsd"
#define CLASS_TYPE "class"
#define HEADER_TYPE "header"
#define METHOD_TYPE "methods"
#define ABSTRACT_METHOD_TYPE "pure"
#define REALIZED_METHOD_TYPE "realized"
#define INHERITANCE_TYPE "inheritance"

component_parser::component_parser()
{
}

const interface_data &component_parser::read_description_file(const QString &file_name)
{
    data_ = interface_data {};
    // this macros neded for link resources from other library
    Q_INIT_RESOURCE(xml_schema_res);
    ::QFile schema_file {INTERF_SCHEMA};
    if (!schema_file.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error {"can't load xsd schema"};
    }
    auto schema_code = schema_file.readAll();
    schema_file.close();
    ::QXmlSchema schema;
    if (!schema.load(schema_code))
    {
        throw std::runtime_error {"couldn't load xml_schema for components"};
    }
    if (!schema.isValid())
    {
        throw std::runtime_error {"xml schema for components is not valid"};
    }

    ::QFile xml_file {file_name};
    if (!xml_file.open(::QIODevice::ReadOnly))
    {
        throw std::runtime_error {"can't load xml_file: " + file_name.toStdString()};
    }
    auto xml_code = xml_file.readAll();
    ::QXmlSchemaValidator xml_validator {schema};
    xml_file.close();
    if (!xml_validator.validate(xml_code))
    {
        QString error = "file:" + file_name + " is not valid";
        throw std::runtime_error {error.toStdString().c_str()};
    }

    ::QDomDocument xml_doc {};
    xml_doc.setContent(xml_code);

    auto root = xml_doc.documentElement();

    add_packages(root);
    add_headers(root);
    add_class(root);
    add_inheritance(root);
    add_methods(root);

    return data_;
}

bool component_parser::add_class(const QDomNode &root)
{
    auto class_info = root.firstChildElement(CLASS_TYPE);
    if (!class_info.isNull())
    {
        ::QString name = class_info.text();
        data_.interface_class << name;
        return true;
    }
    return false;
}

bool component_parser::add_inheritance(const QDomNode &root)
{
    auto inheritance_info = root.firstChildElement(INHERITANCE_TYPE);
    if (!inheritance_info.isNull())
    {
        for (auto i = inheritance_info.firstChildElement(); !i.isNull(); i = i.nextSiblingElement())
        {
            data_.inheritance_classes << i.text();
        }
        return true;
    }
    return false;
}

bool component_parser::add_headers(const QDomNode &root)
{
    auto header = root.firstChildElement(HEADER_TYPE);
    if (!header.isNull())
    {
        data_.headers << header.text();
        return true;
    }
    return false;
}

bool component_parser::add_methods(const QDomNode &root)
{
    auto methods = root.firstChildElement(METHOD_TYPE);
    if (!methods.isNull())
    {
        for (auto i = methods.firstChildElement(); !i.isNull(); i = i.nextSiblingElement())
        {
            auto method_info = i.firstChildElement();
            ::QString method_status_type = method_info.text();
            method_info = method_info.nextSiblingElement();
            ::QString name = method_info.text();
            method_info = method_info.nextSiblingElement();
            ::QString signature = method_info.text();
            // All functions have to have (), but I check it just in case
            int index = signature.indexOf('(');
            ::QString func;
            if (index >= 0)
            {
                func = signature.insert(index, "%1" + name);
            }
            if (method_status_type == ABSTRACT_METHOD_TYPE)
            {
                if (!data_.abstract_methods.contains(func))
                {
                    data_.abstract_methods << func;
                }
            }
            else if (method_status_type == REALIZED_METHOD_TYPE)
            {
                if (!data_.realized_methods.contains(func))
                {
                    data_.realized_methods << func;
                }
            }
        }
        return true;
    }
    return false;
}

bool component_parser::add_packages(const QDomNode &root)
{
    auto packages = root.firstChildElement();
    if (!packages.isNull())
    {
        for (auto i = packages.firstChildElement(); !i.isNull(); i = i.nextSiblingElement())
        {
            ::QString name = i.text();
            if (!data_.packages.contains(name))
            {
                data_.packages << name;
            }
        }
        return true;
    }
    return false;
}
