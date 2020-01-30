#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QColor(199,146,234));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    QStringList varPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b"<<"\\busing\\b"<<"\\bconstexpr\\b"
                    <<"\\bsizeof\\b"<<"\\bif\\b"<<"\\bfor\\b"<<"\\bforeach\\b"<<"\\bwhile\\b"<<"\\bdo\\b"<<"\\bcase\\b"
                    <<"\\bbreak\\b"<<"\\bcontinue\\b"<<"\\btemplate\\b"<<"\\bdelete\\b"<<"\\bnew\\b"
                    <<"\\bdefault\\b"<<"\\btry\\b"<<"\\breturn\\b"<<"\\bthrow\\b"<<"\\bcatch\\b"<<"\\bgoto\\b"<<"\\belse\\b"
                    <<"\\bextren\\b"<<"\\bthis\\b"<<"\\bswitch\\b"<<"#include\\b"<<"#define\\b"<<"#ifndef\\b"<<"#if\\b"<<"#endif\\b"
                   <<"#error\\b"<<"#elif\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    //数字
    digitFormat.setForeground(QColor(247,140,100));
    rule.pattern=QRegularExpression("\\W\\d+");
    rule.format=digitFormat;
    highlightingRules.append(rule);

    //字符
    charFormat.setForeground(QColor(130,170,255));
    rule.pattern=QRegularExpression("\\(|\\)|\\{|\\}|!|=|-|\\+|\\\\|\\[|\\]|-|>|\\.|\\&|;");
    rule.format=charFormat;
    highlightingRules.append(rule);
    //类 规则
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("(?<=class\\s)\\w*");
    rule.format = classFormat;
    highlightingRules.append(rule);

    //头文件包含规则
    quotationFormat.setForeground(QColor(145,219,141));
    rule.pattern = QRegularExpression("(?<=#include\\s)(<.*>)|(?<=#include)(<.*>)|(?<=#include\\s)(\".*\")|(?<=#include)(\".*\")|\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    //函数 规则
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(130,170,255));
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
    //注释代码 规则
    singleLineCommentFormat.setForeground(QColor(120,120,120));
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
    multiLineCommentFormat.setForeground(QColor(120,120,120));

    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);
    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
