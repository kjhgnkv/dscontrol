// DevAndAppMimeGenerator.hpp

#pragma once

#include "nodeeditor/nodeeditor_export.h"
#include <QUuid>
#include <vector>

class QObject;
class QMimeData;
class QGraphicsItem;

namespace QtNodes
{
class UniversalSelectableItem;

/*!
\brief Class containing static methods for generating mimes for each type of
objects
*/
class NODEEDITOR_EXPORT MimeGenerator
{
public:
    /*!
    \brief Generates mime data for adding system on scene
     */
    static void generateSystemDragMime(QObject* parent);

    /*!
    \brief Generates mime data for adding subsystem on scene
     */
    static void generateSubsystemDragMime(QObject* parent);

    /*!
    \brief Generates mime data for adding computer on scene
     */
    static void generateComputerDragMime(QObject* parent);

    /*!
    \brief Generates mime data for adding application on scene
     */
    static void generateApplicationDragMime(QObject* parent);

    /*!
    \brief Generates mime data for adding external component on scene
     */
    static void generateExternalDragMime(QObject* parent);

    /*!
    \brief Generates mime data for adding text item on scene
     */
    static void generateTextDragMime(QObject* parent);

    /*!
    \brief Generates mime data for adding boundary frame on scene
     */
    static void generateFrameDragMime(QObject* parent);

    static QMimeData* generateCopyMime(QList<QGraphicsItem*> objects);
};
} // namespace QtNodes
