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

#ifndef LABJACK_CLASS_H_
#define LABJACK_CLASS_H_

#include "u6.h"
#include <iostream>
#include <vector>
//#include <string>
//#include <time.h>
#include <stdio.h>


class LabjackClass{

 public:
  LabjackClass(int num_ch, int samples);
  ~LabjackClass();
  int ConfigIO();
  int StreamConfig(uint16 scanInterval, uint8 ResolutionIndex, 
		   uint8 SettlingFactor, uint8 ScanConfig);
  int StreamStart();
  int InitStreamData();
  int StreamData();
  int StreamStop();
  int PrintLog(long t0,long tf);
  int PrintBuffer();
  int PrintConfigU6();
  int SetDO(uint8 fio, uint8 eio, uint8 cio);
   
 private:
  uint8 NumChannels_;        //For this example to work proper, SamplesPerPacket needs
                                   //to be a multiple of NumChannels.
  uint8 SamplesPerPacket_;   //Needs to be 25 to read multiple StreamData responses
                                   //in one large packet, otherwise can be any value between
                                   //1-25 for 1 StreamData response per packet.

  int packetCounter_;
  int totalPackets_;               //The total number of StreamData responses read
  

  HANDLE hDevice_;
  u6CalibrationInfo caliInfo_;

  bool pro_; // Is it a U6 pro or not?

/*
  int recChars_, backLog_;
  //  int packetCounter;
  int currChannel_, scanNumber_;
  //int totalPackets;  //The total number of StreamData responses read
  uint16 voltageBytes_, checksumTotal_; 
  int autoRecoveryOn_;
*/
  long totalScanNumber_;
  int backLog_;
  int scanNumber_;
  int recBuffSize_;  
  long startTime_, endTime_;
  int bufferSize_;
  //int numDisplay;          //Number of times to display streaming information
  //int numReadsPerDisplay;  //Number of packets to read before displaying streaming information
  int readSizeMultiplier_;  //Multiplier for the StreamData receive buffer size
  int responseSize_;        //The number of bytes in a StreamData response (differs with SamplesPerPacket_)
  
  std::vector< std::vector<double> > voltages_;

};

#endif
