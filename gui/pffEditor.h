#ifndef PFFEDITOR_H
#define PFFEDITOR_H

#include "io/pffParser.hpp"
#include <QListView>
#include <QTreeView>
#include <QTextEdit>
#include <QWidget>
#include "myMdiSubWindow.h"
#include <QMainWindow>
#include <QUndoStack>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>


// Basic composition of an editor include :
// - The fact that it’s a MdiSubWindow
// - A QUndoStack to return and add to the main
// - A ToolDock
// - A Deactivation(QMdiSubWindow*) slot to hide
// - A whenClosed to delete
// - An initActivate() to link all actions performed when activated

class TreeViewPff: public QTreeView{
    Q_OBJECT;
public:
    TreeViewPff(QWidget* parent):QTreeView(parent){
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(ShowContextMenu(const QPoint &)));
        setDragEnabled(true);
        setAcceptDrops(true);
        setDropIndicatorShown(true);
        setFocusPolicy(Qt::NoFocus);
    };

protected:
    //void dragEnterEvent(QDragEnterEvent *event) override;
    //void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    //void startDrag(Qt::DropActions supportedActions) override;


    void showContextMenu(){
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Remove Data Point", this);
        //connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
        contextMenu.addAction(&action1);

        QAction* selectedAction = contextMenu.exec();
    }

signals:
    void movedItem(int from, int to);

};


//TODO Qt::NoFocus on


// TODO : Prevent this class from grabbing the keyboard shotcuts especially Ctrl Z Ctrl Y
class HexTextEdit : public QTextEdit{
    Q_OBJECT
public:
    template<typename ...Args> HexTextEdit(Args ...args):QTextEdit(args...){};
protected:
    //void keyPressEvent(QKeyEvent *e)override;
    //bool eventFilter(QObject *o, QEvent *e )override;
protected slots:

};


class PffEditor: public MyMdiSubWindow
{
    Q_OBJECT

    QString mFileName;
    QLabel* mFileList;
    NovalogicPffParser mFileParser;
    bool f_fileModified;

    void init();
    void initActivate();


public:
    PffEditor(const QString & file, QMainWindow* mainWin);

    bool modified()const{return f_fileModified;}


private:
    QDockWidget *mToolDock = nullptr;
    QDockWidget *mFilesDock = nullptr;

    QVBoxLayout *mToolDockLayout=nullptr;
    HexTextEdit *mTextEdit = nullptr;

    QPushButton* mToggleEditionButton=nullptr;
    QPushButton* mToggleViewButton=nullptr;

    TreeViewPff *mTreeView = nullptr;

    void refreshFileState();
    void setModified(bool isModified=true);


public slots:
    void deactivation(QMdiSubWindow* window);
    void whenClosed();
    void onTreeViewFileClick(const QModelIndex &index);
    void onTreeViewFileRightClick(const QModelIndex &index);

    void onToggleEditionMode(){
        if(mToggleEditionButton->isChecked()){
            mTextEdit->setFocusPolicy(Qt::StrongFocus);
        } else {
            mTextEdit->setFocusPolicy(Qt::NoFocus);
        }
    };

    void moveItem(int from, int to);
    void exportItemToFile();

    void onSave();//override;
    void onSaveAs();//override;
    void onAppendItem();
    void onInsertItem();
    void onRemoveItem();

    void onOpenMap();



signals:
    void onDeleteKeyPressed();
    void eventOpenMapEditor(const std::vector<char> & mapData);

protected:
    void actMoveItem(int from, int to);
    void actInsertItem(std::string filename, int idx, const std::vector<char> & data);
    void actRemoveItem(int idx);

    void keyPressEvent(QKeyEvent * event)override;

    //Actions
    friend class ActpffMoveFile;
    friend class ActpffInsertFile;
    friend class ActpffRemoveFile;

};

#endif // PFFEDITOR_H
