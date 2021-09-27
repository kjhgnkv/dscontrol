#include "UiHighlighter.h"

struct HighlightingRule
{
    QRegularExpression pattern_;
    QTextCharFormat format_;
};

UiHighlighter::UiHighlighter(QTextDocument* parent)
: QSyntaxHighlighter {parent}
{
    keywordFormat_.setForeground(Qt::yellow);
    keywordFormat_.setFontWeight(QFont::Bold);

    const QString keywordPatterns[] = {QStringLiteral("<xml>"), QStringLiteral("</xml>")};

    for (const QString &pattern : keywordPatterns)
    {
        auto rule = new HighlightingRule;
        rule->pattern_ = QRegularExpression(pattern);
        rule->format_ = keywordFormat_;
        highlightingRules_.append(rule);
    }
}

void UiHighlighter::highlightBlock(const QString &text)
{
    for (auto rule : highlightingRules_)
    {
        auto matchIterator = rule->pattern_.globalMatch(text);
        while (matchIterator.hasNext())
        {
            auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule->format_);
        }
    }
}
