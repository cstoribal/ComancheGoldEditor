#ifndef GUISETUP_H
#define GUISETUP_H


#include <QAction>
#include <QString>

namespace qth{

    template<typename T, typename U> QAction* addAction(
        QMenu *fileMenu,
        const QString & entry,
        T* linked, U action,
        const QString & theme,
        const QString & statusTip,
        const QString & iconPath,
        QKeySequence::StandardKey keysequence);




}

#include "guiSetup.hpp"



#endif // GUISETUP_H
