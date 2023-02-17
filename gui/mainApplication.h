#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <QMainWindow>

#include <QMdiArea>
#include <QSizeGrip>
#include <QFileSystemWatcher>

#include <QMenuBar>

#include <QToolBar>
#include <QString>
#include <QUndoGroup>

#define NI {nonImp(__func__);}
#define NI2 {;}  //Silent non implemented function


#include "myMdiSubWindow.h"



class MainApplication : public QMainWindow
{
    Q_OBJECT

    QMenuBar* mMenuBar = nullptr;
    QMdiArea* mMdiArea = nullptr;

    QSizeGrip* mSizeGrip = nullptr;
    QStatusBar* mStatusBar = nullptr;

    std::vector<std::vector<QAction*>> mvActions;

    QFileSystemWatcher mFileSystemWatcher; //TODO allow to ask to «reload files» whenever they are modified



public:
    explicit MainApplication(QWidget *parent = nullptr);

    void addUndoStackToGroup(QUndoStack* stack);


/////////////////////////
private slots:
    void newProject() NI
    void openProject()NI
    void saveProject() NI
    void saveAsProject() NI
    void closeProject() NI
    //--------------------



/////////////////////////
//TODO mettre les chemins d’accès des modèles éventuels

    void editMap();
    void onOpenMapEditorFromData(const std::vector<char> & data);
    void editAIObject() NI
    void editMission()  NI
    //-------------
    void editPFF();
    void closeEditedObjects() NI

/////////////////////////
//
    void openPffInterface() NI


    void updateMenus() NI2

    // Deactivate all unselected Mdi Windows
    // may be simplified by keeping track of the current
    // active MDI in a static pointer.
    void onMdiActivation(QMdiSubWindow* window);

private:
    void createActions();
    void createStatusBar();
    void createDockWindows();

    QStringList maybeSelectFiles(const QString & dialogBoxTitle, const QString & informativeText, const QString & fileDialogTitle, const QString & fileFilter);
    bool maybeSave();
    void nonImp(const QString & function="");

    void readSettings();
    void writeSettings();

    QUndoGroup* mUndoGroup;
    QDockWidget* mLeftDock,*mRightDock;

signals:
    void thisMdiDeactivated(QMdiSubWindow* window);
};

#endif // MAINAPPLICATION_H
