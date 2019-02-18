#include "occmainwindow.h"
#include "ui_occmainwindow.h"
#include "utils.h"
#include "parametersetmodel.h"
#include "commandexecutedialog.h"

#define OCC_CMD "OpenCppCoverage.exe"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QInputDialog>

OccMainWindow::OccMainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OccMainWindow)
    , m_addMenu(nullptr)
    , m_lvParamMenu(nullptr)
    , m_lastAction(nullptr)
    , m_model(new ParameterSetModel(this))
    , m_dialog(new CommandExecuteDialog(this))
{
    ui->setupUi(this);
    m_addMenu = new QMenu(this);
    m_lvParamMenu = new QMenu(this);

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
    path = QFileDialog::getOpenFileName(this, "Select", qApp->applicationDirPath(), OCC_CMD);

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
    // For Add button
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
    m_actions[ActionTypeAddWorkDirectory] = m_addMenu->addAction (IC_SET->ic_workdir,
                                            "添加工作目录", this, &OccMainWindow::OnAddWorkDir);


    // For lvParameter
    m_lvParamMenu->addAction (IC_SET->ic_edit, "编辑", [&]
    {
        auto index = ui->lvParameters->currentIndex ();

        if (index.isValid ())
        {
            ui->lvParameters->edit (index);
        }
    });

    m_lvParamMenu->addAction (IC_SET->ic_delete, "删除", [&]
    {
        OnRemoveParameter(ui->lvParameters->currentIndex ());
    });
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
        this->SetLastAction (act);
    });
    connect (ui->btnAddMenu, &QToolButton::clicked, [&]
    {
        if (m_lastAction != nullptr && m_lastAction->isEnabled ())
        {
            m_lastAction->trigger ();
        }
    });
    connect (ui->lvParameters, &QListView::doubleClicked, this, &OccMainWindow::OnRemoveParameter);

    connect (ui->lvParameters, &QListView::customContextMenuRequested, this, &OccMainWindow::ShowLvParameterMenu);
}

void OccMainWindow::SetLastAction(QAction *act)
{
    if (!act)
    {
        return;
    }

    m_lastAction = act;

    if (!m_lastAction->isEnabled ())
    {
        ui->btnAddMenu->setIcon (IC_SET->ic_select);
        ui->btnAddMenu->setText ("请选择操作");
    }
    else
    {
        ui->btnAddMenu->setIcon (act->icon ());
        ui->btnAddMenu->setText (act->text ());
    }

    // if all required parameters are set, the analysis button will enabled
    if (!ui->leOccPath->text ().isEmpty () && // OpenCppCoverage.exe required
            !m_actions[ActionTypeAddExecutable]->isEnabled () && // guest program
            !m_actions[ActionTypeAddExport]->isEnabled () && // export path
            !m_actions[ActionTypeAddWorkDirectory]->isEnabled ()) // work directory
    {
        ui->btnAnalysis->setEnabled (true);
    }
    else
    {
        ui->btnAnalysis->setEnabled (false);
    }
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
        m_lastAction->setEnabled (false);
    }

    SetLastAction (m_lastAction);
}

void OccMainWindow::OnAddExecutable()
{
    QString path = QFileDialog::getOpenFileName (this, "请选择要分析的可执行文件", QDir::currentPath (), "*.exe");

    if (!path.isEmpty ())
    {
        m_model->AddParameter (ActionTypeAddExecutable, path);
        m_lastAction->setEnabled (false);
    }

    SetLastAction (m_lastAction);
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

void OccMainWindow::OnAddWorkDir()
{
    QString path = QFileDialog::getExistingDirectory (this, "请选择程序工作目录", QDir::currentPath ());

    if (!path.isEmpty ())
    {
        m_model->AddParameter (ActionTypeAddWorkDirectory, path);
        m_lastAction->setEnabled (false);
    }

    SetLastAction (m_lastAction);
}

void OccMainWindow::ShowLvParameterMenu(const QPoint &pos)
{
    Q_UNUSED (pos)
    m_lvParamMenu->exec (QCursor::pos());
}

void OccMainWindow::OnRemoveParameter(const QModelIndex &index)
{
    if (!index.isValid ())
    {
        return;
    }

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

    SetLastAction (m_lastAction);
}

void OccMainWindow::on_btnReset_clicked()
{
    m_model->Reset ();

    for (QAction *act : m_actions)
    {
        act->setEnabled (true); // Renable All actions
    }

    ui->btnAnalysis->setEnabled (false);
}

void OccMainWindow::on_btnAnalysis_clicked()
{
    QString work_dir = m_model->GetWorkDirectory ();
    QStringList env = m_model->GetProgramEnv ();
    QStringList args;
    m_model->BuildArguments (args);
    m_dialog->OpenDialog (ui->leOccPath->text (), work_dir, env, args);
}
