#include <QCheckBox>
#include <QtSql>

#include "checkboxdelegate.h"

CheckboxDelegate::CheckboxDelegate(QObject *parent)
    : QItemDelegate{parent}
{
}

bool CheckboxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonRelease){
        model->setData(index, !model->data(index).toBool());
        model->submit();
        event->accept();
    }
    return QItemDelegate::editorEvent(event, model, option, index);
}

void CheckboxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    drawCheck(painter, option, option.rect, index.data().toBool() ? Qt::Checked : Qt::Unchecked);
    drawFocus(painter, option, option.rect);
}
