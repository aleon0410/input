#ifndef TESTMERGINAPI_H
#define TESTMERGINAPI_H

#include <QObject>

#include <qgsvectorlayer.h>
#include <merginapi.h>
#include <projectsmodel.h>
#include <merginprojectmodel.h>

#include <qgsapplication.h>

class TestMerginApi: public QObject
{
    Q_OBJECT
 public:
    explicit TestMerginApi(MerginApi* api, MerginProjectModel* mpm, ProjectModel* pm, QObject* parent = nullptr);
    ~TestMerginApi() = default;

  public slots:
    void initTestCase();
    void testListProject();
    void testDownloadProject();
    void cleanupTestCase();

private:
    MerginApi *mApi;
    MerginProjectModel *mMerginProjectModel;
    ProjectModel* mProjectModel;
    QString mApiRoot;
    QString mUsername;
    QString mPassword;

    QString mProjectName;
};

# endif // TESTMERGINAPI_H
