#include "component_creator.hpp"

// #include "config.h"
#include <QDir>
#include <QDomDocument>
#include <QTextStream>
#include <stdexcept>

component_creator::component_creator()
{
}

::QStringList component_creator::generate_project(const QString &component_name, const QDir &path,
                                                  component_creator::Mode mode)
{
    QStringList files {};
    if (inheritance_interfaces_.empty())
    {
        throw std::runtime_error {"no one interfase select"};
    }
    if (!path.exists())
    {
        auto error = "path: " + path.absolutePath() + " not exists";
        throw std::runtime_error {error.toStdString().c_str()};
    }
    if (component_name.isEmpty())
    {
        throw std::runtime_error {"component_name is empty"};
    }
    // TODO: remove subfolder
    if (/*path.mkdir(component_name)*/ true)
    {
        QDir project_path {
        path.absolutePath() + ::QDir::separator()/* +
                          component_name*/};
        auto component_base = merge_interfaces(inheritance_interfaces_);
        files << generate_header_file(component_name, project_path, component_base);
        files << generate_cpp_file(component_name, project_path, component_base);
        files << generate_cmake_file(component_name, project_path, component_base, mode);
        if (mode == component_creator::Mode::executable)
        {
            files << generate_main_file(component_name, project_path, component_base);
        }
    }
    else
    {
        auto error = "couldn't create folder in path: " + path.absolutePath();
        throw std::runtime_error {error.toStdString().c_str()};
    }
    return files;
}

QStringList component_creator::generate_xmls(const QDir &destination)
{
    QStringList files;
    for (const auto &i : inheritance_interfaces_)
    {
        files << generate_xml(i, destination);
    }
    return files;
}

void component_creator::add_interface(const interface_data &data)
{
    inheritance_interfaces_.push_back(data);
}

interface_data component_creator::merge_interfaces(const std::list<interface_data> &input)
{
    interface_data retval;
    for (const auto &i : input)
    {
        for (const auto &j : i.packages)
        {
            if (!retval.packages.contains(j))
            {
                retval.packages << j;
            }
        }

        for (const auto &j : i.headers)
        {
            if (!retval.headers.contains(j))
            {
                retval.headers << j;
            }
        }

        for (const auto &j : i.interface_class)
        {
            if (!retval.interface_class.contains(j))
            {
                retval.interface_class << j;
            }
        }

        for (const auto &j : i.abstract_methods)
        {
            if (!retval.abstract_methods.contains(j))
            {
                retval.abstract_methods << j;
            }
        }

        for (const auto &j : i.realized_methods)
        {
            if (!retval.realized_methods.contains(j))
            {
                retval.realized_methods << j;
            }
        }
    }
    return retval;
}

QString component_creator::generate_header_file(const QString &component_name, const QDir &path,
                                                const interface_data &data)
{
    ::QStringList header {};
    header << "// " + component_name + ".h" << "#pragma once";
    for (auto &i : data.headers)
    {
        header << "#include \"" + i + "\"";
    }
    header << "class " + component_name + " :";
    for (auto i = data.interface_class.begin();;)
    {
        header << " public " + *i;
        if (++i != data.interface_class.end())
        {
            header.last().append(',');
        }
        else
        {
            break;
        }
    }
    header << " {" << "public:" << "explicit " + component_name + "();";
    for (auto &i : data.abstract_methods)
    {
        header << "virtual " + i.arg(" ") + " override;";
    }
    header << "private:" << "};";
    ::QFile header_file {path.absolutePath() + ::QDir::separator() + component_name + ".h"};
    if (header_file.open(::QIODevice::WriteOnly | ::QIODevice::Text))
    {
        ::QTextStream stream {&header_file};
        stream << header.join('\n');
        header_file.close();
        return header_file.fileName();
    }
    return "";
}

QString component_creator::generate_cpp_file(const QString &component_name, const QDir &path,
                                             const interface_data &data)
{
    ::QStringList cpp {};
    cpp << "// " + component_name + ".cpp" << "#include \"" + component_name + ".h\"";
    cpp << component_name + "::" + component_name + "() {\n}";
    for (auto &i : data.abstract_methods)
    {
        cpp << i.arg(" " + component_name + "::") + "{\n}";
    }
    ::QFile cpp_file {path.absolutePath() + ::QDir::separator() + component_name + ".cpp"};
    if (cpp_file.open(::QIODevice::WriteOnly | ::QIODevice::Text))
    {
        ::QTextStream stream {&cpp_file};
        stream << cpp.join('\n');
        cpp_file.close();
        return cpp_file.fileName();
    }
    return "";
}

QString component_creator::generate_cmake_file(const QString &component_name, const QDir &path,
                                               const interface_data &data, component_creator::Mode mode)
{
    ::QStringList cmake {};
    cmake << "# cmake for " + component_name << "cmake_minimum_required(VERSION 3.5)" << "project(" + component_name + ")" << QString {
    "set(CMAKE_MODULE_PATH "
    } + "\"" + "" + ";${CMAKE_MODULE_PATH}" + "\"" + ")";
    for (auto &i : data.packages)
    {
        cmake << "find_package(" + i + " REQUIRED)";
    }
    if (mode == component_creator::Mode::library)
    {
        cmake << "add_library(${PROJECT_NAME} " + component_name + ".cpp)";
    }
    else if (mode == component_creator::Mode::executable)
    {
        cmake << "add_executable(${PROJECT_NAME} " + component_name + ".cpp main.cpp)";
    }
    cmake << "target_compile_options(${PROJECT_NAME} PUBLIC \"-std=c++11\")";
    for (auto &i : data.packages)
    {
        cmake << "target_include_directories(${PROJECT_NAME} PUBLIC ${" + i + "_INCLUDE_DIR})";
        cmake << "target_link_libraries(${PROJECT_NAME} PUBLIC ${" + i + "_LIBRARIES})";
    }
    ::QFile cmake_file {path.absolutePath() + ::QDir::separator() + "CMakeLists.txt"};
    if (cmake_file.open(::QIODevice::WriteOnly | ::QIODevice::Text))
    {
        ::QTextStream stream {&cmake_file};
        stream << cmake.join('\n');
        cmake_file.close();
        return cmake_file.fileName();
    }
    return "";
}

QString component_creator::generate_main_file(const QString &component_name, const QDir &path,
                                              const interface_data & /*data*/)
{
    QStringList main_code {};
    main_code << "// main.cpp" << "#include \"" + component_name + ".h\"" << "#include <iostream>" << "#include <cstdlib>" << "int main(int argc, char **argv) {" << component_name + " " + component_name + "();" << "return EXIT_SUCCESS;" << "}";
    ::QFile main_file {path.absolutePath() + ::QDir::separator() + "main.cpp"};
    if (main_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ::QTextStream stream {&main_file};
        stream << main_code.join('\n');
        main_file.close();
        return main_file.fileName();
    }
    return "";
}

QString component_creator::generate_xml(const interface_data &data, const QDir &destination) const
{
    ::QDomDocument doc;
    auto instuction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instuction);
    auto root = doc.createElement("description");
    doc.appendChild(root);
    auto packages = doc.createElement("packages");
    root.appendChild(packages);
    for (const auto &i : data.packages)
    {
        auto package = doc.createElement("package");
        auto text = doc.createTextNode(i);
        package.appendChild(text);
        packages.appendChild(package);
    }

    auto class_node = doc.createElement("class");
    QString class_name = data.interface_class.first();
    auto text_node = doc.createTextNode(class_name);
    class_node.appendChild(text_node);
    root.appendChild(class_node);

    auto inheritance_info = doc.createElement("inheritance");
    root.appendChild(inheritance_info);
    for (const auto &i : data.inheritance_classes)
    {
        auto class_inherit = doc.createElement("class");
        auto text = doc.createTextNode(i);
        class_inherit.appendChild(text);
        inheritance_info.appendChild(class_inherit);
    }

    auto headers = doc.createElement("headers");
    root.appendChild(headers);
    for (const auto &i : data.headers)
    {
        auto header = doc.createElement("header");
        auto text = doc.createTextNode(i);
        header.appendChild(text);
        headers.appendChild(header);
    }

    auto methods = doc.createElement("methods");
    root.appendChild(methods);
    for (const auto &i : data.abstract_methods)
    {
        auto method = doc.createElement("method");
        auto type = doc.createElement("type");
        auto name = doc.createElement("name");
        auto signature = doc.createElement("signature");
        method.appendChild(type);
        method.appendChild(name);
        method.appendChild(signature);
        auto type_text = doc.createTextNode("pure");
        type.appendChild(type_text);
        QString func = i.arg('/');
        QString name_func = func.section('/', 1, 1).section('(', 0, 0);
        func.remove('/');
        func.remove(name_func);
        auto name_text = doc.createTextNode(name_func);
        name.appendChild(name_text);
        auto ret_val_text = doc.createTextNode(func);
        signature.appendChild(ret_val_text);
        methods.appendChild(method);
    }

    for (const auto &i : data.realized_methods)
    {
        auto method = doc.createElement("method");
        auto type = doc.createElement("type");
        auto name = doc.createElement("name");
        auto signature = doc.createElement("signature");
        method.appendChild(type);
        method.appendChild(name);
        method.appendChild(signature);
        auto type_text = doc.createTextNode("realized");
        type.appendChild(type_text);
        // here we divided function on name and signature
        QString func = i.arg('/');
        QString name_func = func.section('/', 1, 1).section('(', 0, 0);
        func.remove('/');
        func.remove(name_func);
        auto name_text = doc.createTextNode(name_func);
        name.appendChild(name_text);
        auto ret_val_text = doc.createTextNode(func);
        signature.appendChild(ret_val_text);
        methods.appendChild(method);
    }

    ::QFile file {
    destination.absolutePath() + QDir::separator() + class_name.section('<', 0, 0).section("::", -1) + ".xml"
    };
    if (file.open(::QIODevice::WriteOnly | ::QIODevice::Text))
    {
        ::QTextStream stream {&file};
        stream << doc.toString();
        file.close();
        return file.fileName();
    }

    return "";
}
