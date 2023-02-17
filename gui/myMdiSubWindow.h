#ifndef MYMDISUBWINDOW_H
#define MYMDISUBWINDOW_H

#include <QMdiSubWindow>
#include <QUndoStack>

class MyMdiSubWindow : public QMdiSubWindow{
    //Q_OBJECT
protected:
    QUndoStack* mUndoStack;
public:
    QUndoStack* undoStack()const{return mUndoStack;}

//private slots:
    //virtual void onSave()=0;
    //virtual void onSaveAs()=0;
};




#endif // MYMDISUBWINDOW_H
