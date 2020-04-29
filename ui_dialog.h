/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLabel *loglab;
    QLabel *contlab;
    QTextBrowser *logShow;
    QTextBrowser *mailShow;
    QLabel *img;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(792, 450);
        loglab = new QLabel(Dialog);
        loglab->setObjectName(QString::fromUtf8("loglab"));
        loglab->setGeometry(QRect(20, 10, 271, 31));
        QFont font;
        font.setFamily(QString::fromUtf8("Kaiti SC"));
        font.setPointSize(18);
        loglab->setFont(font);
        contlab = new QLabel(Dialog);
        contlab->setObjectName(QString::fromUtf8("contlab"));
        contlab->setGeometry(QRect(410, 10, 211, 31));
        contlab->setFont(font);
        logShow = new QTextBrowser(Dialog);
        logShow->setObjectName(QString::fromUtf8("logShow"));
        logShow->setGeometry(QRect(20, 40, 361, 181));
        mailShow = new QTextBrowser(Dialog);
        mailShow->setObjectName(QString::fromUtf8("mailShow"));
        mailShow->setGeometry(QRect(410, 40, 361, 371));
        img = new QLabel(Dialog);
        img->setObjectName(QString::fromUtf8("img"));
        img->setGeometry(QRect(20, 240, 361, 171));
        img->setTextFormat(Qt::AutoText);
        img->setAlignment(Qt::AlignCenter);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "\351\202\256\344\273\266\346\234\215\345\212\241\345\231\250", nullptr));
        loglab->setText(QCoreApplication::translate("Dialog", "SMTP\346\234\215\345\212\241\345\231\250\346\227\245\345\277\227", nullptr));
        contlab->setText(QCoreApplication::translate("Dialog", "\346\216\245\346\224\266\347\232\204\351\202\256\344\273\266\345\206\205\345\256\271", nullptr));
        img->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
