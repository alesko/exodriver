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

#include "labjackclass.h"
#include <time.h>

using namespace std;

int main(int argc, char* argv[])
{

  int i,j;
  LabjackClass lj(6,25);
  long startTime, endTime;

  int numDisplay;          //Number of times to display streaming information
  int numReadsPerDisplay;  //Number of packets to read before displaying streaming information

  numDisplay = 100;
  numReadsPerDisplay = 1;
   
  uint16 scanInterval = 4000;
  uint8 ResolutionIndex = 0x01;
  uint8 SettlingFactor = 0x00;
  uint8 ScanConfig = 0x00;

  if( lj.StreamConfig(scanInterval, ResolutionIndex, SettlingFactor, ScanConfig ) != 0 )
    {
      cout << "Error in  StreamConfig_example." << endl;
      exit(0);
    }
  
 
  if( lj.StreamStart() != 0 )
    {
      cout << "Error in StreamStart." << endl;
      exit(0);
    }


  lj.InitStreamData();

  startTime = getTickCount();

  for( i = 0; i < numDisplay; i++ )
  {
    for( j = 0; j < numReadsPerDisplay; j++ )
      {
	lj.StreamData();
      }
    lj.PrintBuffer();
  }

  endTime = getTickCount();

  lj.PrintLog(startTime,endTime);
  lj.StreamStop();

}
