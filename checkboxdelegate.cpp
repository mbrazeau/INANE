#include <QCheckBox>
#include <QtSql>

#include "checkboxdelegate.h"

CheckboxDelegate::CheckboxDelegate(QObject *parent)
    : QItemDelegate{parent}
{
}

QWidget *CheckboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    if (!childModel)
        return QItemDelegate::createEditor(parent, option, index);

    QCheckBox *editor = new QCheckBox(parent);

    return editor;
}

void CheckboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(editor);
    if (!sqlModel || !checkBox) {
        QItemDelegate::setEditorData(editor, index);
        return;
    }
}

void CheckboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

}

void CheckboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

}

void CheckboxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect(option.rect.x(), option.rect.y(), 20, 20);
    drawCheck(painter, option, rect, /*index.data().toBool() ?*/ Qt::Checked /*: Qt::Unchecked*/);
    drawFocus(painter, option, rect);
}
