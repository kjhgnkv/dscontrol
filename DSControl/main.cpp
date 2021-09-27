#include "manager/ControllManager.hpp"
#include "common/globals.h"
#include "ui/platform/Platform/Platform.hpp"

#include <QApplication>
#include <QDir>
#include <QPainter>
#include <QSplashScreen>
#include <QString>
#include <QTextStream>
#include <QTime>

QString logPath;
static int logCounter;

/*!
 \brief Log handler
 \param type - log message type
 \param context - message attributes
 \param msg - message body
 \details Handler to logging all messaging into log file $LOG_FILE
 */

void platformMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const QtMsgType LOG_LEVEL = QtDebugMsg;
    if (type < LOG_LEVEL)
    {
        return;
    }
    QFile file(logPath);
    if (file.open(QIODevice::Append))
    {
        auto dt = QDateTime::currentDateTime();
        QString messageString = "------" + QString::number(
        logCounter++) + "[" + dt.date().toString() + " " + dt.time().toString() + "]";
        switch (type)
        {
            case QtDebugMsg:
                messageString.append("<debug>");
                break;
            case QtInfoMsg:
                messageString.append("<info>");
                break;
            case QtWarningMsg:
                messageString.append("<warning>");
                break;
            case QtCriticalMsg:
                messageString.append("<critical>");
                break;
            case QtFatalMsg:
                messageString.append("<fatal>");
                break;
        }
        messageString.append("\t\t\t" + msg + "\n");
        messageString.append(QString("%1 %2 \n%3\n").arg(context.file).arg(context.line).arg(context.function));
        QTextStream stream(&file);
        stream << messageString;
    }
    file.close();
}

inline void initLog()
{
    auto dt = QDateTime::currentDateTime();
    logPath = QDir::currentPath() + "/logs/" + "DS_full_" + dt.date().toString("dd.MM.yyyy") + "_" + dt.time().toString(
    "hh.mm.ss") + ".log";
    QDir dir = QDir::currentPath() + "/logs";

    if (!dir.exists())
    {
        qDebug() << "dir.exists";
        dir.mkpath(dir.path());
    }

    qInstallMessageHandler(platformMessageHandler);
}

/*!
 \brief Start splash window runner
 \param app - base application
 \param WINDOW_ICON - window QPixmap (CETC logo)
 \param PLATFORM_VERSION - QString with platform version (show on splash)
 \param LOAD_TIME_MSEC - load bar fill time in miliseconds
 \details Function start splash window when application starting. Splash
 \window consist application title, version and CETC logo
 */
void startLoadWindow(QApplication* app, QPixmap WindowIcon, int LoadTimeMsec)
{
    QPainter painter;
    painter.begin(&WindowIcon);
    painter.setPen(Qt::black);

    QSplashScreen splashScreen(WindowIcon);
    splashScreen.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    splashScreen.setEnabled(false);

    splashScreen.show();

    QTime time;
    time.start();
    while (time.elapsed() <= LoadTimeMsec)
    {
        app->processEvents();
    }

    splashScreen.close();
}

void setTextCodec(const QString &encodingName)
{
    QTextCodec* codec = QTextCodec::codecForName(encodingName.toStdString().c_str());
    QTextCodec::setCodecForLocale(codec);
}

int main(int argc, char* argv[])
{
    qDebug() << "start main";
    QString Title(PROGRAMM_NAME);
    QString Version(PROGRAMM_VERSION);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(Title);
    // int          LoadTimeMsec = 500;

    #ifdef NDEBUG
    initLog();
    #endif

    setTextCodec("UTF-8");

    qDebug() << "Initialize application";

    // set stylesheet
    QApplication application(argc, argv);
    application.setStyleSheet(R"(

QWidget {
    font-family: Source Sans Pro;
}

QMainWindow{
     background: #605f6d;
}

QMainWindow::separator {
     background: #605f6d;
     width: 10px; /* when vertical */
     height: 10px; /* when horizontal */
 }
QDialog{
    background: #33333A;
}
QLabel{
    color: white;
}
QCheckBox{
    color: white;
}
QGroupBox{
    color: white;
}
QRadioButton{
    color: white;
}
QTableView
{
background: #484853;
}
QHeaderView
{
    font-size: 15px;
    font-style: normal;
    font-weight: normal;
}
QTableView::item
{
    background: #605F6E;
    font-size: 14px;
    color: #FFFFFF;

}
QTableView::item:hover
{
    background: #595866;
}
QTableView::item:selected
{
  background: #7B59FF;
}
QTreeView
{
    background: #484853;
    color: #FFFFFF
}

QLineEdit{
    background: #3D3D46;
    border: 1px solid #AAAAC5;
    border-radius: 5px;
    font-size: 14px;
    color: #FFFFFF;
}

QComboBox
{
    selection-background-color: #605F6E;
    selection-color: #FFFFFF;
    color: white;
    font-size: 14px;
    background-color: red;
    border: 1px solid #33333A;
}

QComboBox:on
{
    color: white;
    background-color: #605F6E;
    selection-background-color: #605F6E;
    border: 1px solid #33333A;
}

QComboBox:!on
{
    color: white;
    background-color: #605F6E;

border: 1px solid #33333A;
}

QComboBox QAbstractItemView
{
    color: #FFFFFF;
    font-size: 14px;
    background-color: #252434;
    border: 1px solid #33333A;
}

QComboBox::drop-down
{
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 15px;
    color: white;
    border-left-width: 0px;
    border-left-color: darkgray;
    border-left-style: solid; /* just a single line */
    border-top-right-radius: 3px; /* same radius as the QComboBox */
    border-bottom-right-radius: 3px;
    padding-left: 10px;
}

QComboBox::down-arrow, QSpinBox::down-arrow, QTimeEdit::down-arrow, QDateEdit::down-arrow
{
    image: url(:/images/resources/images/down-arrow.png);
    width: 8px;
    height: 4px;
}

#centralwidget {
    background: #605F6E;
}
#startPage
{
    background: #222033;
}
#projectPage
{
    background: #605F6E;
}
QGraphicsView{
                              border:none;
                              }

QScrollBar:horizontal {
    /*border: 1px solid #313131;*/
    background: #3f3f47;
    height: 11px;
    margin: 0px 20px 0 20px;
}
QScrollBar::handle:horizontal {
    background: #545569;
    width: 44px;
    height: 11px;
    /*border: 1px solid #313131;*/
    border-radius: 5px;
}
QScrollBar::add-line:horizontal {
    /*border: 1px solid 313131;*/
    background: #3f3f47;
    width: 20px;
    subcontrol-position: right;
    subcontrol-origin: margin;
}

QScrollBar::sub-line:horizontal {
    /*border: 1px solid 313131;*/
    background: #3f3f47;
    width: 20px;
    subcontrol-position: left;
    subcontrol-origin: margin;
}

QScrollBar::sub-page:horizontal, QScrollBar::add-page:horizontal  {
    background: #3f3f47;
}

QScrollBar::left-arrow:horizontal {
    image: url(:/toolbar/mode/left);
}

QScrollBar::right-arrow:horizontal {
    image: url(:/toolbar/mode/right);
}

QScrollBar:vertical {
    /*border: 1px solid #313131;*/
    background: #3f3f47;
    width: 11px;
    margin: 20px 0 20px 0;
}
QScrollBar::handle:vertical {
    background: #545569;
    width: 11px;
    min-height: 20px;
    /*border: 1px solid #313131;*/
    border-radius: 5px;
}
QScrollBar::add-line:vertical {
    /*border: 1px solid 313131;*/
    background: #3f3f47;
    height: 20px;
    subcontrol-position: bottom;
    subcontrol-origin: margin;
}

QScrollBar::sub-line:vertical {
    /*border: 1px solid 313131;*/
    background: #3f3f47;
    height: 20px;
    subcontrol-position: top;
    subcontrol-origin: margin;
}

QScrollBar::sub-page:vertical, QScrollBar::add-page:vertical  {
    background: #3f3f47;
}

QScrollBar::up-arrow:vertical {
    image: url(:/toolbar/mode/up);
}

QScrollBar::down-arrow:vertical {
    image: url(:/toolbar/mode/down);
}

/*QFrame {background: #605f6d;}*/

/*QTabWidget::pane {
     border-top: 1px solid #313131;
 }*/

 /* Style the tab using the tab sub-control. Note that
     it reads QTabBar _not_ QTabWidget */
 QTabBar::tab {
     background: #605f6d;
     color: white;
     /*border: 1px solid #313131;
     border-bottom-color: #313131;*/
     border-top-left-radius: 4px;
     border-top-right-radius: 4px;
     min-width: 8px;
     padding-left: 8px;
     padding-right: 24px;
     padding-top: 2px;
     padding-bottom: 2px;
 }

 QTabBar::tab:selected, QTabBar::tab:hover {
     background: #3f3f47;
 }

QTabBar::close-button {
    image: url(:toolbar/mode/close);
}

 QTabBar::tab:selected {
     border-color: #313131;
     /*border-bottom-color: #313131;*/ /* same as pane color */
 }

 QTabBar::tab:!selected {
     margin-top: 2px; /* make non-selected tabs look smaller */
    background: #494953;
 }


QHeaderView::section {
background-color: #252434;

color: #FFFFFF;

}

QDockWidget {
    border: 1px solid #313131;
    color: white;
    font-size: 10pt;
    font-weight: bold;
    background: #605f6d;
    padding-top: 10px;
    titlebar-close-icon: url(:toolbar/mode/close);
    titlebar-normal-icon: url(:toolbar/mode/float);
 }

 QDockWidget::title {
     text-align: left;
     background: #605f6d;
     padding-left: 4px;
     padding-top: 10px;
     font-size : 16pt;
 }

QDockWidget::close-button, QDockWidget::float-button {
    border: none;
    background: transparent;
    padding-top: 0px;
    icon-size: 16px;
}

 /*QDockWidget::float-button {
    image: url(:toolbar/mode/float);
}

QDockWidget::close-button {
    image: url(:toolbar/mode/close);
}*/

QDockWidget::float-button:hover {
    image: url(:toolbar/mode/float);
}

QDockWidget::close-button:hover {
    image: url(:toolbar/mode/close);
}

QToolBar {
     background: #52525e;
     border: 1px solid #313131;
     spacing: 3px; /* spacing between items in the tool bar */
 }

QMenuBar {
    background-color: #494953;
}

QMenuBar::item {
    spacing: 10px;
    padding: 1px 10px;
    background: transparent;
    border-radius: 4px;
    color: white;
}

QMenuBar::item:selected {
    background: #a8a8a8;
}

QMenuBar::item:pressed {
    background: #888888;
}

QSpinBox{
    background-color: transparent;
    color: #ffffff;
    border: 1px solid #AAAAC5;
    border-radius: 5px;
    height: 20px;
}
QSpinBox::up-button{
    border-left: 1px solid #AAAAC5;
    width: 16px;
    height: 10px;
}
QSpinBox::up-arrow{
    image: url(:/toolbar/upArrow.png);
    width: 16px;
    height: 9px;
}
QSpinBox::down-button{
    border-left: 1px solid #AAAAC5;
    width: 16px;
    height: 10px;
}
QSpinBox::down-arrow{
    image: url(:/toolbar/downArrow.png);
    width: 16px;
    height: 9px;
    top: 1px;
}

    QtNodes--NodeStyle {
      qproperty-normalBackgroundColor : "gray";
      qproperty-hoveredBackgroundColor : "green";
      qproperty-pressedBackgroundColor : "cyan";
      qproperty-selectedBackgroundColor : "brown";

      qproperty-portFontColor : "green";
      qproperty-portFontColorFaded : "cyan";
      qproperty-titleBarBackgroundColor : "#4A36B9";
      qproperty-headerFont : "Tahoma,12,-1,5,75,0,0,0,0,0,Bold";
      qproperty-normalBackgroundColor : "#605F6D";

      qproperty-titleBarMargins : "5, 5, 5, 5";

    qproperty-portBarBackgroundColor : "#605f6d";
qproperty-portFontColor : "white";
      qproperty-portFontColorFaded : "white";
    }

    QtNodes--ConnectionStyle {
      qproperty-useDataDefinedColors : true;
    }
  )");

    QPixmap WindowIcon {PLATFORM_ICON};
    application.setWindowIcon(WindowIcon);
    startLoadWindow(&application, QPixmap {":/images/resources/images/splash_v2.png"}, 2000);

    dscontrol::Platform mainWindow;

    // TODO Deploy
    dscontrol::ControllManager manager {&mainWindow};
    mainWindow.setControllManager(&manager);
    mainWindow.setWindowTitle(Title);

    application.setStyleSheet(application.styleSheet() + R"(
                                QMenu {
                                    margin: 10px;
                                    background-color: #605F6E;
                                }
                                QMenu::item:selected {
                                    background-color: #7B59FF;
                                }
                                QMenu::item::text {
                                    color: #FFFFFF;
                                }
                                QMenu::item:disabled {
                                    color: #808080;
                                }
                                QMenu::icon {
                                    padding-left: 10px;
                                }
                                QMenu::separator {
                                    height: 1px;
                                    background-color: #484853;
                                }

                              )");

    mainWindow.showMaximized();

    return application.exec();
}
