/*  
    Tailor's Assistant - a tool to help with estimating work times as a tailor
    Copyright (C) 2019  by Kirstin Rohwer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

	For contact information, see the author's website <http://metakiki.net>.
*/

#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include <QDialog>
#include "general.h"

namespace Ui {
class FilePathSettingsDialog;
}

class FilePathSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePathSettingsDialog(QWidget *parent = 0);
    ~FilePathSettingsDialog();

    QString getNewPath();

    void setTextAndPath(const QString& text);
    void setLabelText(const QString& text);
    void setFileDialogTitle(const QString& value);

    void setFileDialogDirPath(const QString& value);

    void setFileDialogTypes(const QString& value);

    void setMode(const FileDialogMode& value);

public slots:
    void setPath(const QString &text);
    void setPathFromFilesystem();

    void setLabelPicture(const QPixmap& picture);
    void setPictureLabelVisibility(bool visible);
private:
    Ui::FilePathSettingsDialog *ui;

    QString path;
    QString fileDialogTitle;
    QString fileDialogDirPath;
    QString fileDialogTypes;

    FileDialogMode mode;
};

#endif // DATABASESETTINGS_H
