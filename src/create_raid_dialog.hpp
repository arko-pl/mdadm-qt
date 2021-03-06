#ifndef CREATE_RAID_DIALOG_HPP
#define CREATE_RAID_DIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QMap>

class QListView;
class QComboBox;
class QSpinBox;

struct IFileSystem;

class CreateRaidDialog : public QDialog
{
    QListView* m_disksView;
    QListView* m_selectedDisksView;
    QStandardItemModel m_disksModel;
    QStandardItemModel m_selectedDisksModel;

    QComboBox *m_cbTypes;
    QSpinBox *m_sbDevNumber;

    const QMap<QString, int> m_raidTypes;

    void addElements();
    void removeElements();

    void recalculateType(int count);

public:
    CreateRaidDialog(IFileSystem *, QWidget *parent = Q_NULLPTR);
    CreateRaidDialog(const CreateRaidDialog &) = delete;
    CreateRaidDialog& operator=(const CreateRaidDialog &) = delete;

    QStringList getSelectedDisks() const;
    QString getType() const;
    unsigned getMDNumber() const;
};

#endif // CREATE_RAID_DIALOG_HPP
