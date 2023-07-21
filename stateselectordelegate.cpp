#include <QAbstractItemView>
#include <QComboBox>
#include <QItemDelegate>
#include <QtSql>
#include <QDebug>
#include <QStringListModel>
#include <QPainter>

#include "stateselectordelegate.h"

StateSelectorDelegate::StateSelectorDelegate(QObject *parent)
    : QItemDelegate{parent}
{
}

/*This mostly borrows the code for QSqlRelationalDelegate verbatim with the exception that a filter is applied
  to the child relational model. This allows the delegate to display only the states relevant to the chosen
  character. */

QWidget *StateSelectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    if (!childModel)
        return QItemDelegate::createEditor(parent, option, index);

    QSqlRecord rec = sqlModel->record(index.row());

    childModel->setFilter(QString("character = %1 OR character IS NULL").arg(rec.field("id").value().toInt()));
    QComboBox *editor = new QComboBox(parent);
    editor->setModel(childModel);
    editor->setModelColumn(childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn()));
    editor->installEventFilter(const_cast<StateSelectorDelegate *>(this));
//    editor->setFrame(false);
    return editor;
}

void StateSelectorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!sqlModel || !combo) {
        QItemDelegate::setEditorData(editor, index);
        return;
    }
    combo->setCurrentIndex(combo->findText(sqlModel->data(index).toString()));
}

void StateSelectorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel *>(model);
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!sqlModel || !childModel || !combo) {
        QItemDelegate::setModelData(editor, model, index);
        return;
    }

    int currentItem = combo->currentIndex();
    int childColIndex = childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn());
    int childEditIndex = childModel->fieldIndex(sqlModel->relation(index.column()).indexColumn());
    sqlModel->setData(index,
                      childModel->data(childModel->index(currentItem, childColIndex), Qt::DisplayRole),
                      Qt::DisplayRole);
    sqlModel->setData(index,
                      childModel->data(childModel->index(currentItem, childEditIndex), Qt::EditRole),
                      Qt::EditRole);
}

void StateSelectorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

void StateSelectorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // TODO: This is the provisional conditional colour coder. Will need to allow user control.

//    QString label;

//    label = index.data().toString();

//    if (label == QString("missing")) {
//        painter->fillRect(option.rect, /*Qt::Dense7Pattern*/ QColor("dark gray"));
//    } else if (label == QString("inapplicable")) {
//        painter->fillRect(option.rect, /*Qt::BDiagPattern */ QColor("gray"));
//    } else if (label.toLower() == "absent") {
//        painter->fillRect(option.rect, QColor("blue"));
//    } else if (label.toLower() == "present") {
//        painter->fillRect(option.rect, QColor("dark cyan"));
//    }


    QItemDelegate::paint(painter, option, index);
}
