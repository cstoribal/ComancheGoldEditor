#include "mapEditor.h"
#include "core/qt/qtFunctions.h"
#include "core/tool/novaPCXencoding.h"


#include <QDockWidget>
#include <QPixmap>

#include <opencv2/core/matx.hpp>

MapEditor::MapEditor(QMainWindow* mainWin, const QString & name, const std::vector<char> & data):mFilename(name)
{

    mUndoStack = new QUndoStack(this);
    mDock = new QDockWidget(tr("Map edition tools") , this);


    // The dock can be docked left or right
    mDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    // The dock cannot be closed
    mDock->setFeatures(mDock->features() & ~QDockWidget::DockWidgetClosable);
    //Todo show and hide depending on selection.
    mainWin->addDockWidget(Qt::RightDockWidgetArea, mDock);

    connect(this,&QMdiSubWindow::aboutToActivate,mDock,&QWidget::show);

    mMapRepresentation = new QLabel(tr("hello"),this);
    setWindowTitle(tr("Map Edition")+mFilename);
    setWidget(mMapRepresentation);

    NovaPCXEncoding<cv::Vec3b> novaMapDecoder;
    novaMapDecoder.readFile(data);
    cv::Mat_<cv::Vec3b> dataMat = novaMapDecoder.dataMat();
    QImage image = mat_to_qimage_ref(dataMat,QImage::Format_RGB888);
    // TODO setup !
    QPixmap pixmap = QPixmap();
    pixmap.convertFromImage(image);
    //pixmap.load("../T1/assets/comancheLogo.jpg");
    mMapRepresentation->setPixmap(pixmap);



}

// Open From 1... filename, file reading

// Open From 2... MapFormat


void MapEditor::deactivation(QMdiSubWindow* window){

    if(window != this){
        return;
    }
    mDock->hide();

}
