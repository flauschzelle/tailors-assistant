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

#include "deletepiecedialog.h"
#include "ui_deletepiecedialog.h"

DeletePieceDialog::DeletePieceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeletePieceDialog)
{
    ui->setupUi(this);
}

DeletePieceDialog::~DeletePieceDialog()
{
    delete ui;
}

void DeletePieceDialog::setMode(PieceStatusMode mode)
{
    this->mode = mode;
    if (mode == record)
    {
        ui->label->setText("Aktuelles Werkstück wirklich löschen?");
    }
    if (mode == offer)
    {
        ui->label->setText("Aktuelles Angebot wirklich löschen?");
    }
}
