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

 #ifndef RENDERAREA_H
 #define RENDERAREA_H

// #include <QBrush>
#include <QPen>
// #include <QPixmap>
 #include <QWidget>
#include <QPaintEvent>
#include "labjackclass.h"

#define MAX_CHANNELS 15


class RenderArea : public QWidget
{
  Q_OBJECT
    
    public:
  
  //enum Gains { G1, G10, G100, G1000 };
  

  RenderArea(QWidget *parent = 0);
  
  void init(int numChannels);
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  LabjackClass lj_;
  
  int setLogFileName(QString filename );
  int closeLogFile();
  
  // LabJack config
  bool* activeChannelList_;
  bool* diffChannelList_;   // Half the size of numChannels (round up)
  int* gain_;
  int   resolutionIndex_;
  //void setChGain(int ch, int gain);
  void setGain(int ch, int gain);
  void setActive(int ch, bool active);
  void setDiff(int diff_ch, bool diff);
  void setResolution(int res);
  void printInfo();
  
 public slots:
     void sample(bool s);
     //void setGain(Gains gain);
     void run(void);

 protected:
     void paintEvent(QPaintEvent *event);

 private:
     
     int num_channels_;
     int num_samples_;

     std::vector< std::vector<double> > data_;
     int counter_;
     QPixmap pixmap;
     //int gain_;
     bool* channel_active_;
     bool  isSampling_;
     QTimer* timer_;
     int max_size_; // Data buffer
     
     QString* log_filename_; // the filename of the data-file
     FILE*    log_file_fd_;   // the file descriptor to the log data-file
     
     bool isLogging_;

 };

 #endif
