#include "mainApplication.h"


#include "qtHelpers/guiSetup.hpp"

#include <QMessageBox>
#include <QMdiSubWindow>
#include <QSettings>
#include <QApplication>
#include <QScreen>
#include <QStatusBar>
#include <QDockWidget>

MainApplication::MainApplication(QWidget *parent)
    : QMainWindow{parent},mMdiArea(new QMdiArea)
{

    mMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mMdiArea);
    connect(mMdiArea, &QMdiArea::subWindowActivated,
            this, &MainApplication::updateMenus);


    createActions();
    createStatusBar();
    updateMenus();
    readSettings();


    setWindowTitle(tr("Commanche Gold Editor"));
    setUnifiedTitleAndToolBarOnMac(true);

    ///// on va construire une liste des actions comme cela
    // il suffira d’avoir une liste des actions actives par mode MDI
    // ok
    //
    connect(mMdiArea, &QMdiArea::subWindowActivated,
            this, &MainApplication::onMdiActivation);


}



void MainApplication::createActions(){
    // UndoGroup
    mUndoGroup = new QUndoGroup(this);
    QAction* undoAction = mUndoGroup->createUndoAction(mUndoGroup);
    QAction* redoAction = mUndoGroup->createRedoAction(mUndoGroup);

    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);
    //grabKeyboard()




    //File Menu
    // This menu manages the current project management.
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    mvActions.push_back(std::vector<QAction*>());
    QToolBar *fileToolBar = addToolBar(tr("File"));

    // New Project
    QAction *newAct = qth::addAction(fileMenu,tr("&New"),this,&MainApplication::newProject,
                                     "document-new", "Create a blank project","../T1/assets/new.png",
                                     QKeySequence::New);
    fileToolBar->addAction(newAct);
    mvActions[0].push_back(newAct);

    // Open Project
    QAction *openAct = qth::addAction(fileMenu,tr("&Open"),this, &MainApplication::openProject,
                   "document-open",tr("Open an existing project"),"../T1/assets/open.png",
                   QKeySequence::Open);
    fileToolBar->addAction(openAct);
    mvActions[0].push_back(openAct);

    // Save Project
    QAction *saveAct = qth::addAction(fileMenu,tr("&Save"),this, &MainApplication::saveProject,
                   "document-save",tr("Save the current project"),"../T1/assets/save.png",
                   QKeySequence::Save);
    fileToolBar->addAction(saveAct);
    mvActions[0].push_back(saveAct);

    // SaveAs Project
    QAction *saveAsAct = qth::addAction(fileMenu,tr("Save&As"),this, &MainApplication::saveAsProject,
                   "document-saveAs",tr("Save the current project in a new file"),"../T1/assets/saveAs.png",
                   QKeySequence::SaveAs);
    fileToolBar->addAction(saveAsAct);
    mvActions[0].push_back(saveAsAct);

    QAction *closeAct = qth::addAction(fileMenu,tr("&Close"),this, &MainApplication::closeProject,
                    "document-close",tr("Close the current project"),"../T1/assets/close.png",
                    QKeySequence::Close);
    mvActions[0].push_back(closeAct);

    fileMenu->addSeparator();
    fileToolBar->addSeparator();

    // EditMenu
    // each of those actions in the create a new MDI Widget that is independent from others and have its own dockable windows
    // except one action that proposes to close them all

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    mvActions.push_back(std::vector<QAction*>());

    // undoRedo
    fileToolBar->addAction(undoAction);
    editMenu->addAction(undoAction);
    mvActions[1].push_back(undoAction);
    fileToolBar->addAction(redoAction);
    editMenu->addAction(redoAction);
    mvActions[1].push_back(redoAction);
    editMenu->addSeparator();


    QAction* editMap = qth::addAction(editMenu,tr("Edit &Map"),this, &MainApplication::editMap,
                                    "edit-map",tr("Opens the map editor"),"",QKeySequence::UnknownKey);
    mvActions[1].push_back(editMap);
    fileToolBar->addAction(editMap);

    QAction* editAIFile = qth::addAction(editMenu,tr("Edit &AIFile"), this, &MainApplication::editAIObject,
                                    "edit-ai",tr("Edit AI object"), "", QKeySequence::UnknownKey);
    mvActions[1].push_back(editAIFile);

    QAction* editMission = qth::addAction(editMenu,tr("Edit Missio&n"), this, &MainApplication::editMission,
                                    "edit-mission",tr("Edit a mission"), "", QKeySequence::UnknownKey);
    mvActions[1].push_back(editMission);

    QAction* editPFF = qth::addAction(editMenu,tr("Edit &PFF file"), this, &MainApplication::editPFF,
                                    "edit-PFF",tr("Edit a Novalogic PFF File"), "", QKeySequence::UnknownKey);
    mvActions[1].push_back(editPFF);
    fileToolBar->addAction(editPFF);

    QAction* closeEditObjects = qth::addAction(editMenu, tr("&Close edited objects"), this, &MainApplication::closeEditedObjects,
                                    "close-edits",tr("Close all current edited objects"), "", QKeySequence::UnknownKey);
    mvActions[1].push_back(closeEditObjects);

    ////








    //// Only shortcut actions

    // The two next links are overriden by default by MDI settings so I removed
    // their implementation here.
    //QAction* openNextMDIWindow = new QAction(tr("Open next MDI Window"),this);
    //openNextMDIWindow->setShortcut(QKeySequence::NextChild);
    //QObject::connect(openNextMDIWindow, &QAction::triggered, mMdiArea, &QMdiArea::activateNextSubWindow);

    //QAction* openPreviousMDIWindow = new QAction(tr("Open previous MDI Window"),this);
    //openPreviousMDIWindow->setShortcut(QKeySequence::PreviousChild);
    //QObject::connect(openPreviousMDIWindow, &QAction::triggered, mMdiArea, &QMdiArea::activatePreviousSubWindow);



}


void MainApplication::createStatusBar(){
    statusBar()->showMessage(tr("Ready"));
}

void MainApplication::createDockWindows(){


}

void MainApplication::addUndoStackToGroup(QUndoStack* stack){
    if(!stack){return;}
    mUndoGroup->addStack(stack);
}





void MainApplication::onMdiActivation(QMdiSubWindow* window){
    static QMdiSubWindow* previous = nullptr;
    if(window!=previous){
        emit thisMdiDeactivated(previous);
        previous = window;
    }
    MyMdiSubWindow* cwindow = dynamic_cast<MyMdiSubWindow*>(window);
    if(cwindow){
        mUndoGroup->setActiveStack(cwindow->undoStack());
        return;
    }
    //deactivated, but could be usefull for routines in the back
    //for(QMdiSubWindow* win : mMdiArea->subWindowList()){
    //    if(win==window){
    //        // do nothing since "about to activate" exists
    //    } else {
    //        win->
    //    }
    //}
}





void MainApplication::nonImp(const QString & function){

    QMessageBox messageBox;
    messageBox.setText(tr("Sorry, the following function has not been implemented yet : \n \n")+function);
    messageBox.exec();
}













//// Generic GUI options

void MainApplication::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainApplication::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

//magn
//ginco bil
