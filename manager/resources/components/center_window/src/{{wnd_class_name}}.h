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

/* Interfaces */
virtual void setTitle(const QString &aTitle);
virtual QString getTitle() const;
virtual QString getChineseTitle() const;
virtual QString getIconFilename();
virtual void setIconFilename(const QString &aIconFilename);
virtual bool isActive();
virtual PluginMenuView* getPluginMenuView() const;

private:
Ui::{
{
wnd_class_name}} *
ui;
};

#endif // __{{wnd_class_name}}_H__
