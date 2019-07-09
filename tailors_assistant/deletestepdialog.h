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

#ifndef DELETESTEPDIALOG_H
#define DELETESTEPDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteStepDialog;
}

class DeleteStepDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteStepDialog(QWidget *parent = 0);
    ~DeleteStepDialog();

private:
    Ui::DeleteStepDialog *ui;
};

#endif // DELETESTEPDIALOG_H
