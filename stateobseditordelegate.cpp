#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include "stateobseditordelegate.h"

StateObsEditorDelegate::StateObsEditorDelegate(QWidget *parent) : QSqlRelationalDelegate (parent)
{

}

void StateObsEditorDelegate::setStateTable(QSqlRelationalTableModel &statesTable)
{
    m_statesTable = &statesTable;
}

void StateObsEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
//
    int charID;
    const QSqlRelationalTableModel *model = qobject_cast<const QSqlRelationalTableModel *>(index.model());

    if (model) {
        qDebug() << "Index row: " << index.row();
        bool ok;
        charID = model->record(index.row()).value("character").toInt(&ok);
        QSqlQuery query;

        if(!query.exec(QString("SELECT character FROM observations"))){
            qDebug() << "Something is fundamentally wrong with this query string";
        }
        query.next();

        if (!ok) {
            qDebug() << "Something went wrong with the selection!";
        }

        qDebug() << model->tableName();

        m_statesTable->setFilter(QString("character = %1 OR label = 'inapplicable' OR label = 'missing'").arg(charID));

        qDebug() << "Char ID: " << charID;
//        QSqlRelationalDelegate::setEditorData(editor, index);
    }

//    if (QComboBox *comboBox = qobject_cast<QComboBox *>(editor)) {
////        comboBox->model();
////        qDebug() << "Index row: " << index.row();
//        QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel *>(comboBox->model());
//        comboBox->clear();
//        if (model)
//        {
//            qDebug() << "Index row: " << index.row();
//            int charID;
//            charID = model->record(index.row()).value("character").toInt();
//            model->setFilter(QString("character = '%1 OR label = 'inapplicable' OR label = 'missing'").arg(charID));
//        }
//    }
}
