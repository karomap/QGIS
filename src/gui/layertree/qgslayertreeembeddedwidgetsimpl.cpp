/***************************************************************************
  qgslayertreeembeddedwidgetsimpl.h
  --------------------------------------
  Date                 : May 2016
  Copyright            : (C) 2016 by Martin Dobias
  Email                : wonder dot sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgslayertreeembeddedwidgetsimpl.h"

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QTimer>

#include "qgsrasterlayer.h"
#include "qgsrasterrenderer.h"
#include "qgsvectorlayer.h"


///@cond PRIVATE

QgsLayerTreeOpacityWidget::QgsLayerTreeOpacityWidget( QgsMapLayer *layer )
  : mLayer( layer )
{
  setAutoFillBackground( true ); // override the content from model
  QLabel *l = new QLabel( QStringLiteral( "Opacity" ), this );
  mSlider = new QSlider( Qt::Horizontal, this );
  mSlider->setRange( 0, 1000 );
  int sliderW = static_cast< int >( QFontMetricsF( font() ).width( 'X' ) * 16 * Qgis::UI_SCALE_FACTOR );
  mSlider->setMinimumWidth( sliderW / 2 );
  mSlider->setMaximumWidth( sliderW );
  QHBoxLayout *lay = new QHBoxLayout();
  QSpacerItem *spacerItem = new QSpacerItem( 1, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
  lay->addWidget( l );
  lay->addWidget( mSlider );
  lay->addItem( spacerItem );
  setLayout( lay );

  // timer for delayed transparency update - for more responsive GUI
  mTimer = new QTimer( this );
  mTimer->setSingleShot( true );
  mTimer->setInterval( 100 );
  connect( mTimer, &QTimer::timeout, this, &QgsLayerTreeOpacityWidget::updateOpacityFromSlider );

  connect( mSlider, &QAbstractSlider::valueChanged, this, &QgsLayerTreeOpacityWidget::sliderValueChanged );

  // init from layer
  switch ( mLayer->type() )
  {
    case QgsMapLayerType::VectorLayer:
    {
      QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( mLayer );
      mSlider->setValue( vl->opacity() * 1000.0 );
      connect( vl, &QgsVectorLayer::opacityChanged, this, &QgsLayerTreeOpacityWidget::layerTrChanged );
      break;
    }

    case QgsMapLayerType::RasterLayer:
    {
      mSlider->setValue( 1000 - qobject_cast<QgsRasterLayer *>( mLayer )->renderer()->opacity() * 1000 );
      // TODO: there is no signal for raster layers
      break;
    }

    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::MeshLayer:
      break;

  }
}

QSize QgsLayerTreeOpacityWidget::sizeHint() const
{
  return QWidget::sizeHint();
  //return QSize(200,200); // horizontal seems ignored, vertical is used for spacing
}

void QgsLayerTreeOpacityWidget::sliderValueChanged( int value )
{
  Q_UNUSED( value )

  if ( mTimer->isActive() )
    return;
  mTimer->start();
}

void QgsLayerTreeOpacityWidget::updateOpacityFromSlider()
{
  int value = mSlider->value();

  switch ( mLayer->type() )
  {
    case QgsMapLayerType::VectorLayer:
    {
      qobject_cast<QgsVectorLayer *>( mLayer )->setOpacity( value / 1000.0 );
      break;
    }
    case QgsMapLayerType::RasterLayer:
    {
      qobject_cast<QgsRasterLayer *>( mLayer )->renderer()->setOpacity( 1 - value / 1000.0 );
      break;
    }

    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::MeshLayer:
      break;
  }

  mLayer->triggerRepaint();
}

void QgsLayerTreeOpacityWidget::layerTrChanged()
{
  mSlider->blockSignals( true );
  mSlider->setValue( qobject_cast<QgsVectorLayer *>( mLayer )->opacity() * 1000.0 );
  mSlider->blockSignals( false );
}

//

QString QgsLayerTreeOpacityWidget::Provider::id() const
{
  return QStringLiteral( "transparency" );
}

QString QgsLayerTreeOpacityWidget::Provider::name() const
{
  return tr( "Opacity slider" );
}

QgsLayerTreeOpacityWidget *QgsLayerTreeOpacityWidget::Provider::createWidget( QgsMapLayer *layer, int widgetIndex )
{
  Q_UNUSED( widgetIndex )
  return new QgsLayerTreeOpacityWidget( layer );
}

bool QgsLayerTreeOpacityWidget::Provider::supportsLayer( QgsMapLayer *layer )
{
  switch ( layer->type() )
  {
    case QgsMapLayerType::VectorLayer:
    case QgsMapLayerType::RasterLayer:
      return true;

    case QgsMapLayerType::MeshLayer:
    case QgsMapLayerType::PluginLayer:
      return false;
  }
  return false;
}

///@endcond
