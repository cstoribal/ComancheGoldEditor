#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QWidget>
#include "myMdiSubWindow.h"

#include <QUndoStack>
#include <QUndoGroup>

#include <QUndoCommand>
#include <QDockWidget>
#include <QMainWindow>

#include <QLabel>

class Elevate : public QUndoCommand{

public:
    Elevate(){setText("elevation");};

    enum { Id = 1234 };
    void undo() override{};
    void redo() override{};
    virtual bool mergeWith(const QUndoCommand *command) override{
        return false;
    }

    int id() const override { return Id; }

};







class MapEditor: public MyMdiSubWindow
{
    Q_OBJECT
    QLabel* mMapRepresentation;
    QString mFilename;

public:
    MapEditor(QMainWindow* mainWin, const QString & title, const std::vector<char> & data);

private:
    QDockWidget *mDock = nullptr;




    void init();
    void initActivation();

public slots:
    void deactivation(QMdiSubWindow* window);

    //void onSave()override{};
    //void onSaveAs()override{};

};

#endif // MAPEDITOR_H
