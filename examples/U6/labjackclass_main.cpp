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
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{

  int i,j;
  int num_samples=25;
  int num_channels=1;

  bool diff = false;

  for(int i = 1; i < argc; i++)
    {
      if( 0 == strcmp(argv[i], "-s"))
	{
	  if (argv[i+1] != NULL )
	    {
	      std::cout << "number of samples/package" << atof(argv[i+1]) << std::endl;
	      num_samples = atof(argv[i+1]);
	    }
	}
      if( 0 == strcmp(argv[i], "-c"))
	{
	  if (argv[i+1] != NULL )
	    {
	      std::cout << "Number of channels:" << atof(argv[i+1]) << std::endl;
	       num_channels = atof(argv[i+1]);
	    }
	}
     if( 0 == strcmp(argv[i], "-d"))
	{
	  diff = true;
	  std::cout << "Differential channels" << std::endl;
	}

    }

    
  
  LabjackClass lj(num_channels,num_samples); // Number of channels, SamplePacket size (25 for high speed)
  long startTime, endTime;

  int numDisplay;          //Number of times to display streaming information
  int numReadsPerDisplay;  //Number of packets to read before displaying streaming information

  numDisplay = 10;
  numReadsPerDisplay = 1;
   
  uint16 scanInterval = 4000;
  uint8 ResolutionIndex = 0x01;
  uint8 SettlingFactor = 0x00;
  uint8 ScanConfig = 0x00;

  if( lj.StreamConfig(scanInterval, ResolutionIndex, SettlingFactor, ScanConfig, diff, 1 ) != 0 )
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

  std::vector< std::vector<double> > data;
  
  lj.StreamData();
  data = lj.GetData();
  cout << "Size (samples):" << data.size() << endl;
  cout << "Size channels:" << data[0].size() << endl;
  
  int k,l;
  for( i = 0; i < numDisplay; i++ )
  {
    for( j = 0; j < numReadsPerDisplay; j++ )
      {
	lj.StreamData();
	data = lj.GetData();
	cout << "Size (samples):" << data.size() << endl;
	cout << "Size channels:" << data[0].size() << endl;
	for( k=0; k < data.size(); k++) 
	  {
	    for( l=0; l < data[k].size(); l++)
	      {
		cout << data[k][l] << "\t";
	      }
	    cout << endl;
	  }
      }
    //lj.PrintBuffer();
  }
  
  endTime = getTickCount();

  lj.PrintLog(startTime,endTime);
  lj.StreamStop();

}
