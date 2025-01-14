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


 


#include "labjackclass.h"

using namespace std;

LabjackClass::LabjackClass()
{

}

int LabjackClass::config(int num_ch, int samples)
{

  if( ( num_ch < 1 ) || (num_ch > 25) )
    {
      cout << "Wrong number of channels, must be 1-25." << endl;
      exit(0);
    }
  else
    NumChannels_  = num_ch;
  if( ( samples < 1 ) || (samples > 25) )
    {
      cout << "Wrong number of SamplesPerPack, must be 1-25 (1 low latency, 25 high speed)." << endl;
      exit(0);
    }
  else
    SamplesPerPacket_ = samples;

  packetCounter_ = 0;
  totalPackets_ = 0;

  //Opening first found U6 over USB
  if( (hDevice_ = openUSBConnection(-1)) == NULL )
    {
      cout << "Error open USB connection" << endl;
      exit(0);
    }

  cout << "USB connection opened." << endl;
  
  PrintConfigU6();

  //Getting calibration information from U6
  if( getCalibrationInfo(hDevice_, &caliInfo_) < 0 )
    {
      cout << "Error when reciving calibration info from U6." << endl;
      exit(0);
    }

  //Stopping any previous streams
  StreamStop();
  return 1;
            
}          

LabjackClass::~LabjackClass()
{
  
  cout << "Closing USB connection" << endl;
  closeUSBConnection(hDevice_);
            
}             
                        



//Sends a StreamConfig low-level command to configure the stream.
int LabjackClass::StreamConfig(uint16 scanInterval, uint8 ResolutionIndex, 
			       uint8 SettlingFactor, uint8 ScanConfig, 
			       bool differetialReading, int gain)
{
 
  int sendBuffSize;
  sendBuffSize = 14+NumChannels_*2;
  uint8 sendBuff[sendBuffSize], recBuff[8];
  int sendChars, recChars;
  uint16 checksumTotal;
  //uint16 scanInterval;
  int i;

  sendBuff[1] = (uint8)(0xF8);     //Command byte
  sendBuff[2] = 4 + NumChannels_;  //Number of data words = NumChannels_ + 4
  sendBuff[3] = (uint8)(0x11);     //Extended command number
  sendBuff[6] = NumChannels_;      //NumChannels_ (1-25)
  sendBuff[7] = ResolutionIndex;   //ResolutionIndex
  sendBuff[8] = SamplesPerPacket_; //SamplesPerPacket_
  sendBuff[9] = 0;                 //Reserved
  sendBuff[10] = SettlingFactor;   //SettlingFactor: 0
  sendBuff[11] = ScanConfig;       //ScanConfig:
                                   //  Bit 3: Internal stream clock frequency = b0: 4 MHz
                                   //  Bit 1: Divide Clock by 256 = b0

  //scanInterval = 4000;
  sendBuff[12] = (uint8)(scanInterval&(0x00FF));  //scan interval (low byte)
  sendBuff[13] = (uint8)(scanInterval/256);       //scan interval (high byte)

  if( differetialReading == true )
    {
      for( i = 0; i < NumChannels_; i++ )
	{
	  sendBuff[14 + i*2] = i*2;   //ChannelNumber (Positive) = i
	  sendBuff[15 + i*2] = 0x80;  //ChannelOptions: 
	                              // Bit 7: Differential = 0
	                              // Bit 5-4: GainIndex = 0 (+-10V)
	                              // E.g., Gain 10 in differential mode: 0x90
	}
    }
  else
    {
      for( i = 0; i < NumChannels_; i++ )
	{
	  sendBuff[14 + i*2] = i;     //ChannelNumber (Positive) = i
	  sendBuff[15 + i*2] = 0x00;  //ChannelOptions: 
	                              // Bit 7: Differential = 0
	                              // Bit 5-4: GainIndex = 0 (+-10V)
	                              // E.g., Gain 10 in differential mode: 0x90
	  
	}
    }
  extendedChecksum(sendBuff, sendBuffSize);

  //Sending command to U6
  sendChars = LJUSB_Write(hDevice_, sendBuff, sendBuffSize);
  if( sendChars < sendBuffSize )
    {
      if( sendChars == 0 )
	printf("Error : write failed (StreamConfig).\n");
      else
	printf("Error : did not write all of the buffer (StreamConfig).\n");
      return -1;
    }

  for( i = 0; i < 8; i++ )
    recBuff[i] = 0;

  //Reading response from U6
  recChars = LJUSB_Read(hDevice_, recBuff, 8);
  if( recChars < 8 )
    {
      if( recChars == 0 )
	printf("Error : read failed (StreamConfig).\n");
      else
	printf("Error : did not read all of the buffer, %d (StreamConfig).\n", recChars);

      for( i = 0; i < 8; i++)
	printf("%d ", recBuff[i]);

      return -1;
    }

  checksumTotal = extendedChecksum16(recBuff, 8);
  if( (uint8)((checksumTotal / 256) & 0xff) != recBuff[5])
    {
      printf("Error : read buffer has bad checksum16(MSB) (StreamConfig).\n");
      return -1;
    }

  if( (uint8)(checksumTotal & 0xff) != recBuff[4] )
    {
      printf("Error : read buffer has bad checksum16(LSB) (StreamConfig).\n");
      return -1;
    }

  if( extendedChecksum8(recBuff) != recBuff[0] )
    {
      printf("Error : read buffer has bad checksum8 (StreamConfig).\n");
      return -1;
    }

  if( recBuff[1] != (uint8)(0xF8) || recBuff[2] != (uint8)(0x01) || recBuff[3] != (uint8)(0x11) || recBuff[7] != (uint8)(0x00) )
    {
      printf("Error : read buffer has wrong command bytes (StreamConfig).\n");
      return -1;
    }

  if( recBuff[6] != 0 )
    {
      printf("Errorcode # %d from StreamConfig read.\n", (unsigned int)recBuff[6]);
      return -1;
    }

  return 0;
}

//Sends a StreamStart low-level command to start streaming.
int LabjackClass::StreamStart()
{
  uint8 sendBuff[2], recBuff[4];
  int sendChars, recChars;

  sendBuff[0] = (uint8)(0xA8);  //Checksum8
  sendBuff[1] = (uint8)(0xA8);  //Command byte

  //Sending command to U6
  sendChars = LJUSB_Write(hDevice_, sendBuff, 2);
  if( sendChars < 2 )
    {
      if( sendChars == 0 )
	printf("Error : write failed.\n");
      else
	printf("Error : did not write all of the buffer.\n");
      return -1;
    }

  //Reading response from U6
  recChars = LJUSB_Read(hDevice_, recBuff, 4);
  if( recChars < 4 )
    {
      if( recChars == 0 )
	printf("Error : read failed.\n");
      else
	printf("Error : did not read all of the buffer.\n");
      return -1;
    }

  if( normalChecksum8(recBuff, 4) != recBuff[0] )
    {
      printf("Error : read buffer has bad checksum8 (StreamStart).\n");
      return -1;
    }

  if( recBuff[1] != (uint8)(0xA9) || recBuff[3] != (uint8)(0x00) )
    {
      printf("Error : read buffer has wrong command bytes \n");
      return -1;
    }

  if( recBuff[2] != 0 )
    {
      printf("Errorcode # %d from StreamStart read.\n", (unsigned int)recBuff[2]);
      return -1;
    }

  return 0;
}

int LabjackClass::InitStreamData()
{
  
  recBuffSize_ = 14 + SamplesPerPacket_*2;
  //numDisplay = 1; //6;
  //numReadsPerDisplay = 1;//24;

  /* For USB StreamData, use Endpoint 3 for reads.  You can read the multiple
   * StreamData responses of 64 bytes only if SamplesPerPacket_ is 25 to help
   * improve streaming performance.  In this example this multiple is adjusted
   * by the readSizeMultiplier variable.
   */
  readSizeMultiplier_ = 1; //NumChannels_*2;


  responseSize_ = 14 + SamplesPerPacket_*2;
  scanNumber_ = 0;
  totalScanNumber_=0;

  
  //double voltages[(SamplesPerPacket_/NumChannels_)*readSizeMultiplier*numReadsPerDisplay*numDisplay][NumChannels_];
  int stream_data_response_size = (SamplesPerPacket_/NumChannels_)*readSizeMultiplier_; //*numReadsPerDisplay*numDisplay;
  bufferSize_ =  stream_data_response_size;
  //std::vector< std::vector<double> > 
  //voltages_(stream_data_response_size, std::vector<double> (NumChannels_));
  //voltages_.get_allocator().allocate(, std::vector<double> (NumChannels_));
  

  voltages_.resize(stream_data_response_size);
  cout << "stream_data_response_size:" << stream_data_response_size << endl;
  for (int i = 0; i < stream_data_response_size; ++i)
    voltages_[i].resize(NumChannels_);

}

//Reads the StreamData low-level function response in a loop.
//All voltages from the stream are stored in the voltages 2D array.
int LabjackClass::StreamData() //u6CalibrationInfo *caliInfo)
{

  int i, j, k, m;
  /*
    int recBuffSize;
    //  int packetCounter;

    //int totalPackets;  //The total number of StreamData responses read
    uint16 voltageBytes, checksumTotal;
    long startTime, endTime;
    int autoRecoveryOn;

    //int numDisplay;          //Number of times to display streaming information
    //int numReadsPerDisplay;  //Number of packets to read before displaying streaming information
    int readSizeMultiplier;  //Multiplier for the StreamData receive buffer size
    int responseSize;        //The number of bytes in a StreamData response (differs with SamplesPerPacket_)
  */
  int currChannel;// scanNumber;
  int recChars;//, backLog;
  uint16 voltageBytes, checksumTotal;
  int autoRecoveryOn;

  // Each StreamData response contains (SamplesPerPacket_ / NumChannels_) * readSizeMultiplier
  // samples for each channel.
  // Total number of scans = (SamplesPerPacket_ / NumChannels_) * readSizeMultiplier * numReadsPerDisplay * numDisplay
 

  uint8 recBuff[responseSize_*readSizeMultiplier_];

  //packetCounter = 0;
  currChannel = 0;
  //scanNumber = 0;
  //totalPackets = 0;
  recChars = 0;
  autoRecoveryOn = 0;
 
  //printf("Reading Samples...\n");

  //startTime_ = getTickCount();
  
  //  for( i = 0; i < numDisplay; i++ )
  //  {
  //      for( j = 0; j < numReadsPerDisplay; j++ )
  //      {


  //Reading stream response from U6
  recChars = LJUSB_Stream(hDevice_, recBuff, responseSize_*readSizeMultiplier_);
  if( recChars < responseSize_*readSizeMultiplier_ )
    {
      if(recChars == 0)
	printf("Error : read failed (StreamData).\n");
      else
	printf("Error : did not read all of the buffer, expected %d bytes but received %d(StreamData).\n", responseSize_*readSizeMultiplier_, recChars);

      return -1;
    }

  //Checking for errors and getting data out of each StreamData response
  for( m = 0; m < readSizeMultiplier_; m++ )
    {
      totalPackets_++;

      checksumTotal = extendedChecksum16(recBuff+ m*recBuffSize_, recBuffSize_);
      if( (uint8)((checksumTotal >> 8) & 0xff) != recBuff[m*recBuffSize_ + 5] )
	{
	  printf("Error : read buffer has bad checksum16(MSB) (StreamData).\n");
	  return -1;
	}

      if( (uint8)(checksumTotal & 0xff) != recBuff[m*recBuffSize_ + 4] )
	{
	  printf("Error : read buffer has bad checksum16(LSB) (StreamData).\n");
	  return -1;
	}

      checksumTotal = extendedChecksum8(recBuff + m*recBuffSize_);
      if( checksumTotal != recBuff[m*recBuffSize_] )
	{
	  printf("Error : read buffer has bad checksum8 (StreamData).\n");
	  return -1;
	}

      if( recBuff[m*recBuffSize_ + 1] != (uint8)(0xF9) || recBuff[m*recBuffSize_ + 2] != 4 + SamplesPerPacket_ || recBuff[m*recBuffSize_ + 3] != (uint8)(0xC0) )
	{
	  printf("Error : read buffer has wrong command bytes (StreamData).\n");
	  return -1;
	}

      if( recBuff[m*recBuffSize_ + 11] == 59 )
	{
	  if( !autoRecoveryOn )
	    {
	      printf("\nU6 data buffer overflow detected in packet %d.\nNow using auto-recovery and reading buffered samples.\n", totalPackets_);
	      autoRecoveryOn = 1;
	    }
	}
      else if( recBuff[m*recBuffSize_ + 11] == 60 )
	{
	  printf("Auto-recovery report in packet %d: %d scans were dropped.\nAuto-recovery is now off.\n", totalPackets_, recBuff[m*recBuffSize_ + 6] + recBuff[m*recBuffSize_ + 7]*256);
	  autoRecoveryOn = 0;
	}
      else if( recBuff[m*recBuffSize_ + 11] != 0 )
	{
	  printf("Errorcode # %d from StreamData read.\n", (unsigned int)recBuff[11]);
	  return -1;
	}

      if( packetCounter_ != (int)recBuff[m*recBuffSize_ + 10] )
	{
	  printf("PacketCounter (%d) does not match with with current packet count (%d)(StreamData).\n", recBuff[m*recBuffSize_ + 10], packetCounter_);
	  return -1;
	}

      backLog_ = (int)recBuff[m*48 + 12 + SamplesPerPacket_*2];

      // 
      for( k = 12; k < (12 + SamplesPerPacket_*2); k += 2 )
	{
	  voltageBytes = (uint16)recBuff[m*recBuffSize_ + k] + (uint16)recBuff[m*recBuffSize_ + k+1]*256;

	  getAinVoltCalibrated(&caliInfo_, 1, 0, 0, voltageBytes, &(voltages_[scanNumber_][currChannel]));

	  currChannel++;
	  if( currChannel >= NumChannels_ )
	    {
	      currChannel = 0;
	      scanNumber_++;
	      totalScanNumber_++;
	    }
	  if(scanNumber_ >= bufferSize_)
	    scanNumber_ = 0;
	}

      if(packetCounter_ >= 255)
	packetCounter_ = 0;
      else
	packetCounter_++;
    }
  //      }


  // }
  //endTime_ = getTickCount();

  return 0;

}

std::vector< std::vector<double> > LabjackClass::GetData()
{
  return voltages_;
}

int LabjackClass::PrintBuffer()
{
  int k;
  for( k = 0; k < NumChannels_; k++ )
    {
      //  printf("  AI%d: %.4f V\n", k, voltages_[scanNumber_ - 1][k]);
      printf("  AI%d: %.4f V\n", k, voltages_[0][k]);
    }
  return 0;

}

int LabjackClass::PrintLog(long t0,long tf)
{
  printf("\nNumber of scans: %ld\n", totalScanNumber_);
  printf("Total packets read: %d\n", totalPackets_);
  printf("Current PacketCounter: %d\n", ((packetCounter_ == 0) ? 255 : packetCounter_-1));
  printf("Current BackLog: %d\n", backLog_);

  //for( k = 0; k < NumChannels_; k++ )
  //  printf("  AI%d: %.4f V\n", k, voltages_[scanNumber_ - 1][k]);
  // }

  
  printf("\nRate of samples: %.0lf samples per second\n", (totalScanNumber_*NumChannels_)/((tf - t0)/1000.0));
  printf("Rate of scans: %.0lf scans per second\n\n", totalScanNumber_/((tf - t0)/1000.0));

}

//Sends a StreamStop low-level command to stop streaming.
int LabjackClass::StreamStop(void)
{
  uint8 sendBuff[2], recBuff[4];
  int sendChars, recChars;

  sendBuff[0] = (uint8)(0xB0);  //Checksum8
  sendBuff[1] = (uint8)(0xB0);  //Command byte

  //Sending command to U6
  sendChars = LJUSB_Write(hDevice_, sendBuff, 2);
  if( sendChars < 2 )
    {
      if( sendChars == 0 )
	printf("Error : write failed (StreamStop).\n");
      else
	printf("Error : did not write all of the buffer (StreamStop).\n");
      return -1;
    }

  //Reading response from U6
  recChars = LJUSB_Read(hDevice_, recBuff, 4);
  if( recChars < 4 )
    {
      if( recChars == 0 )
	printf("Error : read failed (StreamStop).\n");
      else
	printf("Error : did not read all of the buffer (StreamStop).\n");
      return -1;
    }

  if( normalChecksum8(recBuff, 4) != recBuff[0] )
    {
      printf("Error : read buffer has bad checksum8 (StreamStop).\n");
      return -1;
    }

  if( recBuff[1] != (uint8)(0xB1) || recBuff[3] != (uint8)(0x00) )
    {
      printf("Error : read buffer has wrong command bytes (StreamStop).\n");
      return -1;
    }

  if( recBuff[2] != 0 )
    {
      unsigned int errorcode = (unsigned int)recBuff[2];
     
      if (errorcode == 52)
	{
	  cout << "Stream not running." << endl;
	}
      else
	{
	  printf("Errorcode # %d from StreamStop read.\n", (unsigned int)recBuff[2]);
	  return -1;
	}
      
    }

  /*
  //Reading left over data in stream endpoint.  Only needs to be done with firmwares
  //less than 0.94.
  uint8 recBuffS[64];
  int recCharsS = 64;
  printf("Reading left over data from stream endpoint.\n");
  while( recCharsS > 0 )
  recCharsS = LJUSB_Stream(hDevice_, recBuffS, 64);
  */

  return 0;
}

//Sends a Feedback low-level command that configures digital directions,

int LabjackClass::SetDO(uint8 fio, uint8 eio, uint8 cio) 
{
  uint8 sendBuff[14], recBuff[10]; //
  int sendChars, recChars;
  int len= 14;
  int r_len= 10;
  uint16 binVoltage16, checksumTotal;
  uint8 state;

  sendBuff[1] = (uint8)(0xF8);  //Command byte
  //sendBuff[2] = 11;             //Number of data words (.5 word for echo, 10.5
  //words for IOTypes and data)
  sendBuff[2] = 0x04;             //Number of data words 

  sendBuff[3] = (uint8)(0x00);  //Extended command number
  sendBuff[6] = 0;     //Echo


  // Set digital out
  sendBuff[7]  = 0x1B; //27;  // Changed to 11 = BitStateWrite 
  sendBuff[8]  = 0xFF; // WriteMask determine if the corresponding bit shouldf be updated
  sendBuff[9]  = 0xFF;
  sendBuff[10] = 0xFF;
  sendBuff[11] = fio;
  sendBuff[12] = eio;
  sendBuff[13] = cio;

  /*    // Read the analog input
	sendBuff[14] = 0x02;
	sendBuff[15] = 0x00; // Positive Channel
	sendBuff[16] = 0x00; // Bit 0-3: Resolution Index
	// Bit 4-7: GainIndex
	sendBuff[17] = 0x00; // Bit 0-2: Settling factor
	// Bit 7:   Differetial
	*/
  extendedChecksum(sendBuff, len);

  //Sending command to U6
  if( (sendChars = LJUSB_BulkWrite(hDevice_, U6_PIPE_EP1_OUT, sendBuff, len)) < len)
    {
      if(sendChars == 0)
	printf("Feedback setup error : write failed");
      else
	printf("Feedback setup error : did not write all of the buffer");
      return -1;
    }

  //Reading response from U6
  if( (recChars = LJUSB_BulkRead(hDevice_, U6_PIPE_EP2_IN, recBuff, r_len)) < r_len)
    {
      if(recChars == 0)
	{
	  printf("Feedback setup error : read failed");
	  return -1;
	}
      else
	{
	  //printf("Feedback setup error : did not read all of the buffer");
	}
    }

  checksumTotal = extendedChecksum16(recBuff, r_len);
  if( (uint8)((checksumTotal / 256 ) & 0xff) != recBuff[5])
    {
      printf("Feedback setup error : read buffer has bad checksum16(MSB)");
      return -1;
    }

  if( (uint8)(checksumTotal & 0xff) != recBuff[4])
    {
      printf("Feedback setup error : read buffer has bad checksum16(LBS)");
      return -1;
    }

  if( extendedChecksum8(recBuff) != recBuff[0])
    {
      printf("Feedback setup error : read buffer has bad checksum8");
      return -1;
    }

  if( recBuff[1] != (uint8)(0xF8) || recBuff[2] != 2 || recBuff[3] != (uint8)(0x00) )
    {
      printf("Feedback setup error : read buffer has wrong command bytes ");
      return -1;
    }

  if( recBuff[6] != 0)
    {
      printf("Feedback setup error : received errorcode %d for frame %d in Feedback response. ", recBuff[6], recBuff[7]);
      return -1;
    }

  return 0;
}


//Sends a ConfigIO low-level command to turn off timers/counters
int LabjackClass::ConfigIO()
{
  uint8 sendBuff[16], recBuff[16];
  uint16 checksumTotal;
  int sendChars, recChars, i;

  sendBuff[1] = (uint8)(0xF8);  //Command byte
  sendBuff[2] = (uint8)(0x03);  //Number of data words
  sendBuff[3] = (uint8)(0x0B);  //Extended command number

  sendBuff[6] = 1;  //Writemask : Setting writemask for TimerCounterConfig (bit 0)

  sendBuff[7] = 0;  //NumberTimersEnabled : Setting to zero to disable all timers.
  sendBuff[8] = 0;  //CounterEnable: Setting bit 0 and bit 1 to zero to disable both counters
  sendBuff[9] = 0;  //TimerCounterPinOffset

  for( i = 10; i < 16; i++ )
    sendBuff[i] = 0;   //Reserved
  extendedChecksum(sendBuff, 16);

  //Sending command to U6
  if( (sendChars = LJUSB_Write(hDevice_, sendBuff, 16)) < 16 )
    {
      if( sendChars == 0 )
	printf("ConfigIO error : write failed\n");
      else
	printf("ConfigIO error : did not write all of the buffer\n");
      return -1;
    }

  //Reading response from U6
  if( (recChars = LJUSB_Read(hDevice_, recBuff, 16)) < 16 )
    {
      if( recChars == 0 )
	printf("ConfigIO error : read failed\n");
      else
	printf("ConfigIO error : did not read all of the buffer\n");
      return -1;
    }

  checksumTotal = extendedChecksum16(recBuff, 15);
  if( (uint8)((checksumTotal / 256 ) & 0xff) != recBuff[5] )
    {
      printf("ConfigIO error : read buffer has bad checksum16(MSB)\n");
      return -1;
    }

  if( (uint8)(checksumTotal & 0xff) != recBuff[4] )
    {
      printf("ConfigIO error : read buffer has bad checksum16(LSB)\n");
      return -1;
    }

  if( extendedChecksum8(recBuff) != recBuff[0] )
    {
      printf("ConfigIO error : read buffer has bad checksum8\n");
      return -1;
    }

  if( recBuff[1] != (uint8)(0xF8) || recBuff[2] != (uint8)(0x05) || recBuff[3] != (uint8)(0x0B) )
    {
      printf("ConfigIO error : read buffer has wrong command bytes\n");
      return -1;
    }

  if( recBuff[6] != 0 )
    {
      printf("ConfigIO error : read buffer received errorcode %d\n", recBuff[6]);
      return -1;
    }

  if( recBuff[8] != 0 )
    {
      printf("ConfigIO error : NumberTimersEnabled was not set to 0\n");
      return -1;
    }

  if( recBuff[9] != 0 )
    {
      printf("ConfigIO error : CounterEnable was not set to 0\n");
      return -1;
    }

  return 0;
}


//Sends a ConfigU3 low-level command to read back configuration settings
int LabjackClass::PrintConfigU6()
{
  uint8 sendBuff[26];
  uint8 recBuff[38];
  uint16 checksumTotal;
  int sendChars, recChars, i;

  sendBuff[1] = (uint8)(0xF8);  //Command byte
  sendBuff[2] = (uint8)(0x0A);  //Number of data words
  sendBuff[3] = (uint8)(0x08);  //Extended command number

  //Setting all bytes to zero since we only want to read back the U6
  //configuration settings
  for( i = 6; i < 26; i++ )
    sendBuff[i] = 0;

  /* The commented out code below sets the U6's local ID to 3.  After setting
     the local ID, reset the device for this change to take effect. */

  //sendBuff[6] = 8;  //WriteMask : setting bit 3
  //sendBuff[8] = 3;  //LocalID : setting local ID to 3

  extendedChecksum(sendBuff, 26);

  //Sending command to U6
  if( (sendChars = LJUSB_Write(hDevice_, sendBuff, 26)) < 26 )
    {
      if( sendChars == 0 )
	printf("ConfigU6 error : write failed\n");
      else
	printf("ConfigU6 error : did not write all of the buffer\n");
      return -1;
    }

  //Reading response from U6
  if( (recChars = LJUSB_Read(hDevice_, recBuff, 38)) < 38 )
    {
      if( recChars == 0 )
	printf("ConfigU6 error : read failed\n");
      else
	printf("ConfigU6 error : did not read all of the buffer\n");
      return -1;
    }

  checksumTotal = extendedChecksum16(recBuff, 38);
  if( (uint8)((checksumTotal / 256) & 0xff) != recBuff[5] )
    {
      printf("ConfigU6 error : read buffer has bad checksum16(MSB)\n");
      return -1;
    }

  if( (uint8)(checksumTotal & 0xff) != recBuff[4] )
    {
      printf("ConfigU6 error : read buffer has bad checksum16(LBS)\n");
      return -1;
    }

  if( extendedChecksum8(recBuff) != recBuff[0] )
    {
      printf("ConfigU6 error : read buffer has bad checksum8\n");
      return -1;
    }

  if( recBuff[1] != (uint8)(0xF8) || recBuff[2] != (uint8)(0x10) || recBuff[3] != (uint8)(0x08) )
    {
      printf("ConfigU6 error : read buffer has wrong command bytes\n");
      return -1;
    }

  if( recBuff[6] != 0 )
    {
      printf("ConfigU6 error : read buffer received errorcode %d\n", recBuff[6]);
      return -1;
    }

  printf("U6 Configuration Settings:\n");
  //  printf("FirmwareVersion: %.3f\n", recBuff[10] + recBuff[9]/100.0);
  printf("  FirmwareVersion = %d.%02d\n", recBuff[10], recBuff[9]);
  //  printf("BootloaderVersion: %.3f\n", recBuff[12] + recBuff[11]/100.0);
  printf("  BootloaderVersion = %d.%02d\n", recBuff[12], recBuff[11]);
  //  printf("HardwareVersion: %.3f\n", recBuff[14] + recBuff[13]/100.0);
  printf("  HardwareVersion = %d.%02d\n", recBuff[14], recBuff[13]);
  printf("  SerialNumber: %u\n", recBuff[15] + recBuff[16]*256 + recBuff[17]*65536 + recBuff[18]*16777216);
  printf("  ProductID: %d\n", recBuff[19] + recBuff[20]*256);
  printf("  LocalID: %d\n", recBuff[21]);
  printf("  Version Info: %d\n", recBuff[37]);
  
  if( recBuff[37] == 4 ){
    printf("  DeviceName = U6\n");
    pro_ = false;
  }
  else if(recBuff[37] == 12) {
    printf("  DeviceName = U6-Pro\n");
    pro_ = true;
  }

  return 0;
}

