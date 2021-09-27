#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;
struct HighlightingRule;

class UiHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
    UiHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QVector<HighlightingRule*> highlightingRules_;
    QTextCharFormat keywordFormat_;
};
