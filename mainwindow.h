#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QFile>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include "codeedit.h"
#include <QPlainTextEdit>
#include <QMessageBox>
#include "highlighter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *);
    void setUpHighlighter();
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;
    QVBoxLayout *layoutV;
    QHBoxLayout *layoutH;
    QMenuBar *myMenuBar;
    int width;
    int height;
    QMenu *file;
    QMenu *edit;
    QMenu *select;
    QMenu *view;
    QMenu *go;
    QMenu *debug;
    QMenu *terminal;
    QMenu *help;
    QFile *qssFile;
    QPushButton *btn_1;
    QPushButton *btn_2;
    QPushButton *btn_3;
    QPushButton *btn_4;
    QPushButton *btn_5;
    QPushButton *file_1;
    CodeEdit *codeArea;
    QPlainTextEdit *editor;
    QAction *actionNewWindow;
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionOpenDir;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionPast;
    QAction *actionCopy;
    QAction *actionFind;
    QAction *actionReplace;
    QAction *actionSelectAll;
    QAction *actionSave;
    QAction *actionSaveAs;
public slots:
    void openFile();
    void saveFile();

};

#endif // MAINWINDOW_H
