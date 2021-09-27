#ifndef __{ { wnd_class_name } }_H__
    #define __{{wnd_class_name}}_H__

    #include "ICenterWindow.h"

namespace Ui
{
class
{
    {
        wnd_class_name
    }
};
}

/** center window provided by {{plugin_name}} */
class
{
    {
        wnd_class_name
    }
}
: public ICenterWindow
{
Q_OBJECT

public:
explicit {
{
wnd_class_name}}(
QWidget* parent = 0
);
~{
{
wnd_class_name}}();

// Interfaces
virtual void setTitle(const QString &aTitle);/**< set title, obsolete */
virtual QString getTitle() const;/**< get title */
virtual QString getChineseTitle() const;/**< get display title */
virtual QString getIconFilename();/**< get file name of icon */
virtual void setIconFilename(const QString &aIconFilename);/**< set icon file name, obsolete */
virtual bool isActive();/**< whether active, always true, obsolete */
virtual PluginMenuView* getPluginMenuView() const;/**< get related plugin menu view for ribbon, obsolete */

private:
Ui::{
{
wnd_class_name}} *
ui;
};

#endif // __{{wnd_class_name}}_H__
