/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputhelp.h"
#include "merginuserauth.h"
#include "merginuserinfo.h"
#include "merginsubscriptionstatus.h"
#include "merginapi.h"
#include "inpututils.h"

#include "qgsquickutils.h"

#include <QNetworkReply>
#include <QSysInfo>

const QString inputHelpRoot = "https://help.inputapp.io";
const QString merginHelpRoot = "https://help.cloudmergin.com";
const QString reportLogUrl = QStringLiteral( "https://opl1bkwxhg.execute-api.us-east-1.amazonaws.com/md_test_function" );
const QString helpDeskMail = QStringLiteral( "info@lutraconsulting.co.uk" );

InputHelp::InputHelp( MerginApi *merginApi, InputUtils *utils ):
  mMerginApi( merginApi ),
  mInputUtils( utils )
{
  emit linkChanged();
}

QString InputHelp::privacyPolicyLink() const
{
  return inputHelpRoot + "/privacy";
}

QString InputHelp::merginSubscriptionDetailsLink() const
{
  return merginHelpRoot + "/subscription";
}

QString InputHelp::howToEnableDigitizingLink() const
{
  return inputHelpRoot + "/howto/enable_digitizing";
}

QString InputHelp::howToEnableBrowsingDataLink() const
{
  return inputHelpRoot + "/howto/enable_browsing";
}

QString InputHelp::howToSetupThemesLink() const
{
  return inputHelpRoot + "/howto/setup_themes";
}

QString InputHelp::howToCreateNewProjectLink() const
{
  return inputHelpRoot + "/howto/project_config";
}

QString InputHelp::howToDownloadProjectLink() const
{
  return inputHelpRoot + "/howto/data_sync";
}

bool InputHelp::submitReportPending() const
{
  return mSubmitReportPending;
}

QString InputHelp::fullLog( bool isHtml )
{
  qint64 limit = 500000;
  QVector<QString> retLines;
  QFile file( InputUtils::logFilename() );
  if ( file.open( QIODevice::ReadOnly ) )
  {
    qint64 fileSize = file.size();
    if ( fileSize > limit )
      file.seek( file.size() - limit );

    QString line = file.readLine();
    while ( !line.isNull() )
    {
      retLines.push_back( line );
      line = file.readLine();
    }

    file.close();
  }
  else
  {
    return QString( "Unable to open log file %1" ).arg( InputUtils::logFilename() );
  }

  QString ret;
  // now add some extra info
  retLines.push_back( QStringLiteral( "------------------------------------------" ) );
  retLines.append( QgsQuickUtils().dumpScreenInfo().split( "\n" ).toVector() );
  retLines.push_back( QStringLiteral( "Screen Info:" ) );
  if ( !mMerginApi->userInfo()->email().isEmpty() )
  {
    retLines.push_back( QStringLiteral( "Mergin Data: %1/%2 Bytes" )
                        .arg( InputUtils::bytesToHumanSize( mMerginApi->userInfo()->diskUsage() ) )
                        .arg( InputUtils::bytesToHumanSize( mMerginApi->userInfo()->storageLimit() ) ) );
    retLines.push_back( QStringLiteral( "Subscription plan: %1" ).arg( mMerginApi->userInfo()->planAlias() ) );
    retLines.push_back( QStringLiteral( "Subscription Status: %1" ).arg( MerginSubscriptionStatus::toString( static_cast<MerginSubscriptionStatus::SubscriptionStatus>( mMerginApi->userInfo()->subscriptionStatus() ) ) ) );
  }
  else
  {
    retLines.push_back( QStringLiteral( "%1Mergin User Profile not available. To include it, open you Profile Page in InputApp%2" ).arg( isHtml ? "<b>" : "" ).arg( isHtml ? "</b>" : "" ) );
  }
  retLines.push_back( QStringLiteral( "Mergin User: %1" ).arg( mMerginApi->userAuth()->username() ) );
  retLines.push_back( QStringLiteral( "System: %1" ).arg( QSysInfo::prettyProductName() ) );
  retLines.push_back( QStringLiteral( "Mergin URL: %1" ).arg( mMerginApi->apiRoot() ) );
  retLines.push_back( QStringLiteral( "InputApp: %1 - %2" ).arg( InputUtils::appVersion() ).arg( InputUtils::appPlatform() ) );

  // now reverse so the most recent messages are on top and add separators
  std::reverse( std::begin( retLines ), std::end( retLines ) );
  int i = 0;
  for ( const QString &str : retLines )
  {
    ++i;
    if ( isHtml )
      ret += QStringLiteral( "<p class=\"%1\">" ).arg( i % 2 == 0 ? "odd" : "even" ) + str.trimmed() + "</p>";
    else
      ret += QString( i ) + str.trimmed() + "\n";
  }

  return ret;
}

void InputHelp::submitReport()
{
  // There is a limit of 1MB on the remote service, send less, let say half of that
  QString log = fullLog( false );
  QByteArray logArr = log.toUtf8();
  QString app = QStringLiteral( "input-%1-%2" ).arg( InputUtils::appPlatform() ).arg( InputUtils::appVersion() );
  QString username = mMerginApi->userAuth()->username().toHtmlEscaped();
  if ( username.isEmpty() )
    username = "unknown";
  QString params = QStringLiteral( "?app=%1&username=%2" ).arg( app ).arg( username );
  QNetworkRequest req( reportLogUrl + params );
  req.setRawHeader( "User-Agent", "InputApp" );
  req.setRawHeader( "Content-Type", "text/plain" );
  QNetworkReply *reply = mManager.post( req, logArr );
  qDebug() << "Report to " << reportLogUrl << endl;

  mSubmitReportPending = true;
  emit submitReportPendingChanged();
  connect( reply, &QNetworkReply::finished, this, &InputHelp::onSubmitReportReplyFinished );
}

void InputHelp::onSubmitReportReplyFinished()
{
  mSubmitReportPending = false;
  emit submitReportPendingChanged();

  QNetworkReply *r = qobject_cast<QNetworkReply *>( sender() );
  Q_ASSERT( r );

  if ( r->error() == QNetworkReply::NoError )
  {
    InputUtils::log( "submit report", "Report submitted!" );
    emit mInputUtils->showNotification( tr( "Report submitted.%1Please contact us on%1%2" ).arg( "<br />" ).arg( helpDeskMail ) );
  }
  else
  {
    InputUtils::log( "submit report", QStringLiteral( "FAILED - %1" ).arg( r->errorString() ) );
    emit mInputUtils->showNotification( tr( "Failed to submit report.%1Please check your internet connection." ).arg( "<br>" ) );
  }
}
