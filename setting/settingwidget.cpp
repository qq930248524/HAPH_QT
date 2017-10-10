#include "settingwidget.h"
#include "DMSNavigation.h"
#include <QTextEdit>
SettingWidget::SettingWidget(QWidget *parent) : QWidget(parent)
{
    DMSNavigation navi;
    navi.resize(600, 50);
    navi.show();

    navi.addTab(new QTextEdit("The first."), "Text 1");
    navi.addTab(new QTextEdit("The second."), "Text 2");
    navi.addTab(new QTextEdit("The third."), "Text 3");

}
