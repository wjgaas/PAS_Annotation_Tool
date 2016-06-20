#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tourialdialog.h"
#include "aboutdialog.h"


#include <QFileDialog>
#include <QMessageBox>
#include "bbox.h"

using namespace std;

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //status bar
    status_image_path = new QLabel("Images :");

    status_image_path->setMinimumWidth(400);

    status_annotations_path =  new QLabel("Annotations :");

    status_annotations_path->setMinimumWidth(400);

    status_copyright =  new QLabel("Copyright© : W.J.G. ZhuQian XieMei 2016-17");

    ui->statusBar->addWidget(status_image_path);

    ui->statusBar->addWidget(status_annotations_path);

    ui->statusBar->addWidget(status_copyright);

    pic_slm = new QStringListModel();

    label_slm = new QStringListModel();

    ui->listViewPicList->setModel(pic_slm);

    ui->listViewLabelList->setModel(label_slm);

    connect(ui->labelCurrentLabelImage, SIGNAL(on_bbox_appending_signal()), this, SLOT(handle_bbox_appending_slot()));

    connect(ui->listViewPicList->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), \
            this, SLOT(handle_listViewPicList_SelectionChanged(const QModelIndex &, const QModelIndex &)));

    connect(ui->listViewLabelList->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), \
            this, SLOT(handle_listViewLabelList_SelectionChanged(const QModelIndex &, const QModelIndex &)));

    //右键菜单
    QAction *listViewLabelListAction = new QAction("清除标注", this);

    ui->listViewLabelList->addAction(listViewLabelListAction);

    ui->listViewLabelList->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(listViewLabelListAction, SIGNAL(triggered(bool)), this, SLOT(handle_lsitViewLabelListAction()));
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    ///Auto Save Annotations To File
    AutoSaveAnnotationsToFile();

    delete ui;

    delete status_image_path;

    delete status_annotations_path;

    delete status_copyright;

    delete pic_slm;

    delete label_slm;
}

/**
 * @brief MainWindow::on_pushButtonImagePath_clicked
 *
 * Button : ImagePath
 */
void MainWindow::on_pushButtonImagePath_clicked()
{
    SetImagePath();
}

/**
 * @brief MainWindow::SetImagePath
 */
void MainWindow::SetImagePath()
{
    QString path = QFileDialog::getExistingDirectory(this, "Images Dir", "./");

    if(path.length() == 0)
    {
        QMessageBox::information(NULL, tr("Images Dir"), tr("Please Choose a Image Dir"));
        return;
    }

    imgInfo.image_path = path;

    PicListModelAppend(imgInfo.image_path, pic_slm);

    status_image_path->setText("Images : " + imgInfo.image_path);
}

/**
 * @brief MainWindow::on_pushButtonLabelPath_clicked
 *
 * Button : LabelPath
 */
void MainWindow::on_pushButtonLabelPath_clicked()
{
    SetAnnotationPath();
}

/**
 * @brief MainWindow::SetAnnotationPath
 */
void MainWindow::SetAnnotationPath()
{
    QString path = QFileDialog::getExistingDirectory(this, "Annotations Dir", "./");

    if(path.length() == 0)
    {
        QMessageBox::information(NULL, tr("Annotations Dir"), tr("Please Choose a Annotation Dir"));
        return;
    }

    imgInfo.annotation_path = path;

    status_annotations_path->setText("Annotations : " + imgInfo.annotation_path);

    LoadCurrentImageAndBBox(_GetListViewCurrentIndex(ui->listViewPicList));

    LabelListModelAppend(imgInfo.bboxes, label_slm);
}

//
/**
 * @brief MainWindow::handle_listViewPicList_SelectionChanged
 * ListView : PiclList SelectionChanged
 *
 * @param current
 * @param previous
 */
void MainWindow::handle_listViewPicList_SelectionChanged(const QModelIndex &current, \
                                                         const QModelIndex &previous)
{
    if(!current.isValid()) return;

    ui->labelCurrentLabelImage->cancle_rect();

    ///Auto Save Annotations To File
    AutoSaveAnnotationsToFile();

    LoadCurrentImageAndBBox(current);

    LabelListModelAppend(imgInfo.bboxes, label_slm);
}

/**
 * @brief MainWindow::handle_listViewLabelList_SelectionChanged
 * ListView : LabelList SelectionChanged
 *
 * @param current
 * @param previous
 */
void MainWindow::handle_listViewLabelList_SelectionChanged(const QModelIndex &current, \
                                                           const QModelIndex &previous)
{
    ui->textBrowserLabelInfo->clear();

    if(!current.isValid()) return;

    QRect *box = &imgInfo.bboxes[current.row()];

    std::stringstream ss;

    ss << "Image filename : \n\"" << imgInfo.image_name.toStdString() << "\"\n\n";

    ss << "Image size (X x Y x C) : \n" << imgInfo.image_size.h << " x " \
       << imgInfo.image_size.w << " x " << imgInfo.image_size.c << "\n\n";

    ss << "Bounding box : \n" << "(" << box->x() << "," <<  box->y() << ")-(" \
       << box->bottomRight().x() << "," << box->bottomRight().y() << ")\n";

    ui->textBrowserLabelInfo->append(QString::fromStdString(ss.str()));

    ui->labelCurrentLabelImage->set_focus(current.row());
}

/**
 * @brief MainWindow::handle_lsitViewLabelListAction
 * SLOT : clear all bbox in current image
 */
void MainWindow::handle_lsitViewLabelListAction()
{
    imgInfo.bboxes.clear();

    ui->labelCurrentLabelImage->set_bboxs(&imgInfo.bboxes);

    label_slm->setStringList(QStringList());

    imgInfo.dirty_flag = true;
}

/**
 * @brief MainWindow::handle_bbox_appending_slot
 * SLOT : bbox_appending
 */
void MainWindow::handle_bbox_appending_slot()
{
    imgInfo.dirty_flag = true;

    LabelListModelAppend(imgInfo.bboxes, label_slm);
}

/**
 * @brief MainWindow::on_pushButtonPreImage_clicked
 * Button : Pre
 */
void MainWindow::on_pushButtonPreImage_clicked()
{
    PicListViewJmpToPreImage();
}

/**
 * @brief MainWindow::on_pushButtonNextImage_clicked
 * Button : Next
 */
void MainWindow::on_pushButtonNextImage_clicked()
{
    PicListViewJmpToNextImage();
}

/**
 * @brief MainWindow::on_pushButtonDelLabel_clicked
 * Button : DEL
 */
void MainWindow::on_pushButtonDelLabel_clicked()
{
    QModelIndex modelIndex = _GetListViewCurrentIndex(ui->listViewLabelList);

    if(!modelIndex.isValid()) return;

    int rownum = modelIndex.row();

    imgInfo.bboxes.erase(imgInfo.bboxes.begin()+rownum);

    ui->labelCurrentLabelImage->set_bboxs(&imgInfo.bboxes);

    LabelListModelAppend(imgInfo.bboxes, label_slm);

    ui->textBrowserLabelInfo->clear();

    imgInfo.dirty_flag = true;
}

/**
 * @brief MainWindow::on_pushButtonSaveLabel_clicked
 * Button : Save annotation
 */
void MainWindow::on_pushButtonSaveLabel_clicked()
{
    if(imgInfo.dirty_flag == true)
    {
        QString filename = *(imgInfo.image_name.split(".").begin())+".txt";

        SetAnnotationsToFile(imgInfo.annotation_path, filename, imgInfo.bboxes, imgInfo.image_size);
    }
}

/**
 * @brief MainWindow::keyPressEvent
 * ShortCut Key
 *
 * @param event
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Escape:
            ui->labelCurrentLabelImage->cancle_rect();
            break;
        case Qt::Key_Control:
            PicListViewJmpToPreImage();
            break;
        case Qt::Key_Alt:
            PicListViewJmpToNextImage();
            break;
        default:
            break;
    }
}

/**
 * @brief MainWindow::wheelEvent
 * WheelEvent :
 *
 * @param e
 */
void MainWindow::wheelEvent(QWheelEvent *e)
{
    if(!ui->checkBoxWheelScroll->isChecked()) return;

    int degrees = e->delta() / 8;
    int steps = degrees / 15;

    if(steps < 0)
        PicListViewJmpToNextImage();
    else
        PicListViewJmpToPreImage();
}

/**
 * @brief MainWindow::_GetListViewCurrentIndex
 * @param listview
 * @return
 */
QModelIndex MainWindow::_GetListViewCurrentIndex(QListView *listview)
{
    QModelIndex modelIndex;

    QModelIndexList modelList = listview->selectionModel()->selectedIndexes();

    if(!modelList.empty())  modelIndex = modelList.first();

    return modelIndex;
}

/**
 * @brief MainWindow::_IsImage
 * @param filename
 * @return
 */
bool MainWindow::_IsImage(QString filename)
{
    if(filename.endsWith(".jpg")) return true;
    if(filename.endsWith(".JPG")) return true;
    if(filename.endsWith(".png")) return true;
    if(filename.endsWith(".PNG")) return true;

    return false;
}

/**
 * @brief MainWindow::_FileListFilter
 * @param flist
 * @return
 */
QStringList MainWindow::_FileListFilter(const FileList &flist)
{
    QStringList sl;

    for(FileList::const_iterator it=flist.begin(); it!=flist.end(); ++it)
    {
        QString image_name = QString::fromStdString(*it);

        if(!_IsImage(image_name)) continue;

        sl.append(image_name);
    }

    return sl;
}

/**
 * @brief MainWindow::PicListModelAppend
 * @param path
 * @param slm
 */
void MainWindow::PicListModelAppend(const QString &path,QStringListModel* slm)
{
    FileSupport fs;
    FileList flist = fs.list_files(path.toStdString(), false);

    QStringList pic_sl = _FileListFilter(flist);

    if(pic_sl.empty())
    {
        ui->labelCurrentLabelImage->disable();

        imgInfo.bboxes.clear();

        label_slm->setStringList(QStringList());
    }
    else
        ui->labelCurrentLabelImage->enable();

    slm->setStringList(pic_sl);

    //set default index to 0
    ui->listViewPicList->setCurrentIndex(pic_slm->index(0));

    imgInfo.dirty_flag = false;
}

/**
 * @brief MainWindow::LabelListModelAppend
 * @param bboxes
 * @param slm
 */
void MainWindow::LabelListModelAppend(const QVector<QRect> &bboxes, QStringListModel* slm)
{
    QStringList label_sl;

    int cnt=0;
    foreach(QRect r, bboxes)
    {
        std::stringstream ss;

        ss << "[" << cnt++ << "] " << "(" << r.x() << "," <<  r.y() << ")-(" \
           << r.bottomRight().x() << "," << r.bottomRight().y() << ")";

        label_sl.push_back(QString::fromStdString(ss.str()));
    }

    slm->setStringList(label_sl);

    //set default index to 0
    ui->listViewLabelList->setCurrentIndex(label_slm->index(bboxes.size()-1));

    ui->labelCurrentLabelImage->set_focus(bboxes.size()-1);
}

/**
 * @brief MainWindow::LoadCurrentImageAndBBox
 * @param modelIndex
 */
void MainWindow::LoadCurrentImageAndBBox(const QModelIndex &modelIndex)
{
    if(!modelIndex.isValid()) return;

   imgInfo.image_name = ui->listViewPicList->model()->data(modelIndex).toString();

    QImage image(imgInfo.image_path+ "\\" + imgInfo.image_name);

    if(image.isNull()) return;

    imgInfo.image_size.w = image.width();
    imgInfo.image_size.h = image.height();

    if(image.format() == QImage::Format_RGB32)
    {
        imgInfo.image_size.c = 3;
    }
    else if(image.format() == QImage::Format_Indexed8)
    {
        imgInfo.image_size.c = 1;
    }

    ui->labelCurrentLabelImage->set_image(image);

    imgInfo.bboxes = GetAnnotationsFromFile(*(imgInfo.image_name.split(".").begin())+".txt");

    ui->labelCurrentLabelImage->set_bboxs(&imgInfo.bboxes);

    imgInfo.dirty_flag = false;
}

/**
 * @brief MainWindow::GetAnnotationsFromFile
 * @param filename
 * @return
 */
QVector<QRect> MainWindow::GetAnnotationsFromFile(QString filename)
{
    QVector<QRect> rects;

    BBox bb(imgInfo.annotation_path.toStdString(), filename.toStdString());

    std::vector<BBox_t> bboxes;

    //std::vector<BBox_t> to QVector<QRect>
    if(bb.get_bboxes(bboxes))
    {
        for(size_t i=0; i<bboxes.size(); ++i)
        {
            BBox_t &box = bboxes[i];

            rects.push_back(QRect(box.x1, box.y1, box.w(), box.h()));
        }
    }

    return rects;
}

/**
 * @brief MainWindow::SetAnnotationsToFile
 * @param path
 * @param filename
 * @param rects
 * @param image_size
 */
void MainWindow::SetAnnotationsToFile(QString path, QString filename, \
                                      const QVector<QRect> &rects, const ImageSize_t &image_size)
{
    qDebug() << "set_bboxes";

    std::vector<BBox_t> bboxes;

    foreach (QRect r, rects)
    {
        bboxes.push_back(BBox_t(r.x(), r.y(), r.bottomRight().x(), r.bottomRight().y()));
    }

    BBox bb(path.toStdString(), filename.toStdString());

    bb.set_bboxes(bboxes, image_size);
}

/**
 * @brief MainWindow::AutoSaveAnnotationsToFile
 * checkBoxAutoSave
 */
void MainWindow::AutoSaveAnnotationsToFile()
{
    if(ui->checkBoxAutoSave->isChecked() && imgInfo.dirty_flag == true)
    {
        QString filename = *(imgInfo.image_name.split(".").begin())+".txt";

        SetAnnotationsToFile(imgInfo.annotation_path, filename, imgInfo.bboxes, imgInfo.image_size);
    }
}

/**
 * @brief MainWindow::PicListViewJmpToPreImage
 */
void MainWindow::PicListViewJmpToPreImage()
{
    QModelIndex modelIndex = _GetListViewCurrentIndex(ui->listViewPicList);

    if(!modelIndex.isValid()) return;

    if(_GetListViewCurrentIndex(ui->listViewPicList).row() == 0)
    {
        ui->listViewPicList->setCurrentIndex(pic_slm->index(pic_slm->rowCount()-1));
    }
    else
    {
        ui->listViewPicList->setCurrentIndex(pic_slm->index(modelIndex.row()-1));
    }
}

/**
 * @brief MainWindow::PicListViewJmpToNextImage
 */
void MainWindow::PicListViewJmpToNextImage()
{
    QModelIndex modelIndex = _GetListViewCurrentIndex(ui->listViewPicList);

    if(!modelIndex.isValid()) return;

    if(_GetListViewCurrentIndex(ui->listViewPicList).row() == pic_slm->rowCount()-1)
    {
        ui->listViewPicList->setCurrentIndex(pic_slm->index(0));
    }
    else
    {
        ui->listViewPicList->setCurrentIndex(pic_slm->index(modelIndex.row()+1));
    }
}

/**
 * @brief MainWindow::on_actionImagePath_triggered
 * Setting
 */
void MainWindow::on_actionImagePath_triggered()
{
    SetImagePath();
}

/**
 * @brief MainWindow::on_actionAnnotationPath_triggered
 */
void MainWindow::on_actionAnnotationPath_triggered()
{
    SetAnnotationPath();
}

/**
 * @brief MainWindow::on_actionTourial_triggered
 * Help
 */
void MainWindow::on_actionTourial_triggered()
{
    TourialDialog *tourial = new TourialDialog(this);
    tourial->setWindowTitle("使用说明");
    tourial->show();
}

/**
 * @brief MainWindow::on_actionAbout_triggered
 */
void MainWindow::on_actionAbout_triggered()
{
    AboutDialog *about = new AboutDialog(this);

    about->setWindowTitle("关于");

    about->show();
}
