/**************************************************************************
 *
 *  Software License Agreement (GPL License)
 *
 *  Copyright (c) 2011, Alexander Skoglund, Karolinska Institute
 *  All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/

#ifndef WINDOW_H_
#define WINDOW_H_

#include <QWidget>
//#include <QString>

#define MAX_ROWS  13

class QPushButton;
class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;
class QSignalMapper;

class RenderArea;

#include "gain.h"

class Window : public QWidget
{
  Q_OBJECT
    
    public:
  Window(int numCh);
  
  private slots:
  //void gainChanged(int num, int g);
  void setChannel(int ch); 
  void setDiff(int ch); 
  //void sampleData(bool s);
  void selectFile();
  void closeFile();

 private:

  //Gain** gain_config_;
  int numChannels_;
  
  RenderArea *renderArea;
  
  
  QPushButton *sampleButton_;
  QPushButton* filePushButton_;
  QPushButton* closefilePushButton_;
  
  QLabel **gainLabel_;
  //QComboBox **gainComboBox_;
  Gain** gainComboBox_;
  QCheckBox **channelCheckBox_;
  QCheckBox **diffCheckBox_;
  QSignalMapper* channelMapper_;
  QSignalMapper* diffMapper_;
  //QSignalMapper* gainMapper_;
  
  QString fileName_;
  

 };

 #endif
