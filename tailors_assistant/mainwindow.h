#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

enum InputMode { record, offer };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setInputMode(InputMode mode);

private:
    Ui::MainWindow *ui;

    InputMode mode;
};

#endif // MAINWINDOW_H
