// component_creator.hpp

#pragma once

#include "interface_data.hpp"

class QDir;

/**\brief this class can create project files or xml files from interface_data
 * structurs*/
class component_creator
{
    enum Mode
    {
        executable,
        library
    };

public:
    component_creator();
    /**\return list names of created files, if file was not created if will be
     * empty string
     * \brief generate header, cpp and cmake files (also, if mode == executable,
     * generate main.cpp). If component_name is empty generate nothing
     * \param mode if library, generate library project, if executable - generate
     * executable project
     * \param path path to directory, where will generate new project directory
     * with files. The directory have to exists
     * \except if component_name is empty, or if directory not exist, or couldn't
     * create folder for project*/
    QStringList generate_project(const QString &component_name, const QDir &path,
                                 component_creator::Mode mode = library);
    /**\brief generate new xmls files from list of interfaces
     * \warning one interface_data have to contain only one class*/
    QStringList generate_xmls(const QDir &destination);
    void add_interface(const interface_data &data);
    /**\brief get final interface from all adding interfaces*/
    interface_data merge_interfaces(const std::list<interface_data> &input);

private:
    QString generate_header_file(const QString &component_name, const QDir &path, const interface_data &data);
    QString generate_cpp_file(const QString &component_name, const QDir &path, const interface_data &data);
    QString generate_cmake_file(const QString &component_name, const QDir &path, const interface_data &data,
                                component_creator::Mode mode);
    QString generate_main_file(const QString &component_name, const QDir &path, const interface_data &data);
    QString generate_xml(const interface_data &data, const QDir &destination) const;

public:
    std::list<interface_data> inheritance_interfaces_;
};
