#ifndef CHARACTEREDITORWINDOW_H
#define CHARACTEREDITORWINDOW_H

#include <QWidget>

class QDialogButtonBox;
class QDialogButtonBox;
class QGridLayout;
class QHBoxLayout;
class QTableView;
class QSqlRelationalTableModel;
class QSqlDatabase;
class QLabel;
class QLineEdit;
class QTextEdit;
class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QPushButton;
class QModelIndex;
class QToolBar;
class QToolButton;
class QVBoxLayout;

class CharacterEditorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CharacterEditorWindow(QWidget *parent = nullptr);


private:
    QGridLayout *charWindLayout;
    QTableView  *charTableView;
    QSqlRelationalTableModel *charTable_p;
    QSqlRelationalTableModel *statesTable_p;
    QWidget *editorArea;

    QDataWidgetMapper *mapper;

    QLabel *labelLabel;
    QLineEdit *labelField;
    QLabel *statesLabel;
    QTableView *statesTable;
    QLabel *sourceLabel;
    QLineEdit  *sourceField;
    QLabel *descripLabel;
    QTextEdit  *descripField;

    QDialogButtonBox *charListButtons;
    QDialogButtonBox *stateListButtons;
    QDialogButtonBox *charDataButtons;

    QToolBar *stateTools;
    QToolButton *moveStateUp;
    QToolButton *moveStateDn;
    QToolButton *newState;
    QToolButton *deleteState;

    QPushButton *newChar;
    QPushButton *deleteChar;
    QPushButton *submitButton;
    QPushButton *cancelButton;

    void setCharTable();
    void initEditorArea();
    void populateEditor();

    void newCharacterAction();
    void deleteCharAction();
    void commitCharChange();
    void newStateAction();
    void deleteStateAction();
    void filterStatesByChar(const QModelIndex &index);

//private slots:

    void onCharacterClicked(const QModelIndex &index);

signals:

};

#endif // CHARACTEREDITORWINDOW_H
