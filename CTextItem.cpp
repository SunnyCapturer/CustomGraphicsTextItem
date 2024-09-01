﻿#include "CTextItem.h"

#include <QGuiApplication>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextBlock>
#include <QInputMethod>
#include <Qt>

static void qt_graphicsItem_highlightSelected(
        QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
    if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
        return;

    const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
    if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
        return;

    qreal itemPenWidth;
    switch (item->type()) {
        case QGraphicsEllipseItem::Type:
            itemPenWidth = static_cast<QGraphicsEllipseItem *>(item)->pen().widthF();
            break;
        case QGraphicsPathItem::Type:
            itemPenWidth = static_cast<QGraphicsPathItem *>(item)->pen().widthF();
            break;
        case QGraphicsPolygonItem::Type:
            itemPenWidth = static_cast<QGraphicsPolygonItem *>(item)->pen().widthF();
            break;
        case QGraphicsRectItem::Type:
            itemPenWidth = static_cast<QGraphicsRectItem *>(item)->pen().widthF();
            break;
        case QGraphicsSimpleTextItem::Type:
            itemPenWidth = static_cast<QGraphicsSimpleTextItem *>(item)->pen().widthF();
            break;
        case QGraphicsLineItem::Type:
            itemPenWidth = static_cast<QGraphicsLineItem *>(item)->pen().widthF();
            break;
        default:
            itemPenWidth = 1.0;
    }
    const qreal pad = itemPenWidth / 2;

    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
        fgcolor.red()   > 127 ? 0 : 255,
        fgcolor.green() > 127 ? 0 : 255,
        fgcolor.blue()  > 127 ? 0 : 255);

    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}


CTextItem::CTextItem(const QString &text, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_text(text)
    , m_textDocument(nullptr)
    , m_textCursor(nullptr)
    , m_isEditing(false)
{
    m_isDisplayTextCursor = false;

    m_textDocument = new QTextDocument(this);
    m_textDocument->setDefaultFont(m_font);

    m_textCursor = new QTextCursor(m_textDocument);
    m_textCursor->insertText(m_text);
    m_boundingRect.setSize(m_textDocument->size());

    m_timer.setInterval(500);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable | ItemAcceptsInputMethod);

    connect(m_textDocument, &QTextDocument::contentsChanged, this, &CTextItem::test);
    connect(m_textDocument, &QTextDocument::contentsChanged, this, &CTextItem::calcCursorPos);
    connect(&m_timer, &QTimer::timeout, this, &CTextItem::cursorFlag);
}

QRectF CTextItem::boundingRect() const
{
    return m_boundingRect;
}

void CTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    painter->save();
    QPen p(Qt::red);
    painter->setPen(p);
    painter->setFont(m_font);
    m_textDocument->drawContents(painter, m_boundingRect);
    painter->restore();

    if((option->state & QStyle::State_HasFocus) && m_isDisplayTextCursor)
        painter->drawLine(m_textCursorPos1, m_textCursorPos2);

    if(option->state & (QStyle::State_Selected  | QStyle::State_HasFocus))
        qt_graphicsItem_highlightSelected(this, painter, option);

}

void CTextItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_isEditing = false;
    m_isDisplayTextCursor = false;
    m_timer.stop();
    setCursor(Qt::ArrowCursor);
}

void CTextItem::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    bool modified = false;

    switch (key) {
    case Qt::Key_Left:
        if (m_textCursor->movePosition(QTextCursor::Left)) {
            modified = true;
        }
        break;
    case Qt::Key_Right:
        if (m_textCursor->movePosition(QTextCursor::Right)) {
            modified = true;
        }
        break;
    case Qt::Key_Up:
        if (m_textCursor->movePosition(QTextCursor::Up)) {
            modified = true;
        }
        break;
    case Qt::Key_Down:
        if (m_textCursor->movePosition(QTextCursor::Down)) {
            modified = true;
        }
        break;
    case Qt::Key_Enter:
        modified = true;
        break;
    case Qt::Key_Backspace:
        m_textCursor->deletePreviousChar();
        modified = true;
        break;
    default:
        m_textCursor->insertText(event->text());
        modified = true;
        break;
    }

    if(modified){
        m_text = m_textDocument->toPlainText();
        calcCursorPos();
        update();
    }
}

void CTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    m_isEditing = true; //进入编辑状态
    m_isDisplayTextCursor = true;
    cursorFlag();       //显示光标
    calcCursorPos();    //计算光标位置
    m_timer.start();    //光标闪烁计时
    if(m_isEditing)
        setCursor(Qt::IBeamCursor);
    else
        setCursor(Qt::ArrowCursor);
}

#include <QInputMethod>

void CTextItem::inputMethodEvent(QInputMethodEvent *event)
{
    if (event->commitString().isEmpty())
        return;

    // 插入输入法事件的文本
    m_textCursor->insertText(event->commitString());

    // 更新文本内容
    m_text = m_textDocument->toPlainText();

    // 更新光标位置
    calcCursorPos();

    // 更新文本显示
    update();

    // 计算光标的矩形区域
    // QRectF cursorRect = m_textCursor->boundingRect();

    // 将光标矩形区域从局部坐标系转换为全局坐标系
    QPointF globalPos = mapToScene(m_textCursorPos2);
    qDebug() << "globalPos:" << globalPos;

    // 更新输入法候选窗口的位置
    QGuiApplication::inputMethod()->setInputItemRectangle(QRectF(globalPos, QSizeF(1000, 1000)));
}


QVariant CTextItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}


void CTextItem::test()
{
    prepareGeometryChange();
    m_boundingRect.setSize(m_textDocument->size());
}

void CTextItem::calcCursorPos()
{
    int pos = m_textCursor->position();
    QString text = m_textDocument->toPlainText();

    QFontMetrics metrics(m_font);
    int pixelPosX = m_textDocument->documentMargin();
    int flag = 0; //换行符
    for (int i = 0; i < pos; ++i) {
        if( text.at(i) == '\n') {
            pixelPosX = m_textDocument->documentMargin();
            flag++;
            continue;
        }
        QChar ch = text.at(i);
        pixelPosX += metrics.width(ch);
    }
    int pixelPosY = m_textDocument->documentMargin() + /*metrics.leading() +*/ flag * (metrics.leading() + metrics.height());
    //p1
    QPointF p1(pixelPosX, pixelPosY);
    m_textCursorPos1 = p1;
    //p2
    pixelPosY = p1.y() + metrics.ascent() + metrics.descent();
    m_textCursorPos2 = QPointF(pixelPosX, pixelPosY);
}

void CTextItem::cursorFlag()
{
    m_isDisplayTextCursor = !m_isDisplayTextCursor;
    update();
}

