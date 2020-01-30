#include "codeedit.h"
#include <QDebug>
#include <QCompleter>


CodeEdit::CodeEdit(QWidget *parent): QPlainTextEdit(parent)
{
    this->myFile=new QFile(this);
    this->lineNumberArea=new LineNumberArea(this);
    this->setFont(QFont(tr("Consolas"), 14));
    this->setStyleSheet("background-color:rgb(33,33,33);color:rgb(240,180,40);border:0px;");

    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highLightCurrentLine()));
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(showCompleteWidget()));
    highLightCurrentLine();
    setViewportMargins(40, 0, 0, 0);
    QFontMetrics metrics(this->font());
    this->setTabStopWidth(4 * metrics.width(' '));

    setUpCompleteList();
    completeWidget= new CompleteListWidget(this);
    completeWidget->hide();
    completeWidget->setMaximumHeight(14*5);
    completeState=CompleteState::Hide;
}

void CodeEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    //qDebug()<<"yes";
    QPainter painter(lineNumberArea);
    painter.fillRect(QRect(0,0,40,1560),QColor(34,39,49));     //画一个灰色的矩形底色
    QTextBlock block=firstVisibleBlock();                      //获取第一个可见文本块
    int num=block.blockNumber();                                //获取第一个行号
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    //这个过程可能难以理解，实际上就是对于某一行代码块建立逻辑坐标系，第一行代码块左上角是(0,0)，建议回顾Qt坐标系统
    //qDebug()<<blockBoundingGeometry(block).translated(contentOffset());不理解的画建议用QDebug输出一下它的返回值，看看它代表什么含义
    int bottom = top + (int) blockBoundingRect(block).height(); //就是代码块矩形底下的线，这里换算为逻辑纵坐标，也就是加上一行代码块的高度
    //qDebug()<<(int) blockBoundingRect(block).height();
    while (block.isValid() && top <= event->rect().bottom()) { //当这个代码块合法且代码块矩形上面线坐标小于等于下面线坐标，当文档结束时，则不符合这两个条件，想想为什么
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(num + 1);      //画出行号
            painter.setPen(Qt::lightGray);
            painter.drawText(0, top, 40, fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        block = block.next();           //代码块向后移一行
        top = bottom;                       //同样当前坐标更新
        //qDebug()<<top;
        bottom = top + (int) blockBoundingRect(block).height();
        ++num;
    }
}

void CodeEdit::highLightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(66,66,66);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEdit::showCompleteWidget()
{
    if(completeState==CompleteState::Ignore)return;//忽略光标和文本变化的响应,避免陷入事件死循环和互相钳制
    completeWidget->hide();
    completeState=CompleteState::Hide;
    QString word=this->getWordOfCursor();
    completeWidget->clear();
    if(!word.isEmpty()){//光标所在单词是不是合法(能不能联想)
        int maxSize=0;
        QMap<QString,int> distance;
        vector<QString> itemList;
        foreach(const QString &temp,completeList){
            if(temp.contains(word)){
                //completeWidget->addItem(new QListWidgetItem(temp));
                itemList.push_back(temp);
                distance[temp]=CompleteListWidget::ldistance(temp.toStdString(),word.toStdString());
                if(temp.length()>maxSize)maxSize=temp.length();

            }
        }
        //有没有匹配的字符
        if(itemList.size()>0){//如果有的话
            sort(itemList.begin(),itemList.end(),[&](const QString &s1,const QString &s2)->bool{return distance[s1]<distance[s2]; });
            foreach(const QString& item,itemList){
                QListWidgetItem *pItem=new QListWidgetItem(item);
                pItem->setFont(QFont(tr("Consolas"), 14));
                //pItem->setSizeHint(QSize(70, 24));  //每次改变Item的高度
                completeWidget->addItem(pItem);
            }

            int x=this->getCompleteWidgetX();
            int y=this->cursorRect().y()+fontMetrics().height();

            completeWidget->move(x,y);
            if(completeWidget->count()>5)completeWidget->setFixedHeight(fontMetrics().height()*6);
            else completeWidget->setFixedHeight(fontMetrics().height()*(completeWidget->count()+1));
            completeWidget->setFixedWidth((fontMetrics().width(QLatin1Char('9'))+6)*maxSize);
            completeWidget->show();
            completeState=CompleteState::Showing;
            completeWidget->setCurrentRow(0,QItemSelectionModel::Select);
        }
    }
}



int CodeEdit::lineNumberAreaWidth()
{
    return 40;
}

void CodeEdit::setUpCompleteList()
{
    completeList<< "char" << "class" << "const"
                << "double" << "enum" << "explicit"
                << "friend" << "inline" << "int"
                << "long" << "namespace" << "operator"
                << "private" << "protected" << "public"
                << "short" << "signals" << "signed"
                << "slots" << "static" << "struct"
                << "template" << "typedef" << "typename"
                << "union" << "unsigned" << "virtual"
                << "void" << "volatile" << "bool"<<"using"<<"constexpr"
                <<"sizeof"<<"if"<<"for"<<"foreach"<<"while"<<"do"<<"case"
               <<"break"<<"continue"<<"template"<<"delete"<<"new"
              <<"default"<<"try"<<"return"<<"throw"<<"catch"<<"goto"<<"else"
             <<"extren"<<"this"<<"switch"<<"#include <>"<<"#include \"\""<<"#define"<<"iostream";
}



void CodeEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), 40, cr.height()));
}

void CodeEdit::keyPressEvent(QKeyEvent *event)
{
    //qDebug()<<event->key();
    if(event->modifiers()==Qt::ShiftModifier&&event->key()==40){
        this->insertPlainText(tr("()"));
        this->moveCursor(QTextCursor::PreviousCharacter);
    }
    else if(event->modifiers()==Qt::ShiftModifier&&event->key()==34){
        this->insertPlainText(tr("\"\""));
        this->moveCursor(QTextCursor::PreviousCharacter);
    }
    else if(event->key()==16777235&&completeState==CompleteState::Showing){
        if(completeWidget->currentRow()>0)
            completeWidget->setCurrentRow(completeWidget->currentRow()-1);
    }
    else if(event->key()==16777237&&(completeState==CompleteState::Showing)){
        if(completeWidget->currentRow()<completeWidget->count()-1)
            completeWidget->setCurrentRow(completeWidget->currentRow()+1);
    }
    else if(event->key()==Qt::Key_Return&&(completeState==CompleteState::Showing)){
        QString insertText=completeWidget->currentItem()->text();
        QString word=this->getWordOfCursor();
        completeState=CompleteState::Ignore;
        for(int i=0;i<word.count();++i)
            this->textCursor().deletePreviousChar();
        this->insertPlainText(insertText);
        if(insertText.contains(tr("#include")))
            this->moveCursor(QTextCursor::PreviousCharacter);
        completeState=CompleteState::Hide;
        completeWidget->hide();
    }//*
    else if(event->key()==Qt::Key_Return){//回车下行层级自动缩进功能
        //获得本行的文本
        QString temp=this->document()->findBlockByLineNumber(this->textCursor().blockNumber()).text();
        QPlainTextEdit::keyPressEvent(event);
        if(temp.count()<=0)return;
        //输出回车那一行的前距
        foreach(const QChar &c,temp){
            if(c.isSpace())this->insertPlainText(c);
            else break;
        }
        //如果是for() while() switch() if()则缩进一个tab,一种粗略地做法可能会出错
        if(temp.at(temp.count()-1)==')'&&(temp.contains(tr("for("))||temp.contains(tr("while("))
                                          ||temp.contains(tr("switch("))||temp.contains(tr("if("))))
            this->insertPlainText(tr("\t"));
        //如果是{ 则缩进并补}
        if(temp.at(temp.count()-1)=='{'){
            this->insertPlainText(tr("\t"));
            QTextCursor cursor=this->textCursor();
            int pos=this->textCursor().position();
            this->insertPlainText(tr("\n"));
            foreach(const QChar &c,temp){
                if(c.isSpace())this->insertPlainText(c);
                else break;
            }
            this->insertPlainText(tr("}"));
            cursor.setPosition(pos);
            this->setTextCursor(cursor);//返回中间一行
        }
    }//*/
    else if(event->key()==Qt::Key_Backspace){
        switch(this->document()->characterAt(this->textCursor().position()-1).toLatin1()){
        case '(':
            QPlainTextEdit::keyPressEvent(event);
            if(this->document()->characterAt(this->textCursor().position())==')'){
                this->textCursor().deleteChar();
            }break;
        case '\"':
            QPlainTextEdit::keyPressEvent(event);
            if(this->document()->characterAt(this->textCursor().position())=='\"'){
                this->textCursor().deleteChar();
            }break;
        case '<':
            QPlainTextEdit::keyPressEvent(event);
            if(this->document()->characterAt(this->textCursor().position())=='>'){
                this->textCursor().deleteChar();
            }break;
        default:
            QPlainTextEdit::keyPressEvent(event);
        }
    }
    else{
        QPlainTextEdit::keyPressEvent(event);
        //qDebug()<<event->key();
    }
}

QString CodeEdit::getWordOfCursor()
{
    int pos=this->textCursor().position()-1;
    QVector<QChar> words;
    QString result;
    QChar ch=this->document()->characterAt(pos+1);
    if(ch.isDigit()||ch.isLetter()||ch==' ')return result;
    ch=this->document()->characterAt(pos);
    if(ch==' ')return result;
    while(ch.isDigit()||ch.isLetter()||ch=='_'||ch=='#'){
        words.append(ch);
        pos--;
        ch=this->document()->characterAt(pos);
    }
    for(int i=words.size()-1;i>=0;i--)
        result+=words[i];
    return result;
}

int CodeEdit::getCompleteWidgetX()
{
    QTextCursor cursor=this->textCursor();
    int pos=cursor.position()-1;
    int origianlPos=pos+1;
    QChar ch;
    ch=this->document()->characterAt(pos);
    while((ch.isDigit()||ch.isLetter()||ch=='_'||ch=='#')&&pos>0){
        pos--;
        ch=this->document()->characterAt(pos);
    }
    pos++;
    completeState=CompleteState::Ignore;
    cursor.setPosition(pos);
    this->setTextCursor(cursor);
    int x=this->cursorRect().x()+2*fontMetrics().width(QLatin1Char('9'));
    cursor.setPosition(origianlPos);
    this->setTextCursor(cursor);
    completeState=CompleteState::Hide;
    return x;
}



void CodeEdit::updateLineNumberArea(QRect rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), 40, rect.height());
}
