#ifndef PFFEDITORACTIONS_H
#define PFFEDITORACTIONS_H

#include "pffEditor.h"
#include "tools/textMgmt.h"
#include <QUndoCommand>


class ActpffMoveFile : public QUndoCommand{
    int idx1,idx2;
    PffEditor* pffEditor;

public:
    ActpffMoveFile(PffEditor* _pffEditor, int _idx1, int _idx2):pffEditor(_pffEditor),idx1(_idx1),idx2(_idx2){
        setText(QString::fromStdString("MoveFile "+std::to_string(idx1)+" to "+std::to_string(idx2)));
    };

    enum { Id = 10011 };
    void undo() override{
        pffEditor->actMoveItem(idx2,idx1);
    };
    void redo() override{
        pffEditor->actMoveItem(idx1,idx2);
    };
    virtual bool mergeWith(const QUndoCommand *command) override{
        const ActpffMoveFile* moveAction = dynamic_cast<const ActpffMoveFile*>(command);
        if(moveAction->idx1 != idx2){
            return false;
        } else {
           idx2 = moveAction->idx2;
           if(idx2==idx1){
               setObsolete(true);
           }
           return true;
        }
    }


    int id() const override { return Id; }
};


class ActpffInsertFile : public QUndoCommand{
    int idxInsert;
    std::string filename;
    //--- more data ?
    std::vector<char> dataInserted;

    PffEditor* pffEditor;
public:
    ActpffInsertFile(PffEditor* _pffEditor, int _idx,
                     std::string _filename, const std::vector<char> & _data):
          pffEditor(_pffEditor),idxInsert(_idx),filename(usr::parse::getFileWithoutPath(_filename)),dataInserted(_data){
        // extract filename with usr::parse

        setText(QString::fromStdString("Insert "+filename+" to "+std::to_string(idxInsert)));
    }
    void undo() override{
        pffEditor->actRemoveItem(idxInsert);
    };
    void redo() override{
        pffEditor->actInsertItem(filename,idxInsert,dataInserted);
    };
};

class ActpffRemoveFile : public QUndoCommand{
    int idxRemove;
    std::string filename;
    std::vector<char> dataRemoved;
    PffEditor* pffEditor;
public:
    ActpffRemoveFile(PffEditor* _pffEditor, int _idx,
                     std::string _filename, const std::vector<char> & _data):
        pffEditor(_pffEditor),idxRemove(_idx),
        filename(usr::parse::getFileWithoutPath(_filename)),dataRemoved(_data){
        setText(QString::fromStdString("Remove "+filename+" from "+std::to_string(idxRemove)));
    }

    void undo() override{
        pffEditor->actInsertItem(filename,idxRemove,dataRemoved);
    }
    void redo() override{
        pffEditor->actRemoveItem(idxRemove);
    }
};
























#endif // PFFEDITORACTIONS_H
