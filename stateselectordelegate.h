#ifndef STATESELECTORDELEGATE_H
#define STATESELECTORDELEGATE_H

#include <QItemDelegate>
#include <QObject>
#include <QWidget>
#include <QMap>

class StateSelectorDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit StateSelectorDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;


private:


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // STATESELECTORDELEGATE_H
