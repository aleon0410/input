/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "featuresmodel.h"

FeaturesModel::FeaturesModel( Loader &loader, QObject *parent )
  : QAbstractListModel( parent ),
    mLoader( loader )
{
}

QgsQuickFeatureLayerPair FeaturesModel::featureLayerPair( const int &featureId )
{
  for ( QgsQuickFeatureLayerPair i : mFeatures )
  {
    if ( i.feature().id() == featureId )
      return i;
  }
  return QgsQuickFeatureLayerPair();
}

int FeaturesModel::rowCount( const QModelIndex &parent ) const
{
  // For list models only the root node (an invalid parent) should return the list's size. For all
  // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
  if ( parent.isValid() )
    return 0;

  return mFeatures.count();
}

QVariant FeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFeatures.count() )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  const QgsQuickFeatureLayerPair feat = mFeatures.at( index.row() );

  switch ( role )
  {
    case FeatureTitle:
    {
      const QString title = mLoader.featureTitle( feat );
      if ( title.isEmpty() )
        return QVariant( feat.feature().id() );
      return QVariant( title );
    }
    case FeatureId: return QVariant( feat.feature().id() );
    case Description: return QVariant( QString( "Feature ID %1" ).arg( feat.feature().id() ) );
    case GeometryType: return QVariant( feat.feature().geometry().type() );
    case IconSource:
      switch ( feat.feature().geometry().type() )
      {
        case QgsWkbTypes::GeometryType::PointGeometry: return QVariant( "mIconPointLayer.svg" );
        case QgsWkbTypes::GeometryType::LineGeometry: return QVariant( "mIconLineLayer.svg" );
        case QgsWkbTypes::GeometryType::PolygonGeometry: return QVariant( "mIconPolygonLayer.svg" );
        default: return QVariant( "" );
      }
    default: return QVariant();
  }
}

void FeaturesModel::reloadDataFromLayer( QgsVectorLayer *layer )
{
  beginResetModel();
  mFeatures.clear();

  const int FEATURES_LIMIT = 100;

  if ( layer )
  {
    QgsFeatureRequest req;

    req.setLimit( FEATURES_LIMIT );

    QgsFeatureIterator it = layer->getFeatures( req );
    QgsFeature f;

    while ( it.nextFeature( f ) )
    {
      mFeatures << QgsQuickFeatureLayerPair( f, layer );
    }
  }

  endResetModel();
}

void FeaturesModel::activeProjectChanged()
{
  emptyData();
}

void FeaturesModel::activeMapThemeChanged( const QString &mapTheme )
{
  Q_UNUSED( mapTheme )
  emptyData();
}

void FeaturesModel::emptyData()
{
  beginResetModel();

  mFeatures.clear();

  endResetModel();
}

QHash<int, QByteArray> FeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[FeatureTitle] = QStringLiteral( "FeatureTitle" ).toLatin1();
  roleNames[FeatureId] = QStringLiteral( "FeatureId" ).toLatin1();
  roleNames[Description] = QStringLiteral( "Description" ).toLatin1();
  roleNames[GeometryType] = QStringLiteral( "GeometryType" ).toLatin1();
  roleNames[IconSource] = QStringLiteral( "IconSource" ).toLatin1();
  return roleNames;
}

bool FeaturesModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  // Mocked method - for future when attributes will be editable (it changes data)
  Q_UNUSED( index );
  Q_UNUSED( value );
  Q_UNUSED( role );
  return false;
}

Qt::ItemFlags FeaturesModel::flags( const QModelIndex &index ) const
{
  // Mocked method - for future when attributes will be editable (it checks if data is editable)
  if ( !index.isValid() )
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable;
}