#pragma once
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDir>
#include <QFileInfoList>
#include <QMessageBox>

class GameSelectionDialog : public QDialog {
    Q_OBJECT
public:
    explicit GameSelectionDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Select Game");
        setMinimumSize(400, 300);
        setStyleSheet("background: #3E2723; color: #F5E6D3; font-family: 'Times New Roman';");

        QVBoxLayout* layout = new QVBoxLayout(this);
        
        QLabel* title = new QLabel("Load a Saved Game or Start New", this);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFD700; margin-bottom: 10px;");
        title->setAlignment(Qt::AlignCenter);
        layout->addWidget(title);

        m_listWidget = new QListWidget(this);
        m_listWidget->setStyleSheet("background: #4E342E; border: 2px solid #8B4513; font-size: 14px;");
        layout->addWidget(m_listWidget);

        QHBoxLayout* btnLayout = new QHBoxLayout();
        
        m_btnNew = new QPushButton("Start New Game", this);
        styleButton(m_btnNew, "#2E7D32"); // Green
        
        m_btnLoad = new QPushButton("Load Selected", this);
        styleButton(m_btnLoad, "#1565C0"); // Blue
        m_btnLoad->setEnabled(false);

        m_btnDelete = new QPushButton("Delete", this);
        styleButton(m_btnDelete, "#B71C1C"); // Red
        m_btnDelete->setEnabled(false);

        btnLayout->addWidget(m_btnNew);
        btnLayout->addWidget(m_btnDelete);
        btnLayout->addWidget(m_btnLoad);
        
        layout->addLayout(btnLayout);

        connect(m_btnNew, &QPushButton::clicked, this, &GameSelectionDialog::onNewGame);
        connect(m_btnLoad, &QPushButton::clicked, this, &GameSelectionDialog::onLoadGame);
        connect(m_btnDelete, &QPushButton::clicked, this, &GameSelectionDialog::onDeleteGame);
        connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &GameSelectionDialog::onSelectionChanged);

        refreshList();
    }

    QString getSelectedFile() const { return m_selectedFile; }
    bool isNewGame() const { return m_isNewGame; }

private slots:
    void onSelectionChanged() {
        bool hasSel = !m_listWidget->selectedItems().isEmpty();
        m_btnLoad->setEnabled(hasSel);
        m_btnDelete->setEnabled(hasSel);
    }

    void onNewGame() {
        m_isNewGame = true;
        accept();
    }

    void onLoadGame() {
        if (m_listWidget->selectedItems().isEmpty()) return;
        m_selectedFile = "saves/" + m_listWidget->selectedItems().first()->text();
        m_isNewGame = false;
        accept();
    }

    void onDeleteGame() {
        if (m_listWidget->selectedItems().isEmpty()) return;
        QString itemText = m_listWidget->selectedItems().first()->text();
        QString path = "saves/" + itemText;
        
        if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete " + itemText + "?") == QMessageBox::Yes) {
            QFile::remove(path);
            refreshList();
        }
    }

private:
    QListWidget* m_listWidget;
    QPushButton* m_btnNew;
    QPushButton* m_btnLoad;
    QPushButton* m_btnDelete;
    QString m_selectedFile;
    bool m_isNewGame = true;

    void refreshList() {
        m_listWidget->clear();
        QDir dir("saves");
        if (!dir.exists()) dir.mkpath(".");
        
        QStringList filters;
        filters << "*.7wd";
        dir.setNameFilters(filters);
        
        QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
        for (const auto& info : list) {
            m_listWidget->addItem(info.fileName());
        }
        onSelectionChanged();
    }

    void styleButton(QPushButton* btn, QString color) {
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(QString(
            "QPushButton { background: %1; color: white; padding: 8px; border-radius: 5px; font-weight: bold; border: 1px solid #000; }"
            "QPushButton:hover { background: white; color: %1; }"
            "QPushButton:disabled { background: #555; color: #888; border: 1px solid #555; }"
        ).arg(color));
    }
};
