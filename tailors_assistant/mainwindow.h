#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "general.h"
#include "workpiece.h"
#include "workpieceselector.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setInputMode(PieceStatusMode mode);

public slots:
    void newPiece();
    void newOffer();

    void openPieceSelector();
    void openOfferSelector();

    void getWorkpieceFromSelector();
    void cleanUpSelector();

private:
    Ui::MainWindow *ui;

    PieceStatusMode mode;

    QStandardItemModel *test_model;

    WorkPiece *currentPiece;

    WorkPieceSelector *selector;

    QSqlDatabase db;

    QString databasePath;
};

#endif // MAINWINDOW_H
