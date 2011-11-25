/**************************************************************************
 *
 * LICENSE
 *
 * All exodriver library and example source code are licensed under MIT X11.
 *
 *  Copyright (c) 2011, Alexander Skoglund <alexander.skoglund@ki.se>, 
 *  Karolinska Institute
 * 
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 * 
 *    The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *    THE SOFTWARE.
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

 protected:
  static LabjackClass  *instance_; // To make callback functions work

 public:

  enum LJ_gain
  {
    G1  = 1,
    G10 = 10
  };

  LabjackClass();
  ~LabjackClass();
  int config(int num_ch, int samples);
  int ConfigIO();
  int StreamConfig(uint16 scanInterval, uint8 ResolutionIndex, 
		   uint8 SettlingFactor, uint8 ScanConfig,
		   bool differetialReading, int gain);

  int StreamStart();
  int InitStreamData();
  int StreamData();
  int StreamStop();
  int PrintLog(long t0,long tf);
  int PrintBuffer();
  int PrintConfigU6();
  int SetDO(uint8 fio, uint8 eio, uint8 cio);
  std::vector< std::vector<double> > GetData();
   
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
