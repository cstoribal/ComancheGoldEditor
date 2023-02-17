#include "pffEditor.h"
#include "gui/qtHelpers/guiSetup.h"
#include "pffEditorActions.h"
#include "qmessagebox.h"
#include "qtoolbar.h"

#include <QDockWidget>
#include <QTextEdit>
#include <QStandardItemModel>
#include <QGuiApplication>

#include <QFileDialog>


#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>

#include <io/pffParser.hpp>
#include <iostream>




PffEditor::PffEditor(const QString& file, QMainWindow* mainWin):mFileName(file),f_fileModified(false)
{
    setWindowTitle(tr("Pff Editor - ")+file);

    mUndoStack = new QUndoStack(this);

    init();
    initActivate();

    //--Dock Widget update
    mainWin->addDockWidget(Qt::LeftDockWidgetArea, mToolDock);
    mainWin->addDockWidget(Qt::RightDockWidgetArea,mFilesDock);

    mTreeView = new TreeViewPff(this);
    mFilesDock->setWidget(mTreeView);

    // Load Second version with incorporated parser
    mFileParser.readFromFile(file.toStdString()); //Check parseHeader()


    mTextEdit = new HexTextEdit(tr("Enable \"view files\" button and click any file for hexadecimal display"));
    mTextEdit->setUndoRedoEnabled(false);
    mTextEdit->setFocusPolicy(Qt::NoFocus);

    mToggleEditionButton = new QPushButton("Edition Mode",this);
    mToggleViewButton = new QPushButton("View Files",this);
    mToolDockLayout->addWidget(mToggleEditionButton);
    mToolDockLayout->addWidget(mToggleViewButton);
    mToggleEditionButton->setCheckable(true);
    mToggleViewButton->setCheckable(true);

    mToggleEditionButton->setStyleSheet(
        "QPushButton:checked{background-color:#DAFAA3;}"
        "QPushButton{background-color:#EDEBEB;}"
    );
    mToggleViewButton->setStyleSheet(
            "QPushButton:checked{background-color:#DAFAA3;}"
            "QPushButton{background-color:#EDEBEB;}"
    );
    connect(mToggleEditionButton,&QPushButton::clicked,this,&PffEditor::onToggleEditionMode );

    // Release the keyboard for the parent widgets. Reenable with toogle button
    QFont font = mTextEdit->font();
    font.setFamily("Courier New");
    mTextEdit->setFont(font);
    setWidget(mTextEdit);


    refreshFileState();

    connect(mTreeView, &QAbstractItemView::clicked,this,&PffEditor::onTreeViewFileClick);
    connect(mTreeView, &QAbstractItemView::pressed,this,&PffEditor::onTreeViewFileRightClick);
    connect(mTreeView, &TreeViewPff::movedItem, this, &PffEditor::moveItem);
    connect(this,   &PffEditor::onDeleteKeyPressed, this, &PffEditor::onRemoveItem);
    //mFileList->setFont(font);
    //setWidget(mFileList);
}

void PffEditor::refreshFileState(){
    QStandardItemModel *itemModel = nullptr;
    if(!mTreeView->model()){
        itemModel = new QStandardItemModel(this);
    } else {
        itemModel = dynamic_cast<QStandardItemModel*>(mTreeView->model());
        itemModel->clear();
    }
    int row = 0;
    std::vector<NovalogicPffParser::FileSegDataConst> fileData = mFileParser.getFileData();
    for (const auto & data : fileData) {
        QStandardItem *itemName = new QStandardItem(QString::fromStdString(data.filePtr->fileName()));
        //TODO make a list of QIcons that are related with the object type.
        QStandardItem *itemPos = new QStandardItem(QString::fromStdString(std::to_string(data.filePtr->position())));
        QStandardItem *itemSize = new QStandardItem(QString::fromStdString(std::to_string(data.filePtr->size())));
        itemModel->setItem(row, 0, itemName);
        itemModel->setItem(row, 1, itemPos);
        itemModel->setItem(row, 2, itemSize);
        row++;
    }

    itemModel->setHorizontalHeaderLabels(QStringList({tr("File Name"),tr("File Position"), tr("File Size")}));

    mTreeView->setModel(itemModel);
}

void PffEditor::onTreeViewFileClick(const QModelIndex &index){
    if(mToggleViewButton->isChecked()){
        mTextEdit->setText(QString::fromStdString(mFileParser.getFileHexContent(index.row())));
    }
}

void TreeViewPff::dropEvent(QDropEvent *event){
    // unused : DropIndicatorPosition dip = dropIndicatorPosition();
    QModelIndex idx1 = QTreeView::currentIndex();
    QModelIndex idx2 = QTreeView::indexAt(event->position().toPoint());
    //TODO emit signal element moving
    if(idx1.row()!=idx2.row()){
        emit movedItem(idx1.row(),idx2.row());
    }
    return;
}

void TreeViewPff::dragLeaveEvent(QDragLeaveEvent * event){
    event->accept();
}

void PffEditor::onTreeViewFileRightClick(const QModelIndex &index){
    if(QGuiApplication::mouseButtons()==Qt::RightButton){
        //TODO right click menu
        //Google : qt right click context menu
        QMenu contextMenu(tr("Context menu"),this);

        QAction actionExport(tr("&Export To File"), this);
        connect(&actionExport, SIGNAL(triggered()), this, SLOT(exportItemToFile()));
        contextMenu.addAction(&actionExport);

        QAction insertItemAction(tr("&Insert Item"), this);
        connect(&insertItemAction, &QAction::triggered, this, &PffEditor::onInsertItem);
        contextMenu.addAction(&insertItemAction);

        QAction removeItemAction(tr("&Remove Item"), this);
        connect(&removeItemAction, &QAction::triggered, this, &PffEditor::onRemoveItem);
        contextMenu.addAction(&removeItemAction);

        contextMenu.addSeparator();

        QAction editAsMapAction(tr("&Open as Map"), this);
        connect(&editAsMapAction, &QAction::triggered, this, &PffEditor::onOpenMap);
        contextMenu.addAction(&editAsMapAction);

        QAction* selectedAction = contextMenu.exec(QCursor::pos( ));

    }
}

void PffEditor::keyPressEvent(QKeyEvent* keyEvent){
    if( keyEvent->key() == Qt::Key_Delete )
    {
        if(mTreeView->currentIndex().row()!=-1){
            emit onDeleteKeyPressed(); // connected elsewhere
        }
    }
}

void PffEditor::exportItemToFile(){
    int idx = mTreeView->currentIndex().row();


    QFileDialog fileDialog(this,tr("Set filename"));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.selectFile(mFileParser.getFileData()[idx].filePtr->fileName());
    fileDialog.exec();
    QStringList fileList = fileDialog.selectedFiles();
    if(fileList.size()==0){
        return; //The user closed the dialogBox
    }
    std::string selectedFile = fileList[0].toStdString();

    RawHexParser hexWriter;
    hexWriter.setBuffer(mFileParser.readItem(idx));
    hexWriter.writeToFile(selectedFile);

    return;
}

void PffEditor::onOpenMap(){
    int index = mTreeView->currentIndex().row();

    emit eventOpenMapEditor(mFileParser.readItem(index));
}




void PffEditor::init(){
    mToolDock = new QDockWidget(tr("PffEditor Tools"),this);
    mToolDock->setAllowedAreas(Qt::RightDockWidgetArea);
    mToolDock->setFeatures(mToolDock->features()
                           & ~QDockWidget::DockWidgetClosable);

    mFilesDock = new QDockWidget(tr("Files"),this);
    mFilesDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    mFilesDock->setFeatures(mFilesDock->features() &
                ~QDockWidget::DockWidgetClosable);

    QWidget* toolDockWidgetHolder = new QWidget(mToolDock);
    mToolDockLayout = new QVBoxLayout(toolDockWidgetHolder);
    mToolDock->setWidget(toolDockWidgetHolder);
    toolDockWidgetHolder->setLayout(mToolDockLayout);
    // TODO add geometry information

    // TODO add tools
    // TODO should make a menu for pff editor (that disappear when hidden)

    QToolBar *toolBar = new QToolBar(tr("Pff Edition Toolbar"));
    QAction *saveAct = qth::addAction(nullptr,tr("&Save"),this,&PffEditor::onSave,
                                   "document-save",tr("Save the current project"),"../T1/assets/save.png",
                                   QKeySequence::Save);//new QAction(tr("&Save"));

    toolBar->addAction(saveAct);
    QAction *saveAsAct = qth::addAction(nullptr,tr("Save&As"),this,&PffEditor::onSaveAs,
                                   "document-save",tr("Save the current project"),"../T1/assets/save.png",
                                   QKeySequence::SaveAs);//new QAction(tr("&Save"));

    toolBar->addAction(saveAsAct);


    QAction *appendAct = qth::addAction(nullptr,tr("&Append Item"),this,&PffEditor::onAppendItem,
                                   "document-append",tr("Save the current project"),"",
                                   QKeySequence::UnknownKey);//new QAction(tr("&Save"));

    toolBar->addAction(appendAct);


    mToolDockLayout->addWidget(toolBar);

}

void PffEditor::initActivate(){
    connect(this,&QMdiSubWindow::aboutToActivate,
            mToolDock, &QWidget::show);
    connect(this, &QMdiSubWindow::aboutToActivate,
            mFilesDock, &QWidget::show);

    // TODO update map, maybe ?

}


// Todo : Fonction Deactivate
//        Fonction onClose




void PffEditor::deactivation(QMdiSubWindow* window){

    if(window != this){
        return;
    }
    mToolDock->hide();
    mFilesDock->hide();

}

void PffEditor::whenClosed(){
    mToolDock->hide();
    mFilesDock->hide();

    //preclose
}

void PffEditor::setModified(bool modif){
    if(f_fileModified==false && modif){
        // TODO
        //emit enableSaveButton();
        setWindowTitle(tr("Pff Editor - ")+mFileName+"*");
        f_fileModified = modif;
    } else if(f_fileModified==true && !modif){
        // TODO
        //emit disableSaveButton();
        f_fileModified = modif;
        setWindowTitle(tr("Pff Editor - ")+mFileName);
    }
}

void PffEditor::moveItem(int from, int to){
    mUndoStack->push(new ActpffMoveFile(this,from,to));
}

void PffEditor::onAppendItem(){
    QFileDialog fileDialog(this,tr("Select file (15 characters max) to insert at"));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setNameFilter("15 caracter max files (? ?? ??? "
                             "???? ????? ?????? "
                             "??????? ???????? ????????? "
                             "?????????? ??????????? "
                             "???????????? ????????????? "
                             "?????????????? ??????????????? "
                             "???????????????? )");
    int result = fileDialog.exec();
    QStringList selectedFile = fileDialog.selectedFiles();
    if(selectedFile.size()==0 || result!=1){
        return; // User canceled the operation
    }
    for(int i=0; i<selectedFile.size(); i++){
        std::string filename = selectedFile[i].toStdString();
        RawHexParser fileReader(filename);
        mUndoStack->push(new ActpffInsertFile(this, -1, selectedFile[i].toStdString(),fileReader.readData()));
    }
}

void PffEditor::onInsertItem(){
    int index = mTreeView->currentIndex().row();

    QFileDialog fileDialog(this,tr("Select file (15 characters max) to insert at"));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setNameFilter("15 caracter max files (? ?? ??? "
                             "???? ????? ?????? "
                             "??????? ???????? ????????? "
                             "?????????? ??????????? "
                             "???????????? ????????????? "
                             "?????????????? ??????????????? "
                             "???????????????? )");
    int result = fileDialog.exec();
    QStringList selectedFile = fileDialog.selectedFiles();
    if(selectedFile.size()==0 || result!=1){
        return; // User canceled the operation
    }
    for(int i=selectedFile.size()-1; i>=0; i--){
        std::string filename = selectedFile[i].toStdString();
        RawHexParser fileReader(filename);
        mUndoStack->push(new ActpffInsertFile(this, index, selectedFile[i].toStdString(),fileReader.readData()));
    }
}

void PffEditor::onRemoveItem(){
    int index = mTreeView->currentIndex().row();
    const std::vector<char> & data = mFileParser.readItem(index);
    NovalogicPffParser::FileSegDataConst fileData = mFileParser.getFileData(index);
    mUndoStack->push(new ActpffRemoveFile(this,index,fileData.filePtr->fileName(),data));
}

void PffEditor::actMoveItem(int from, int to){
    setModified(true);
    mFileParser.moveItem(from,to);
    refreshFileState();
}

void PffEditor::actInsertItem(std::string filename, int idx, const std::vector<char> & data){
    setModified(true);

    PffWrapperS* wrapper = new PffWrapperS(data.size());
    wrapper->setFileName(filename);
    mFileParser.insertItem(idx,wrapper,data);

    refreshFileState();
}

void PffEditor::actRemoveItem(int idx){
    setModified(true);
    mFileParser.removeItem(idx);
    refreshFileState();
}

void PffEditor::onSave(){
    mFileParser.writeToFile(mFileName.toStdString());
    setModified(false);
}

void PffEditor::onSaveAs(){
    QFileDialog fileDialog(this,tr("Set filename"));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.selectFile(mFileName);
    fileDialog.exec();
    QStringList fileList = fileDialog.selectedFiles();
    if(fileList.size()==0){
        return; //The user closed the dialogBox or canceled
    }
    std::string selectedFile = fileList[0].toStdString();

    RawHexParser hexWriter;
    mFileParser.writeToFile(selectedFile);

    mFileName = QString::fromStdString(selectedFile);

    setModified(false);
}



