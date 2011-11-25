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

#include <QtGui>

#include "renderarea.h"
#include "window.h"

#include <iostream>

const int IdRole = Qt::UserRole;


Window::Window(int numCh)
{
  numChannels_ = numCh;
   
  renderArea = new RenderArea();
  renderArea->init(numChannels_);

  gainComboBox_ = new Gain*[numChannels_];
  gainLabel_ = new QLabel*[numChannels_];
   
  for(int i=0; i < numChannels_; i++)
    {
      gainComboBox_[i] = new Gain(i, &(*renderArea));
      gainLabel_[i] = new QLabel(tr("&Gain:"));
      gainLabel_[i]->setBuddy(gainComboBox_[i]);
    }
   
  channelCheckBox_ = new QCheckBox*[numChannels_];
  diffCheckBox_ = new QCheckBox*[numChannels_/2];
  channelMapper_ = new QSignalMapper(this);
  diffMapper_ = new QSignalMapper(this);
   
  char ch_name[10];
  for(int i=0; i < numChannels_; i++)
    {
      sprintf(ch_name,"Channel %d",i);
      channelCheckBox_[i] = new QCheckBox(tr(ch_name));
      channelMapper_->setMapping(channelCheckBox_[i], i);	 
      connect(channelCheckBox_[i], SIGNAL(toggled(bool)), channelMapper_, SLOT(map()));
       
    }

  for(int i=0; i < numChannels_/2; i++)
    {
      sprintf(ch_name,"Diff %d",i);
      diffCheckBox_[i] = new QCheckBox(tr(ch_name));
      diffMapper_->setMapping(diffCheckBox_[i], i);	 
      connect(diffCheckBox_[i], SIGNAL(toggled(bool)), diffMapper_, SLOT(map()));
    }
   
  connect(channelMapper_, SIGNAL(mapped(int)), this, SLOT(setChannel(int)));
  //connect(gainMapper_, SIGNAL(mapped(int,int)), this, SLOT(gainChanged(int,int)));

  connect(diffMapper_, SIGNAL(mapped(int)), this, SLOT(setDiff(int)));   

  QGridLayout *mainLayout = new QGridLayout;
   
  int col = 0;
  int row = 0;
  int max_col = col;
  for(int i=0; i < numChannels_; i++)
    {
      mainLayout->addWidget(channelCheckBox_[i], row, col+0);
      mainLayout->addWidget(gainLabel_[i], row, col+1, Qt::AlignRight);
      mainLayout->addWidget(gainComboBox_[i], row, col+2);
      //mainLayout->addWidget(gain_config_[i], row, col+2);
       
      if( i%2 == 1 )
	{
	  mainLayout->addWidget(diffCheckBox_[i/2], row, col+3);
	  col = 0;
	  row++;
	}
      else
	col = col + 3;
      if( col > max_col)
	max_col = col;
	 
    }
  
  col = max_col + 1;

  char sample_name[10];
  sampleButton_ = new QPushButton(tr("Sample"));

  sampleButton_->setCheckable(true);
  connect(sampleButton_, SIGNAL(toggled(bool)),
	  renderArea, SLOT(sample(bool)));

  mainLayout->addWidget(sampleButton_, MAX_ROWS+1, 0);
     
    

  mainLayout->addWidget(renderArea, 0, col+3, numChannels_, 2);
  

  filePushButton_ = new QPushButton( "&Filename" );
  connect(filePushButton_, SIGNAL( clicked() ),
	  this, SLOT( selectFile() ) );
  mainLayout->addWidget(filePushButton_);

  closefilePushButton_ = new QPushButton( "Close log file" );
  connect(closefilePushButton_, SIGNAL( clicked() ),
	  this, SLOT( closeFile() ) );
  mainLayout->addWidget(closefilePushButton_);
     
     
  setLayout(mainLayout);

 
  //channelCheckBox_[0]->setChecked(true);

  setWindowTitle(tr("LabJack Scope"));
}

/*void Window::gainChanged(int num, int g)
{
  std::cout << "Checked: " << num << std::endl;
  //   std::cout << "Checked: " << ch << std::endl;
  //RenderArea::Gains gain = RenderArea::Gains(gainComboBox_[num]->itemData(gainComboBox_[num]->currentIndex(), IdRole).toInt());
  //renderArea->setGain(gain);

}
*/

void Window::setChannel(int ch)
{

  if (channelCheckBox_[ch]->isChecked())
    {
      //      std::cout << "Checked: " << ch << "With gain:" << gainComboBox_[ch]->getGain() << std::endl;
      renderArea->setActive(ch, true);
    }
  else
    {
      //std::cout << "Unchecked: " << ch << std::endl;
      renderArea->setActive(ch, false);
    }
}

void Window::setDiff(int ch)
{
  if (diffCheckBox_[ch]->isChecked())
    {
      renderArea->setDiff(ch, true);
    }
  else
    {
      renderArea->setDiff(ch, false);
    }
}

/*
void Window::sampleData(bool s)
{
  if (s)
    {
      //std::cout << "Start sample " << std::endl;
      sampleButton_->setChecked(true);
    }
  else
    {
      //std::cout << "Stop sample " << std::endl;
      sampleButton_->setChecked(false);
    }
}
*/

void Window::selectFile()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("Logfiles (*.dat *.log *.txt)"));
  dialog.setViewMode(QFileDialog::Detail);

  if (dialog.exec())
    {
      QString fileName = dialog.selectedFiles()[0];
      renderArea->setLogFileName(QString(fileName));
    }

  
}

void Window::closeFile()
{
  renderArea->closeLogFile();//QString(fileName_));
}
