#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H

#include <QGraphicsItem>
#include <QDebug>
#include <QInputEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QTextCursor>
#include <QPlainTextDocumentLayout>
#include <QTimer>

class GraphicsTextItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    GraphicsTextItem(const QString &text = "please enter text!", QGraphicsItem *parent = nullptr);

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *event) override;                           // 重写 输入法事件（中文）
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;               // 修复 中文输入框的位置不正确
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;     // 字符事件(英文）

private slots:
    void test();
    void calcCursorPos();
    void cursorFlag();

private:
    QString        m_text;
    QTextDocument* m_textDocument;
    QTextCursor*   m_textCursor;
    QRectF         m_boundingRect;
    QFont          m_font;
    QTimer         m_timer; //光标闪烁计时
    QPointF        m_textCursorPos1; //光标线段p1点
    QPointF        m_textCursorPos2; //光标线段p2点
    bool           m_isDisplayTextCursor; //是否显示文本光标
    bool           m_isEditing;   //是否编辑状态
};

#endif // GRAPHICSTEXTITEM_H
