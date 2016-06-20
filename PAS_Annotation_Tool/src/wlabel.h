#ifndef WLABEL_H
#define WLABEL_H

#include <QDebug>
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

#include <iostream>

typedef struct CurrentRect
{
    QPoint p;//top left point
    QPoint cp;//current point in mousemove event
    bool init;

    CurrentRect() : init(false) {}

    int w() { return cp.x()-p.x(); }
    int h() { return cp.y()-p.y(); }
}CRect;

class WLabel : public QLabel
{
    Q_OBJECT
public:
    explicit WLabel( QWidget *parent=0 );
    ~WLabel(){}

    void set_bboxs(QVector<QRect> *rects);

    void clear_bboxs() { bboxs = NULL; }

    void set_image(const QImage &image);

    void set_focus(int f) { focus_index = f; this->update(); }

    void enable();

    void disable();

    void cancle_rect() { cbox.init = false; this->update(); }

signals:
    void on_bbox_appending_signal();

protected:
    void paintEvent(QPaintEvent *e);

    void mousePressEvent( QMouseEvent *e );

    void mouseMoveEvent(QMouseEvent *e);

    void leaveEvent(QEvent *event);

private:
    QPoint point_label2orig(const QSize &labelsize, const QSize &pixmapsize, const QPoint &p);

    QPoint point_orig2label(const QSize &labelsize, const QSize &pixmapsize, const QPoint &p);

    QRect rect_orig2label(const QRect &r);

    QRect rect_label2orig(const QRect &r);

    QVector<QRect> rects_orig2label(const QVector<QRect> &boxs);

    bool _InPicRect(const QPoint &pos)
    {
        if(pos.x() < topleft.x()) return false;
        if(pos.y() < topleft.y()) return false;
        if(pos.x() > bottomright.x()-1) return false;
        if(pos.y() > bottomright.y()-1) return false;
        return true;
    }

    float _wmin(float a, float b) { return a<b ? a : b; }

private:
    bool _enabled;
    QPixmap img_pixmap;
    QPoint topleft;
    QPoint bottomright;
    QVector<QRect> *bboxs;
    CRect cbox;
    QPoint mousepos;
    int focus_index;

    //Cursor change
    enum CursorStat{NoneInit, CursorArrow, CursorCross};
    CursorStat cursorstat;
};

#endif // WLABEL_H
