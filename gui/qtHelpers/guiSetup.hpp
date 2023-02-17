#ifndef GUISETUP_HPP
#define GUISETUP_HPP

#include "guiSetup.h"


#include <QMenuBar>
#include <QIcon>

#include <QToolBar>

///
/// \brief qth::addAction
/// adds an action to $fileMenu with name $entry, that calls, in the $linked object, a $slot.
/// If defined, a QIcon is created, with a $theme, a $status-tip help and the icon linked by $iconpath.
/// Finally, a $keySequence may be bound to the action.
///
/// \param fileMenu existing QMenu object
/// \return newly created QAction*. Object management is to handled by $fileMenu
///
template<typename T, typename U> QAction* qth::addAction(
        QMenu *fileMenu,
        const QString & entry,
        T* linked, U slot,
        const QString & theme,
        const QString & statusTip,
        const QString & iconPath,
        QKeySequence::StandardKey keySequence){
    QAction *action;
    if(iconPath == ""){
        action = new QAction(entry, linked);
    } else {
        const QIcon icon = QIcon::fromTheme(theme, QIcon(iconPath));
        action = new QAction(icon, entry, linked);
    }
    action->setShortcuts(keySequence);
    action->setStatusTip(statusTip);
    QObject::connect(action, &QAction::triggered, linked, slot);
    if(fileMenu){
        fileMenu->addAction(action);
    }
    return action;
}















#endif // GUISETUP_HPP
