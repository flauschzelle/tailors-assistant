#-------------------------------------------------
#
# Project created by QtCreator 2019-06-15T00:14:50
#
#-------------------------------------------------

#    Tailor's Assistant - a tool to help with estimating work times as a tailor
#    Copyright (C) 2019  by Kirstin Rohwer
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#    For contact information, see the author's website <http://metakiki.net>.

QT       += core gui
QT       += sql #added manually for sql support

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tailors_assistant
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    workpiece.cpp \
    step.cpp \
    workpieceselector.cpp \
    piecetablemodel.cpp \
    deletepiecedialog.cpp \
    deletestepdialog.cpp \
    steptablemodel.cpp \
    filepathsettingsdialog.cpp \
    aboutdialog.cpp \
    deletepicturedialog.cpp

HEADERS += \
        mainwindow.h \
    workpiece.h \
    step.h \
    workpieceselector.h \
    piecetablemodel.h \
    general.h \
    deletepiecedialog.h \
    deletestepdialog.h \
    steptablemodel.h \
    filepathsettingsdialog.h \
    aboutdialog.h \
    deletepicturedialog.h

FORMS += \
        mainwindow.ui \
    workpieceselector.ui \
    deletepiecedialog.ui \
    deletestepdialog.ui \
    filepathsettingsdialog.ui \
    aboutdialog.ui \
    deletepicturedialog.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
