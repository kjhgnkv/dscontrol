// edit_component_dialog.hpp

#pragma once

#include "component_creator.hpp"

#include <QDialog>
#include <QDir>
#include <QString>

namespace Ui
{
class edit_component_dialog;
}
class interfaces_select_dialog;

class edit_component_dialog : public QDialog
{
Q_OBJECT
public:
    /**\param path_to_generate folder, where will generated new project for
     * component*/
    edit_component_dialog(const QDir &path_to_generate, QWidget* parent = nullptr);
    ~edit_component_dialog();
    /**\return output CMakeLists.txt with path to it.
     * \brief start modal dialog window
     * */
    QString create_component();

private slots:
    void save_parameters();

private:
    Ui::edit_component_dialog* ui_;
    ::QString GUID_;
    ::QString version_;
    component_creator all_interfaces_;
    component_creator creator_;
    interfaces_select_dialog* select_dialog_;
    QDir path_;
    ::QString build_project_file_;

    // QWidget interface
protected:
    void changeEvent(QEvent* event);
};
