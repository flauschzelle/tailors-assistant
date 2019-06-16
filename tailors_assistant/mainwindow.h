#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "general.h"
#include "workpiece.h"

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

private:
    Ui::MainWindow *ui;

    PieceStatusMode mode;

    QStandardItemModel *test_model;

    WorkPiece currentPiece;


};

#endif // MAINWINDOW_H
