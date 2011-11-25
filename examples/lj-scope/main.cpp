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


#include <QApplication>

#include "window.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
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

  
  QApplication app(argc, argv);
  Window window(num_channels);


  window.show();
  return app.exec();

}
