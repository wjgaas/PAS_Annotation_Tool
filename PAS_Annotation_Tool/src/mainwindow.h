#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QListView>
#include <QHBoxLayout>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QString>
#include <QDebug>
#include <io.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "filesupport.h"
#include "bbox.h"

namespace Ui {
class MainWindow;
}

typedef struct ImageInfo
{
    QString image_path;

    QString image_name;

    QString annotation_path;

    QVector<QRect> bboxes;

    ImageSize_t image_size;

    bool dirty_flag;

    ImageInfo() : dirty_flag(false) {}

}ImageInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void SetImagePath();

    void SetAnnotationPath();

    void PicListModelAppend(const QString &path,QStringListModel* slm);

    void LoadCurrentImageAndBBox(const QModelIndex &modelIndex);

    void LabelListModelAppend(const QVector<QRect> &bboxes, QStringListModel* slm);

    QVector<QRect> GetAnnotationsFromFile(QString filename);

    void SetAnnotationsToFile(QString path, QString filename, \
                              const QVector<QRect> &rects, const ImageSize_t &image_size);

    void AutoSaveAnnotationsToFile();

    void PicListViewJmpToPreImage();

    void PicListViewJmpToNextImage();

protected:
    void keyPressEvent(QKeyEvent *event);

    void wheelEvent(QWheelEvent *e);

private:
    bool _IsImage(QString filename);

    QStringList _FileListFilter(const FileList &flist);

    QModelIndex _GetListViewCurrentIndex(QListView *listview);

private slots:
    void on_pushButtonImagePath_clicked();

    void on_pushButtonLabelPath_clicked();

    void handle_bbox_appending_slot();

    void on_pushButtonDelLabel_clicked();

    void handle_listViewPicList_SelectionChanged(const QModelIndex &current, const QModelIndex &previous);

    void handle_listViewLabelList_SelectionChanged(const QModelIndex &current, const QModelIndex &previous);

    void handle_lsitViewLabelListAction();

    void on_pushButtonSaveLabel_clicked();

    void on_pushButtonNextImage_clicked();

    void on_pushButtonPreImage_clicked();

    void on_actionTourial_triggered();

    void on_actionAbout_triggered();

    void on_actionImagePath_triggered();

    void on_actionAnnotationPath_triggered();

private:
    Ui::MainWindow *ui;

    QLabel *status_image_path;

    QLabel *status_annotations_path;

    QLabel *status_copyright;

    //
    QStringListModel* pic_slm;

    QStringListModel* label_slm;

    //
    ImageInfo imgInfo;
};

#endif // MAINWINDOW_H
