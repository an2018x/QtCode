#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->resize(1000,670);
//    this->setUpHighlighter();

    this->setWindowTitle("Qt Code");
    width=this->geometry().width();
    height=this->geometry().height();
    this->setWindowIcon(QPixmap(":/img/logo.png"));

    myMenuBar=new QMenuBar(this);
    //增加菜单栏

    file=myMenuBar->addMenu(tr("文件(&F)"));

    //file->setFont(QFont("等线",14));
    edit=myMenuBar->addMenu(tr("编辑(&E)"));
    select=myMenuBar->addMenu(tr("选择(&S)"));
    view=myMenuBar->addMenu(tr("查看(&V)"));
    go=myMenuBar->addMenu(tr("转到(&G)"));
    debug=myMenuBar->addMenu(tr("调试(&D)"));
    terminal=myMenuBar->addMenu(tr("终端(&T)"));
    help=myMenuBar->addMenu(tr("帮助(&H)"));
    //细分菜单栏
    actionNew=file->addAction(tr("新建文件"));
    actionNewWindow=file->addAction(tr("新建窗口"));
    actionNew->setShortcut(QKeySequence("Ctrl+N"));
    actionNewWindow->setShortcut(QKeySequence("Ctrl+Shift+N"));
    file->addSeparator();
    actionOpen=file->addAction(tr("打开文件"));
    actionOpenDir=file->addAction(tr("打开文件夹"));
    actionOpen->setShortcut(QKeySequence("Ctrl+O"));
    actionOpenDir->setShortcut(QKeySequence("Ctrl+Shift+O"));
    actionSave=file->addAction(tr("保存"));
    actionSave->setShortcut(QKeySequence("Ctrl+S"));
    file->addSeparator();

    actionUndo=edit->addAction(tr("撤销"));
    actionRedo=edit->addAction(tr("恢复"));
    actionCut=edit->addAction(tr("剪切"));
    actionCopy=edit->addAction(tr("复制"));
    actionPast=edit->addAction(tr("粘贴"));
    actionUndo->setShortcut(QKeySequence("Ctrl+Z"));
    actionRedo->setShortcut(QKeySequence("Ctrl+Y"));
    actionCut->setShortcut(QKeySequence("Ctrl+X"));
    actionCopy->setShortcut(QKeySequence("Ctrl+C"));
    actionPast->setShortcut(QKeySequence("Ctrl+V"));


    //按钮点击
    connect(this->actionOpen,&QAction::triggered,this,&MainWindow::openFile);
    connect(this->actionSave,&QAction::triggered,this,&MainWindow::saveFile);

    //侧边栏
    btn_1=new QPushButton(this);
    btn_1->setStyleSheet("QPushButton{border-image:url(:/img/file_1.png);}QPushButton:hover{border-image:url(:/img/file.png);}");
    btn_1->resize(33,33);
    btn_1->move(14,45);
    btn_2=new QPushButton(this);
    btn_2->setStyleSheet("QPushButton{border-image:url(:/img/search_1.png);}QPushButton:hover{border-image:url(:/img/search.png);}");
    btn_2->resize(33,33);
    btn_2->move(14,95);
    btn_3=new QPushButton(this);
    btn_3->setStyleSheet("QPushButton{border-image:url(:/img/git_1.png);}QPushButton:hover{border-image:url(:/img/git.png);}");
    btn_3->resize(33,33);
    btn_3->move(14,155);
    btn_4=new QPushButton(this);
    btn_4->setStyleSheet("QPushButton{border-image:url(:/img/bug_1.png);}QPushButton:hover{border-image:url(:/img/bug.png);}");
    btn_4->resize(33,33);
    btn_4->move(14,215);
    btn_5=new QPushButton(this);
    btn_5->setStyleSheet("QPushButton{border-image:url(:/img/app_1.png);}QPushButton:hover{border-image:url(:/img/app.png);}");
    btn_5->resize(33,33);
    btn_5->move(14,275);
    file_1=new QPushButton(this);
    file_1->setStyleSheet(tr("QPushButton{border:0px;border-color:rgb(33,33,33);border-bottom:2px solid blue;background-color:rgb(33,33,33);color:white;font:bold;}"));
    file_1->setText("Untitled");
    file_1->setFont(QFont("等线",10));
    file_1->resize(140,30);
    file_1->move(60,35);
    //代码文本编辑区
    codeArea=new CodeEdit(this);
    codeArea->resize(geometry().width()-60,geometry().height()-65);
    codeArea->move(60,65);
    highlighter=new Highlighter(codeArea->document());

    //qss样式文件读取
    qssFile = new QFile(":/stylesheet.qss", this);
    // 只读方式打开该文件
    qssFile->open(QFile::ReadOnly);
    // 读取文件全部内容，使用tr()函数将其转换为QString类型
    QString styleSheet = tr(qssFile->readAll());
    // 为QApplication设置样式表
    qApp->setStyleSheet(styleSheet);
    qssFile->close();
    myMenuBar->setGeometry(QRect(0,0,1920,35));
    file->resize(38,35);

}

void MainWindow::setUpHighlighter(){
//  QFont font;
//  font.setFamily("Courier");
//  font.setFixedPitch(true);
//  //font.setPointSize(20);
//  this->codeArea->setFont(font);
//  this->codeArea->setTabStopWidth(fontMetrics().width(QLatin1Char('9'))*4);
  highlighter=new Highlighter(this->codeArea->document());
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush(QColor(33,33,33));
    painter.setBrush(brush);
    painter.fillRect(QRect(0,35,60,1500),QBrush(QColor(33,33,33)));
    painter.fillRect(QRect(60,35,2000,30),QBrush(QColor(33,33,33)));
}

void MainWindow::resizeEvent(QResizeEvent *){
    this->codeArea->resize(this->geometry().width()-60,this->geometry().height()-65);
}


/////////////slots///////////////////
void MainWindow::openFile(){
    QString filename=QFileDialog::getOpenFileName(this,"打开文件",".","Text(*.cpp *.h *.txt)");
    this->codeArea->fileName=filename;
    if(filename.isEmpty())
        return;
    QStringList list=filename.split("/");
    QString title=list.at(list.length()-1);
    this->file_1->setText(title);
    //   QFileInfo info(filename);
    //   QString title=info.fileName();
    this->codeArea->myFile->setFileName(filename);
    bool ret=this->codeArea->myFile->open(QIODevice::ReadOnly);
    if(!ret)
    {
        QMessageBox::warning(this,"文件失败","打开失败");
    }
    QTextStream stream(this->codeArea->myFile);
    this->codeArea->codeName="UTF-8";
    stream.setCodec(this->codeArea->codeName.toLocal8Bit().data());
    this->codeArea->setPlainText(stream.readAll());
    this->codeArea->myFile->close();
}
void MainWindow::saveFile(){
    if(this->codeArea->fileName.isEmpty())
    {
        QString filename=QFileDialog::getSaveFileName(this,"保存",".","*.txt *.cpp *.h");
        this->codeArea->fileName=filename;
    }

    this->codeArea->myFile->setFileName(this->codeArea->fileName);
    this->codeArea->myFile->open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream stream(this->codeArea->myFile);
    stream.setCodec(this->codeArea->codeName.toLocal8Bit().data());
    stream<<this->codeArea->toPlainText();
    stream.flush();
    this->codeArea->myFile->close();
    if(!this->codeArea->fileName.isEmpty())
        QMessageBox::information(this,"成功","保存成功！");
}
////////////slots/////////////////




MainWindow::~MainWindow()
{
    delete ui;
    delete highlighter;

}
