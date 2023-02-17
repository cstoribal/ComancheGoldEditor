

#include "mainApplication.h"
#include "mapEditor.h"
#include "pffEditor.h"

#include <QMessageBox>
#include <QFileDialog>

//Function slots from the main application for partial edition


QStringList MainApplication::maybeSelectFiles(const QString & dialogBoxTitle, const QString & informativeText,
                                              const QString & fileDialogTitle, const QString & fileFilter){
    QMessageBox msgBox;
    msgBox.setText(dialogBoxTitle);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Open | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Open);
    int ret = msgBox.exec();
    switch (ret) {
        case QMessageBox::No:
            // No was clicked - Starting immediately
            return QStringList({QString("")});
            break;
        case QMessageBox::Open:
            // Open was clicked
            {
                QFileDialog fileDialog(this,fileDialogTitle);
                fileDialog.setFileMode(QFileDialog::ExistingFiles);
                fileDialog.setNameFilter(fileFilter);
                fileDialog.exec();
                return fileDialog.selectedFiles();
            }
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked - returning before pff edition
            return QStringList();
            break;
        default:
            // should never be reached
            return QStringList();
            break;
    }
    return QStringList();
}


void MainApplication::editMap(){
    QStringList selectedFiles = maybeSelectFiles(tr("About to open map editor."),
                                                 tr("Do you wish to open an existing file?"),
                                                 tr("Select map to open"),
                                                 "Map files (*c.pcx *m.pcx *d.pcx);;All files (*)");
    for(const QString & selectedFile : selectedFiles){
        std::vector<char> data;
        if(selectedFile.size()>0){
            RawHexParser rawFile(selectedFile.toStdString());
            if(!rawFile.checkError()){
                data = rawFile.readData();
            } else {
                // error
            }
        } else {

        }

        MapEditor *mapEditor = new MapEditor(this,selectedFile,data);
        mUndoGroup->addStack(mapEditor->undoStack());
        mMdiArea->addSubWindow(mapEditor);


        connect(this,&MainApplication::thisMdiDeactivated,
                mapEditor,&MapEditor::deactivation);

        mapEditor->show();



    }


}

void MainApplication::onOpenMapEditorFromData(const std::vector<char> & data){
    MapEditor *mapEditor = new MapEditor(this,"new file",data);
    mUndoGroup->addStack(mapEditor->undoStack());
    mMdiArea->addSubWindow(mapEditor);


    connect(this,&MainApplication::thisMdiDeactivated,
            mapEditor,&MapEditor::deactivation);

    mapEditor->show();



}




void MainApplication::editPFF(){
    QStringList selectedFiles = maybeSelectFiles(tr("About to open PFF editor."),
                                                 tr("Do you wish to open an existing file?"),
                                                 tr("Select file to open"),
                                                 "pff formats (*.pff *.mrf);;All files (*)");

    for(const QString & file : selectedFiles){
    PffEditor *pffEditor = new PffEditor(file, this);
        mUndoGroup->addStack(pffEditor->undoStack());
        //TODO
        mMdiArea->addSubWindow(pffEditor);

        connect(this, &MainApplication::thisMdiDeactivated,
                pffEditor,&PffEditor::deactivation);

        connect(pffEditor, &PffEditor::eventOpenMapEditor,
                this, &MainApplication::onOpenMapEditorFromData);

        pffEditor->show();
    }



}
