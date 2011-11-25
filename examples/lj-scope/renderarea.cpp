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


//#include <time>
#include <unistd.h>
#include <QtGui>

#include "renderarea.h"

#include <iostream>
#include <vector>

 RenderArea::RenderArea(QWidget *parent)
   : QWidget(parent)  //lj_(25,25)
 {

   setBackgroundRole(QPalette::Base);
   setAutoFillBackground(true);

   int num_channels = 4;
   int num_samples = 24;

   lj_.config(num_channels,num_samples); // Number of channels, SamplePacket size (25 for high speed)

   int numDisplay;          //Number of times to display streaming information
   int numReadsPerDisplay;  //Number of packets to read before displaying streaming information

   //   numDisplay = 10;
   //numReadsPerDisplay = 25;
   
   uint16 scanInterval = 4000;
   uint8 ResolutionIndex = 0x01;
   uint8 SettlingFactor = 0x00;
   uint8 ScanConfig = 0x00;
   bool diff = false;

   if( lj_.StreamConfig(scanInterval, ResolutionIndex, SettlingFactor, ScanConfig, diff, 1 ) != 0 )
     {
       std::cout << "Error in  StreamConfig_example." << std::endl;
       exit(0);
     }

 }



 QSize RenderArea::minimumSizeHint() const
 {
     return QSize(100, 100);
 }

 QSize RenderArea::sizeHint() const
 {
     return QSize(600, 400);
 }

void RenderArea::init(int numChannels)
{
  num_channels_ = numChannels;
  num_samples_ = ((int) ( MAX_CHANNELS/num_channels_ ) ) * num_channels_;
  //std::cout << "Num ch: " << num_channels_ << " num_samples:" << num_samples_ << std::endl;

  activeChannelList_ = new bool[numChannels];
  gain_ = new int[numChannels];

  diffChannelList_ = new bool[numChannels/2];   // Half the size of numChannels (round up)

  for(int i=0; i< num_channels_; i++)
    {
      activeChannelList_[i] = false;
      gain_[i] = 1;
    }
  for(int i=0; i< num_channels_/2; i++)
    diffChannelList_[i] = false;
}


void RenderArea::setGain(int ch, int gain)
{
  gain_[ch] = gain; 
  printInfo();
}



void RenderArea::setActive(int ch, bool active)
{
  activeChannelList_[ch] = active;
  printInfo();
  
}

void RenderArea::setDiff(int diff_ch, bool diff)
{
  diffChannelList_[diff_ch] = diff;
  printInfo();
}

void RenderArea::setResolution(int res)
{
}

void RenderArea::printInfo()
{
  
  for(int i=0; i< num_channels_; i++)
    std::cout << "ch[" << i << "]: act:" << activeChannelList_[i] << " gain " << gain_[i] << std::endl;
  for(int i=0; i< num_channels_/2; i++)
    std::cout << "Diff [" << i << "]:" << diffChannelList_[i] << std::endl;
    
}

int RenderArea::setLogFileName(QString filename )
{

  (log_filename_) = &filename;
  log_file_fd_ = fopen(log_filename_->toAscii(),"wt");
  isLogging_ = true;
  if(log_file_fd_ > 0)
      return 1;
  else 
    return 0;
  
}

int RenderArea::closeLogFile()
{

  isLogging_ = false;
  fclose(log_file_fd_);

}


void RenderArea::sample(bool s)
{
  this->isSampling_ = s;
  if (s == true)
    {
      counter_= 1;
      std::cout << "Start sample" << std::endl;
      if( lj_.StreamStart() != 0 )
	{
	  std::cout << "Error in StreamStart." << std::endl;
	  exit(0);
	}
      
      lj_.InitStreamData();

      timer_ = new QTimer(this);
      connect(timer_, SIGNAL(timeout()), this, SLOT(run()));
      timer_->start(10);
    }
  else
    {
      lj_.StreamStop();
      std::cout << "Stop sample" << std::endl;
      timer_->stop();
    }

}

void RenderArea::run(void)
{

  std::vector< std::vector<double> > newdata;
  //  std::vector< std::vector<double> >::iterator Iter;
  lj_.StreamData();
  newdata = lj_.GetData();

  int num_Data = data_.size();
  int num_ch = newdata[0].size();

  // Log data
  if(true == isLogging_ )
    {
      num_Data = newdata.size();
      for( int j=0; j< num_Data; j++)
	{
	  for(int i=0; i < num_ch; i++)
	    fprintf(log_file_fd_,"%f\t",newdata[j][i]);
	  fprintf(log_file_fd_,"\n");
	}
      
    }
 
  // Check if we have prevoiud data
  num_Data = data_.size();
  if( num_Data < 0 )
    {
      data_ = newdata;
      update();
      return;
    }
  else
    {
      num_Data = newdata.size();
      //std::cout << "Data size (samples) before:" << data_.size() << std::endl;
      for( int j=0; j< num_Data; j++)
	{
	  data_.push_back( std::vector<double>() );
	  for(int i=0; i < num_ch; i++)
	    {
	      data_[data_.size()-1].push_back( newdata[j][i] );
	    }
	  if( data_.size() > max_size_)
	    {
	      data_[0].clear();
	      data_.erase( data_.begin( ) );
	    }
	}

      
    }
  std::cout << "Buffer size:" << data_.size() << std::endl;
  update();

}



 void RenderArea::paintEvent(QPaintEvent * /* event */)
 {

   int w = width();
   int h = height();

   max_size_ = width();

   QPen penData[14]={QPen(Qt::blue,1),	
		    QPen(Qt::green,1),	
		    QPen(Qt::red,1),	
		    QPen(Qt::cyan,1),	
		    QPen(Qt::magenta,1),
		    QPen(Qt::yellow,1),
                    QPen(Qt::darkBlue,1),	
		    QPen(Qt::darkGreen,1),	
		    QPen(Qt::darkRed,1),	
		    QPen(Qt::darkCyan,1),	
		    QPen(Qt::darkMagenta,1),
		    QPen(Qt::darkYellow,1),
		    QPen(Qt::gray,1),
		    QPen(Qt::darkGray,1)};





  
   QPainter painter(this);
   

   int i=0;
   
   int yOffset = 0;
   int yZero = h/2;
   double yRange = 20;

   painter.setPen(QPen(Qt::black,1, Qt::DotLine));
   painter.drawLine(5,yZero, w-5, yZero );
   painter.drawLine(5,5, 5, h-5 );
  

   int num_Data = data_.size();   
   int penColor = 0;
 
   counter_ = 0;    

   if( 0 < num_Data)
     {
       int num_Ch = data_[i].size();
       for(i=1; i < num_Data; i++)
	 {
	   int penColor = 0;
	   
	   for(int j=0; j < num_Ch; j++)
	     {
	       int p0 = (-data_[i-1][j]/yRange)*h+yZero;
	       int p1 = (-data_[i][j]/yRange)*h+yZero;
	       painter.setPen(penData[penColor%num_channels_]);
	       painter.drawLine(counter_-1,p0, counter_,p1 );
	       penColor++;
	     }
	   counter_++;
	 }
   
     }
   
 }
