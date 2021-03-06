﻿#include "polyline.h"
#include <QCursor>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QDebug>

Polyline::Polyline(QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    type(line),
    elevation(1),
    alpha(0),
    offset(QPointF())
{
    setShapeType(Shape::Polyline);
    // 设置图元为可焦点的
    setFlag(QGraphicsItem::ItemIsFocusable);
    // 设置图元为可移动的
    setFlag(QGraphicsItem::ItemIsMovable, false);
    // 设置图元为可接受拖拽事件
    setAcceptDrops(true);
    // 设置图元为可接受hover事件
    setAcceptHoverEvents(true);
<<<<<<< HEAD
=======
    i=0;
>>>>>>> Jeremy
}

void Polyline::startDraw(QGraphicsSceneMouseEvent *event)
{
    QPen pen = QPen();
    pen.setColor(penStyle.color);
    pen.setStyle(penStyle.style);
    pen.setWidthF(penStyle.width);
    setPen(pen);
    QPointF point = event->scenePos();
    points.append(point);
}

void Polyline::drawing(QGraphicsSceneMouseEvent *event)
{
    newPoint = event->scenePos();
    update();
}

bool Polyline::updateFlag(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    QPointF point = event->scenePos();
    points.append(point);
    update();
    return overFlag;
}

void Polyline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    scaleFactor = painter->matrix().m11();
    // 获取到当前的线宽，这里的线宽其实还是之前设置的线宽值;
    // 比如我们之前设置线宽为 2 ，这里返回的线宽还是 2 ，但是当前的缩放比例变了；
    // 其实当前的线宽就相当于 penWidth * scaleFactor;
    // 所以如果我们想要让线宽保持不变，那就需要进行转换，即 penWidth = penWidth / scaleFactor;
    QPen pen = this->pen();
    // 重新设置画笔线宽;
    pen.setColor(penStyle.color);
    pen.setWidthF(pen.widthF() / scaleFactor);
    if(collision){
        pen.setColor(selectedEntity.color);
    }
<<<<<<< HEAD
=======

>>>>>>> Jeremy
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    // 如果已经结束绘图则直接绘原图
    if(overFlag){
        setPath(this->path());
    }
    //  如果点集为空，则返回
    if(points.isEmpty()){
        return;
    }
    int size = 2;
    int len = points.length();
    QPainterPath path(points.at(0));
    switch (type) {
    case line:
        for (int i = 0; i < len - 1; ++i) {
            //painter->setBrush(QBrush(collides ? selectedEntity.color : penStyle.color));
            //drawRectPoint(painter, points.at(i), size);
            if(fill){  // 如果填充，设置刷子颜色
                painter->setBrush(penStyle.brush);
            }
            path.lineTo(points.at(i+1));
        }
        if(!overFlag){
            path.lineTo(newPoint);
        }
        break;
    case curve:

        break;
    case cubic:
        for (int i = 0; i < len - 1; ++i) {
            // 控制点的 x 坐标为 sp 与 ep 的 x 坐标和的一半
            // 第一个控制点 c1 的 y 坐标为起始点 sp 的 y 坐标
            // 第二个控制点 c2 的 y 坐标为结束点 ep 的 y 坐标
            QPointF sp = points.at(i);
            QPointF ep = points.at(i+1);

            painter->setBrush(QBrush(penStyle.brush));
            drawRectPoint(painter, points.at(0), 1);
            painter->setBrush(QBrush());

            QPointF c1 = QPointF((sp.rx() + ep.rx()) / 2, sp.ry());
            QPointF c2 = QPointF((sp.rx() + ep.rx()) / 2, ep.ry());
            path.cubicTo(c1, c2, ep);
        }
        if(!overFlag){
            QPointF sp = points.at(len-1);
            QPointF ep = newPoint;
            QPointF c1 = QPointF((sp.rx() + ep.rx()) / 2, sp.ry());
            QPointF c2 = QPointF((sp.rx() + ep.rx()) / 2, ep.ry());
            path.cubicTo(c1, c2, ep);
        }
        break;

    default:
        break;
    }
//    drawCrossPoint(painter, boundingRect().center(), 2, upright);
//    painter->drawRect(boundingRect());

    if(offset!=QPointF(0, 0)){
        qDebug() << offset;
    }

    if(alpha != 0){
        painter->save();
        painter->translate(boundingRect().center());
        painter->rotate(alpha); //以中心点为中心，顺时针旋转alpha度
        painter->translate(-boundingRect().center());
        painter->drawPath(path);
        setPath(path);
        painter->restore();
        return;
    }
//    drawRectPoint(painter, points.at(0), size);
    painter->drawPath(path);
    drawRectPoint(painter, this->boundingRect().center(), 2);
    setPath(path);

<<<<<<< HEAD
=======
    foreach (QLineF line, rLines) {
        painter->save();
        QPen p = pen;
        p.setStyle(Qt::DashLine);
        p.setColor(Qt::red);
        painter->setPen(p);
        painter->drawLine(line);
        painter->restore();
    }

    painter->drawText(boundingRect().center(), QString("%1").arg(i));
}

>>>>>>> Jeremy
QPainterPath Polyline::shape() const
{
    return this->path();
}

bool Polyline::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    collision = false;
    if(QGraphicsItem::collidesWithItem(other, mode)){
        collision = true;
    }
    return collision;
}

<<<<<<< HEAD
void Polyline::setPolyline(QList<QPointF> pList, int flag, qreal ele, qreal angle, const QPointF off)
=======
void Polyline::setPolyline(QVector<QPointF> pList, int flag, qreal ele, qreal angle, const QPointF off)
>>>>>>> Jeremy
{
    QPen pen = QPen();
    pen.setColor(penStyle.color);
    pen.setStyle(penStyle.style);
    pen.setWidthF(penStyle.width);
    pen.setBrush(penStyle.brush);
    setPen(pen);

    fill = true;
    points.append(pList);
    type = (Type)(flag == 0 ? 1 : flag);
    elevation = ele;
    alpha = angle;
    offset = off;
    update();
    overFlag = true;
}

void Polyline::setPoints(const QVector<QPointF> &value)
{
    points.clear();
    points.append(value);
}

QVector<QPointF> Polyline::getPoints()
{
    return this->points;
}

void Polyline::setRLines(const QVector<QLineF> &lines)
{
    this->rLines = lines;
}

QVector<QLineF> Polyline::getRLines()
{
    return this->rLines;
}

void Polyline::setType(Polyline::Type type)
{
    this->type = type;
}

Polyline::Type Polyline::getType()
{
    return this->type;
}

void Polyline::setElevation(qreal elevation)
{
    this->elevation = elevation;
}

qreal Polyline::getElevation()
{
    return this->elevation;
}

void Polyline::setAlpha(const qreal alpha)
{
    this->alpha = alpha;
}

qreal Polyline::getAlpha() const
{
    return this->alpha;
}

QRectF Polyline::getBoundingRect()
{
    qreal top = LONG_MAX;
    qreal bottom = LONG_MIN;
    qreal left = LONG_MAX;
    qreal right = LONG_MIN;
    foreach (QPointF pos, points) {
        if(top > pos.ry()){
            top = pos.ry();
        }
        if(bottom < pos.ry()){
            bottom = pos.ry();
        }
        if(left > pos.rx()){
            left = pos.rx();
        }
        if(right < pos.rx()){
            right = pos.rx();
        }
    }
    QRectF boundRect(QPointF(left, top), QPointF(right, bottom));
    return boundRect;
}

Polyline *Polyline::copy()
{
    Polyline *p = new Polyline(this);
    p->setPolyline(points, type, elevation, alpha, offset);
    p->setRLines(this->rLines);
    return p;
}

void Polyline::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(selectable){
        selected = !selected;
        qDebug() << "type: " << getShapeType();
        qDebug() << "id: " << getShapeId();
        setCursor(Qt::ClosedHandCursor);
        QPen pen = QPen();
        pen.setColor(selectedEntity.color);
        pen.setStyle(selectedEntity.style);
        pen.setWidthF(selectedEntity.width);
        setPen(pen);
        emit select(this);
    }
    QGraphicsItem::mousePressEvent(event);
}

void Polyline::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void Polyline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // qDebug() << "Polyline::mouseReleaseEvent";
    QGraphicsItem::mouseReleaseEvent(event);
}

void Polyline::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Polyline::dragEnterEvent";
    QGraphicsItem::dragEnterEvent(event);
}

void Polyline::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Polyline::dragLeaveEvent";
    QGraphicsItem::dragLeaveEvent(event);
}

void Polyline::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Polyline::dragMoveEvent";
    QGraphicsItem::dragMoveEvent(event);
}

void Polyline::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Polyline::dropEvent";
    QGraphicsItem::dropEvent(event);
}

void Polyline::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(selectable){
        QPen pen = QPen();
        if(!selected){
            pen.setColor(underCursorStyle.color);
            pen.setStyle(underCursorStyle.style);
            pen.setWidthF(underCursorStyle.width);
        } else{
            pen.setColor(selectedEntity.color);
            pen.setStyle(selectedEntity.style);
            pen.setWidthF(selectedEntity.width);
        }
        setPen(pen);
        setCursor(Qt::OpenHandCursor);
        QGraphicsItem::hoverEnterEvent(event);
    }
}

void Polyline::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    return;
    if(selectable){
        qDebug() << "Polyline::hoverMoveEvent";
        setCursor(Qt::PointingHandCursor);
        for(int j = 0; j<points.length();j++) {
            QLineF line(points[j], points[(j+1)%points.length()]);
            QLineF line1(line.p1(), event->scenePos());
            if((0 <= line.angleTo(line1) && line.angleTo(line1) <= 2)
                    || (178 <= line.angleTo(line1) && line.angleTo(line1) <= 182)
                    || (358 <= line.angleTo(line1) && line.angleTo(line1)<= 360)){
                qDebug() << "存在该点 " << i++;
                setCursor(Qt::UpArrowCursor);
            }
        }
        QGraphicsItem::hoverMoveEvent(event);
    }
}

void Polyline::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(selectable){
        QPen pen = QPen();
        if(!selected){
            pen.setColor(penStyle.color);
            pen.setStyle(penStyle.style);
            pen.setWidthF(penStyle.width);
        } else{
            pen.setColor(selectedEntity.color);
            pen.setStyle(selectedEntity.style);
            pen.setWidthF(selectedEntity.width);
        }
        setPen(pen);
        QGraphicsItem::hoverLeaveEvent(event);
    }
}

void Polyline::onSceneMoveableChanged(bool moveable)
{
    this->moveable = moveable;
    setFlag(QGraphicsItem::ItemIsMovable, moveable);
}

