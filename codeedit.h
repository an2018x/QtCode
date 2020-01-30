#ifndef CODEEDIT_H
#define CODEEDIT_H
#include <QPlainTextEdit>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QTextBlock>
#include <QFile>
#include <QFileDialog>
#include <QCompleter>
#include <QListWidget>
#include <QListWidgetItem>
#include "completelistwidget.h"

class LineNumberArea;
class CodeEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    QString codeName;
    QString fileName;
    QFile *myFile;
    CodeEdit(QWidget *parent = 0);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setUpCompleteList();
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    LineNumberArea *lineNumberArea;
    QStringList completeList;//储存自动填充的关键字
    CompleteListWidget *completeWidget;
    QString getWordOfCursor();
    int completeState;
    int getCompleteWidgetX();
public slots:
    void updateLineNumberArea(QRect,int);
    void highLightCurrentLine();
    void showCompleteWidget();
};

#endif // CODEEDIT_H
class LineNumberArea:public QWidget
{
    Q_OBJECT
public:

    LineNumberArea(CodeEdit *editor):QWidget(editor){
        this->resize(40,1500);
        this->move(0,0);
        codeEdit=editor;
    }
    void paintEvent(QPaintEvent *event){
        //qDebug()<<"no";
        codeEdit->lineNumberAreaPaintEvent(event);
    }
private:
    CodeEdit *codeEdit;


};
enum CompleteState{
  Ignore=0,
  Showing=1,
  Hide=2
};
