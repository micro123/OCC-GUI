#include "occmainwindow.h"
#include "ui_occmainwindow.h"
#include "utils.h"
#include "parametersetmodel.h"
#include "commandexecutedialog.h"

#define OCC_CMD "OpenCppCoverage"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QInputDialog>

OccMainWindow::OccMainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OccMainWindow)
    , m_addMenu(nullptr)
    , m_lastAction(nullptr)
    , m_model(new ParameterSetModel(this))
    , m_dialog(new CommandExecuteDialog(this))
{
    ui->setupUi(this);
    m_addMenu = new QMenu(this);

    InitMenus ();
    InitViews ();
    InitConnections ();

    // 启动时尝试自动获取OpenCppCoverage路径
    on_btnAutoDetect_clicked ();
}

OccMainWindow::~OccMainWindow()
{
    delete ui;
}

void OccMainWindow::on_btnAutoDetect_clicked()
{
    QString path;

    if (Utils::GetSystemExecutablePath(OCC_CMD, path))
    {
        ui->leOccPath->setText(path);
    }
    else
    {
        QMessageBox::critical(this, "错误", "我们无法检测系统中的OpenCppCoverage.exe的位置");
        ui->leOccPath->clear();
    }
}

void OccMainWindow::on_btnBrowser_clicked()
{
    QString path;
    path = QFileDialog::getOpenFileName(this, "Select", qApp->applicationDirPath(), "OpenCppCoverage.exe");

    if (!path.isEmpty())
    {
        ui->leOccPath->setText(path);
    }
    else
    {
        ui->leOccPath->clear();
    }
}

void OccMainWindow::InitMenus()
{
    // TODO 添加处理

    m_actions[ActionTypeAddPath] = m_addMenu->addAction (IC_SET->ic_variable,
                                   "添加Path路径", this, &OccMainWindow::OnAddPath);
    m_actions[ActionTypeAddExecutable] = m_addMenu->addAction (IC_SET->ic_program,
                                         "添加可执行文件路径", this, &OccMainWindow::OnAddExecutable);
    m_actions[ActionTypeAddSource] = m_addMenu->addAction (IC_SET->ic_source,
                                     "添加源码路径", this, &OccMainWindow::OnAddSourceDir);
    m_actions[ActionTypeAddExcludeSource] = m_addMenu->addAction (IC_SET->ic_ignore,
                                            "添加排除源码路径", this, &OccMainWindow::OnAddExcludedSourceDir);
    m_actions[ActionTypeAddModule] = m_addMenu->addAction (IC_SET->ic_module,
                                     "添加要分析的模块名", this, &OccMainWindow::OnAddModule);
    m_actions[ActionTypeAddExport] = m_addMenu->addAction (IC_SET->ic_export,
                                     "添加导出路径", this, &OccMainWindow::OnAddExport);
}

void OccMainWindow::InitViews()
{
    ui->btnAddMenu->setMenu (m_addMenu);

    ui->lvParameters->setModel (m_model);

}

void OccMainWindow::InitConnections()
{
    connect (ui->btnAddMenu, &QToolButton::triggered, [&](QAction * act)
    {
        if (act == m_actions[ActionTypeAddExecutable] || act == m_actions[ActionTypeAddExport])
        {
            ui->btnAddMenu->setText ("请选择操作");
            m_lastAction = nullptr;
            act->setEnabled (false);
            ui->btnAddMenu->setIcon(IC_SET->ic_select);
        }
        else
        {
            m_lastAction = act;
            ui->btnAddMenu->setText (act->text ());
            ui->btnAddMenu->setIcon (act->icon ());
        }
    });
    connect (ui->btnAddMenu, &QToolButton::clicked, [&]
    {
        if (m_lastAction != nullptr)
        {
            m_lastAction->trigger ();
        }
    });
    connect (ui->lvParameters, &QListView::doubleClicked, [&](const QModelIndex & index)
    {
        auto btn = QMessageBox::question (this, "消息", "确定要移除这个参数吗?");

        if (btn == QMessageBox::Yes)
        {
            // 判断是不是程序或者导出目录
            if (index.data (Qt::UserRole).canConvert<int>())
            {
                int type = index.data (Qt::UserRole).value<int>();

                if (type == ActionTypeAddExport)
                {
                    m_actions[ActionTypeAddExport]->setEnabled (true);
                }
                else if (type == ActionTypeAddExecutable)
                {
                    m_actions[ActionTypeAddExecutable]->setEnabled (true);
                }
            }

            m_model->RemoveAt (index.row ());
        }
    });
}

void OccMainWindow::OnAddPath()
{
    QString path = QFileDialog::getExistingDirectory (this, "请选择附加搜索目录");

    if (!path.isEmpty ())
    {
        m_model->AddParameter (ActionTypeAddPath, path);
    }
}

void OccMainWindow::OnAddExport()
{
    QString path = QFileDialog::getExistingDirectory (this, "请选择导出目录");

    if (!path.isEmpty ())
    {
        m_model->AddParameter (ActionTypeAddExport, path);
    }
    else
    {
        m_actions[ActionTypeAddExport]->setEnabled (true);
    }
}

void OccMainWindow::OnAddExecutable()
{
    QString path = QFileDialog::getOpenFileName (this, "请选择要分析的可执行文件", QDir::currentPath (), "*.exe");

    if (!path.isEmpty ())
    {
        m_model->AddParameter (ActionTypeAddExecutable, path);
    }
    else
    {
        m_actions[ActionTypeAddExecutable]->setEnabled (true);
    }
}

void OccMainWindow::OnAddModule()
{
    QString names = QInputDialog::getText (this, "输入模块名", "多个可使用空格分割:");

    if (!names.isEmpty ())
    {
        QStringList all_modules = names.trimmed ().split (QRegExp(R"(\s+)"));
        QSet<QString> mm;

        for (auto &x : all_modules)
        {
            mm.insert (x);
        }

        for (auto &x : mm)
        {
            m_model->AddParameter (ActionTypeAddModule, x);
        }
    }
}

void OccMainWindow::OnAddSourceDir()
{
    QString path = QFileDialog::getExistingDirectory (this, "请选择源代码目录");

    if (!path.isEmpty ())
    {
        m_model->AddParameter (ActionTypeAddSource, path);
    }
}

void OccMainWindow::OnAddExcludedSourceDir()
{
    QString path = QFileDialog::getExistingDirectory (this, "请选择要排除的源代码目录");

    if (!path.isEmpty ())
    {
        m_model->AddParameter (ActionTypeAddExcludeSource, path);
    }
}

void OccMainWindow::on_btnReset_clicked()
{
    m_model->Reset ();
}

void OccMainWindow::on_pushButton_clicked()
{
    QStringList env = m_model->GetProgramEnv ();
    QStringList args;
    m_model->BuildArguments (args);
    m_dialog->OpenDialog (ui->leOccPath->text (), env, args);
}
