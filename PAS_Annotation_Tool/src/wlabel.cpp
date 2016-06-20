#include "wlabel.h"
#include <math.h>
#include <QToolTip>

/**
 * @brief WLabel::WLabel
 * @param parent
 */
WLabel::WLabel(QWidget *parent)
    : bboxs(NULL), _enabled(false), focus_index(-1)
{
}

/**
 * @brief WLabel::enable
 */
void WLabel::enable()
{
    this->setMouseTracking(true);

    this->setCursor(Qt::BlankCursor);

    _enabled = true;
}

/**
 * @brief WLabel::disable
 */
void WLabel::disable()
{
    this->setMouseTracking(false);

    this->setCursor(Qt::ArrowCursor);

    _enabled = false;
}

/**
 * @brief WLabel::set_bboxs
 * @param rects
 */
void WLabel::set_bboxs(QVector<QRect> *rects)
{
    bboxs = rects;

    //call paintEvent
    this->update();
}

/**
 * @brief WLabel::set_image
 * @param image
 */
void WLabel::set_image(const QImage &image)
{
    img_pixmap = QPixmap::fromImage(image);

    //call paintEvent
    this->update();
}

/**
 * @brief WLabel::paintEvent
 * @param e
 */
void WLabel::paintEvent(QPaintEvent *e)
{
    if(!_enabled) return;

    QPainter painter;

    painter.begin(this);

    double ratio = _wmin(float(this->rect().width())/img_pixmap.width(), \
                  float(this->rect().height())/img_pixmap.height());
    int width = img_pixmap.width()*ratio;
    int height = img_pixmap.height()*ratio;

    topleft.setX((this->width()-width)/2);
    topleft.setY((this->height()-height)/2);
    bottomright.setX((this->width()+width)/2);
    bottomright.setY((this->height()+height)/2);

    painter.drawPixmap(topleft, img_pixmap.scaled(width, height));

    if(bboxs != NULL)
    {
        painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap));

        QVector<QRect> drawboxes = rects_orig2label(*bboxs);
        painter.drawRects(drawboxes);

        if(focus_index >= 0 && focus_index < drawboxes.size())
        {
            painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine, Qt::RoundCap));
            painter.drawRect(drawboxes[focus_index]);
        }
    }

    if(cbox.init == true)
    {
        painter.setPen(QPen(Qt::red, 1, Qt::DashLine, Qt::RoundCap));

        QVector<QRect> rect;
        rect.push_back(QRect(cbox.p, cbox.cp));

        QVector<QRect> drawboxes = rects_orig2label(rect);
        painter.drawRects(drawboxes);
    }

    if(_InPicRect(mousepos))
    {
        //CrossArrow
        painter.setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap));
        QPoint linepoint1(mousepos.x()-80, mousepos.y()+1);
        QPoint linepoint2(mousepos.x()+80, mousepos.y()+1);
        painter.drawLine(linepoint1, linepoint2);

        QPoint linepoint3(mousepos.x()+1, mousepos.y()-80);
        QPoint linepoint4(mousepos.x()+1, mousepos.y()+80);
        painter.drawLine(linepoint3, linepoint4);
    }
}

/**
 * @brief WLabel::mousePressEvent
 * @param e
 */
void WLabel::mousePressEvent(QMouseEvent *e)
{
    if(!_enabled) return;

    if(e->button() == Qt::LeftButton)
    {
        if(_InPicRect(mousepos))
        {
            if(cbox.init == false)
            {
                cbox.p = point_label2orig(this->size(), img_pixmap.size(), e->pos());
                cbox.cp = cbox.p;
                cbox.init = true;
            }
            else    //init == true
            {
                QPoint pp = point_label2orig(this->size(), img_pixmap.size(), e->pos());

                bboxs->push_back(QRect(cbox.p, pp));

                emit on_bbox_appending_signal();

                cbox.init = false;
            }
        }
        else
        {
            if(cbox.init == true)
            {
                QToolTip::showText(e->globalPos(), "\342\206\222_\342\206\222   Out of image area");
            }
        }
    }
    else if(e->button() == Qt::RightButton)
    {
        this->cancle_rect();
    }
    else if(e->button() == Qt::MidButton)
    {

    }

    this->update();
}

/**
 * @brief WLabel::mouseMoveEvent
 * @param e
 */
void WLabel::mouseMoveEvent(QMouseEvent *e)
{
    if(!_enabled) return;

    if(cbox.init == true)
    {
        cbox.cp = point_label2orig(this->size(), img_pixmap.size(), e->pos());
    }

    mousepos = point_label2orig(this->size(), img_pixmap.size(), e->pos());
    mousepos = point_orig2label(this->size(), img_pixmap.size(), mousepos);

    //cursor change
    bool cursor_in_image = _InPicRect(mousepos);
    if(cursorstat == NoneInit)
    {
        if(cursor_in_image)
        {
            cursorstat = CursorCross;
            this->setCursor(Qt::BlankCursor);
        }
        else
        {
            cursorstat = CursorArrow;
            this->setCursor(Qt::ArrowCursor);
        }
        this->update();
    }
    else
    {
        if(cursor_in_image && cursorstat == CursorArrow)
        {
            cursorstat = CursorCross;
            this->setCursor(Qt::BlankCursor);
        }
        else if(!cursor_in_image && cursorstat == CursorCross)
        {
            cursorstat = CursorArrow;
            this->setCursor(Qt::ArrowCursor);
        }
        this->update();
    }
}

/**
 * @brief WLabel::leaveEvent
 * @param event
 */
void WLabel::leaveEvent(QEvent *event)
{
    mousepos = QPoint(9999999, 9999999);
    this->update();
}

/**
 * @brief WLabel::point_label2orig
 * @param labelsize
 * @param pixmapsize
 * @param p
 * @return
 */
QPoint WLabel::point_label2orig(const QSize &labelsize, const QSize &pixmapsize, const QPoint &p)
{
    int x = double(p.x()*pixmapsize.width()) / labelsize.width();
    int y = double(p.y()*pixmapsize.height()) / labelsize.height();

    return QPoint(x, y);
}

/**
 * @brief WLabel::point_orig2label
 * @param labelsize
 * @param pixmapsize
 * @param p
 * @return
 */
QPoint WLabel::point_orig2label(const QSize &labelsize, const QSize &pixmapsize, const QPoint &p)
{
    int x = double(p.x()*labelsize.width()) / pixmapsize.width();
    int y = double(p.y()*labelsize.height()) / pixmapsize.height();

    return QPoint(x, y);
}

/**
 * @brief WLabel::rect_label2orig
 * @param r
 * @return
 */
QRect WLabel::rect_label2orig(const QRect &r)
{
    QPoint p1 = point_label2orig(this->size(),img_pixmap.size(), r.topLeft());
    QPoint p2 = point_label2orig(this->size(),img_pixmap.size(), r.bottomRight());

    return QRect(p1, p2);
}

/**
 * @brief WLabel::rect_orig2label
 * @param r
 * @return
 */
QRect WLabel::rect_orig2label(const QRect &r)
{
    QPoint p1 = point_orig2label(this->size(),img_pixmap.size(), r.topLeft());
    QPoint p2 = point_orig2label(this->size(),img_pixmap.size(), r.bottomRight());

    return QRect(p1, p2);
}

/**
 * @brief WLabel::rects_orig2label
 * @param boxs
 * @return
 */
QVector<QRect> WLabel::rects_orig2label(const QVector<QRect> &boxs)
{
    QVector<QRect> res;

    foreach(QRect r, boxs)
        res.push_back(rect_orig2label(r));

    return res;
}

