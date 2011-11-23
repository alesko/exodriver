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

#include <gtk/gtk.h>

//TapoMeter *tap_counter;  // Global variable 

/* OpenGL includes */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h> //#include <glut/glut.h> 
#endif

using namespace std;


LabjackClass *Glj;//(num_channels,num_samples);
std::vector<double> Gtime;
std::vector<double> G_data_vec;

int Gscreen_width_ = 640;
int Gscreen_height_ = 480;
int Gzoom = 1000;
float Gyamp = 10.0;

double GetRunTime(void)
{
   double sec = 0;
   struct timeval tp;

   gettimeofday(&tp,NULL);
   sec = tp.tv_sec + tp.tv_usec / 1000000.0;

   return(sec);
}

void ZoomIn()
{
  Gzoom = Gzoom + 100;
}

void ZoomOut()
{
  Gzoom = Gzoom - 100;
}

void ZoomOutYamp()
{
  if( Gyamp >= 1.0)
    Gyamp = Gyamp + 1.0;
  else
    Gyamp = Gyamp + 0.1;
  printf("Yamp: %f\n",Gyamp);
  
}

void ZoomInYamp()
{
  if( Gyamp > 1.0)
    Gyamp = Gyamp - 1.0;
  else
    {
      Gyamp = Gyamp - 0.1;
    }
  printf("Yamp: %f\n",Gyamp);
}



void HandleKeyboard(unsigned char key,int x, int y)
{
  switch (key) {
  case ':':                           // Decrease multiplier
  case '.':
    //tap_counter->decrease_k_();
    break;
  case '>':                           // Increase multiplier
  case '<':
    //tap_counter->increase_k_();
    break;
  case '-':                           // Decrease threshold 
  case '_':
    //tap_counter->decrease_c_();
    break;
  case '+':                           // Increase threshold 
  case '?':
    //tap_counter->increase_c_();
    break;    
  case 27:                            // Quit
  case 'Q':
  case 'q':
    {
      //tap_counter->close_LJ_device();
      usleep(1000);
      //tap_counter->set_is_running(false);      
      break;
    }
  }
}

void HandleMenu(int selection)
{
  
  switch(selection)
    {
    case(1):
      //ZoomIn();
      ZoomInYamp();
      //tap_counter->increase_c_();
      break;
    case(2):
      //ZoomOut();
      ZoomOutYamp();
      //tap_counter->decrease_c_();
      break;
    case(3):
      //tap_counter->increase_k_();
      break;
    case(4):
      //tap_counter->decrease_k_();
      break;
    case(10):
      exit(0);
      break;
    }
  
}

void CreateMenu(void)
{

  int main_menu;
  
  main_menu = glutCreateMenu(HandleMenu);
  glutAddMenuEntry("Zoom in",1);
  glutAddMenuEntry("Zoom out",2);
  glutAddMenuEntry("Increase factor",3);
  glutAddMenuEntry("Decrease factor",4);
  glutAddMenuEntry("Quit",10);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
  

}




void HandleIdle(void)
{
   static double tstart = -1;
   double tstop;
   usleep(10);
   /*   double fps = (double)tap_counter->get_fps();
   if (tstart < 0)
      tstart = tap_counter->GetRunTime();
   tstop = tap_counter->GetRunTime();
   if (tstop - tstart > 1.0/fps) {
      
      tstart = tstop;
   }*/
   glutPostRedisplay();
}

void HandleReshape(int w,int h)
{
  //Gscreen_width_ = 640;
  //Gscreen_height_ = 480;
  Gscreen_width_ = w;
  Gscreen_height_ = h;
}


int DrawPlot(std::vector<double> data,double color[3], double t1, double t2){//,color,maximum)
  //screenWidth=1280; screenHeight=960;
  //maximum = 
  int len = data.size(); //data_.size();
  int i = 0;
  int s = 0;
  int x=0;
  //int zoom=1000;

  glLineWidth(1);
  
  // x axis
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 0.0);
  glVertex2f(10,(GLint)((GLfloat)Gscreen_height_*0.5));
  glVertex2f(Gscreen_width_-10, (GLint)((GLfloat)Gscreen_height_*0.5));
  glEnd();

  // Y axis
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 0.0);
  glVertex2f(10, 10);
  glVertex2f(10, Gscreen_height_-10 );
  glEnd();

  GLfloat hy = (Gscreen_height_-10)*0.5;
  
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 0.0);
  if(Gyamp > 1.0)
    {
      for(i=(int)-Gyamp;i <= Gyamp; i++){
	//glVertex2f(i,Gscreen_height_-Gzoom*data[x]);
	/*glVertex2f(5, -i*Gyamp*hy+Gscreen_height_*0.5);
	glVertex2f(15, -i/5.0*Gyamp/5.0*hy+Gscreen_height_*0.5);
	glVertex2f(5, i/5.0*Gyamp/5.0*hy+Gscreen_height_*0.5);
	glVertex2f(15, i/5.0*Gyamp/5.0*hy+Gscreen_height_*0.5);*/
      }
    }
  glEnd();
  
  /* 
  glBegin(GL_LINES);
  glColor3f(1.0, 1.0, 1.0);
  glVertex2f(0, Gscreen_height_-Gzoom*t1);
  glVertex2f(Gscreen_width_, Gscreen_height_-Gzoom*t1);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0.0, 1.0, 0.0);
  glVertex2f(0,Gscreen_height_-Gzoom*t2);
  glVertex2f(Gscreen_width_,Gscreen_height_-Gzoom*t2);
  glEnd();
*/
  
  if( len > Gscreen_width_ )
      x = len - Gscreen_width_;
  glBegin(GL_LINE_STRIP);
  glColor3f(color[0], color[1], color[2]);
  
  for(i=0;i < len; i++){
    //glVertex2f(i,Gscreen_height_-Gzoom*data[x]);
    glVertex2f(i, -data[i]/Gyamp*hy+Gscreen_height_*0.5);
    x++;
  }
  glEnd();

  return 1;
}


// Called by GLUT when the screen needs to be redrawn 
void DoRedraw(std::vector<double> d,double t1, double t2)
{

  // Setup the display 
  glViewport(0,0,(GLsizei) Gscreen_width_,(GLsizei) Gscreen_height_); 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)Gscreen_width_, (GLdouble) Gscreen_height_, 0.0, 0.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(1.0,1.0,1.0,0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  double white[3]={1.0, 1.0, 1.0};
  double red [3]={1.0, 0.0, 0.0};
  double green[3]={0.0, 1.0, 0.0};
  double blue[3]={0.0, 0.0, 1.0};

  DrawPlot(d,red, t1,t2);
  glFlush();
  glutSwapBuffers();

}

void IsRunning(void)
{
  
  /*if(tap_counter->get_is_running())
    tap_counter->SampleData();
  else
    exit(0);
  */
  int buffersize = Gscreen_width_-20;
  std::vector< std::vector<double> > data;

  int k,l;
  Glj->StreamData();
  data = Glj->GetData();
  
  for( k=0; k < data.size(); k++) 
    {
      for( l=0; l < data[k].size(); l++)
	{
	  Gtime.push_back(GetRunTime());
	  G_data_vec.push_back(data[0][l]); // Fix number of channels
	  if( G_data_vec.size() > buffersize )
	    {
	      // Remove values if vector is too long
	      G_data_vec.erase(G_data_vec.begin());//	temp1_vec_.pop_front();
	      Gtime.erase(Gtime.begin());//
	    }
	  //DoRedraw(G_data_vec,double t1, double t2)
	  //cout << data[k][l] << "\t";
	}
      //cout << endl;
    }
  usleep(10);
  if( Gtime.size() != G_data_vec.size() )
    cout << "ERROR in datasize" << endl;
  for( k=0; k < data.size(); k++) 
    {
    }
  DoRedraw(G_data_vec,Gtime[0], Gtime[buffersize-1]);
}

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
  
  // Initialize the LabJack device
  Glj  = new LabjackClass(num_channels,num_samples); // Number of channels, SamplePacket size (25 for high speed)
  long startTime, endTime;

  int numDisplay;          //Number of times to display streaming information
  int numReadsPerDisplay;  //Number of packets to read before displaying streaming information

  numDisplay = 5;
  numReadsPerDisplay = 10;
   
  uint16 scanInterval = 4000;
  uint8 ResolutionIndex = 0x01;
  uint8 SettlingFactor = 0x00;
  uint8 ScanConfig = 0x00;
  int gain=1;

  if( Glj->StreamConfig(scanInterval, ResolutionIndex, SettlingFactor, ScanConfig, diff, gain ) != 0 )
    {
      cout << "Error in  StreamConfig_example." << endl;
      exit(0);
    }
  
 
  if( Glj->StreamStart() != 0 )
    {
      cout << "Error in StreamStart." << endl;
      exit(0);
    }


  Glj->InitStreamData();

  startTime = getTickCount();

  std::vector< std::vector<double> > data;  

  Glj->StreamData();
  data = Glj->GetData();
  cout << "Size (samples):" << data.size() << endl;
  cout << "Size channels:" << data[0].size() << endl;


  /* You must call glutInit before any other OpenGL/GLUT calls */
  glutInit(&argc,argv); 
  //glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // | GLUT_DEPTH);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // | GLUT_DEPTH);
  glutCreateWindow("LabJack Graph");
  glutInitWindowSize(640,480);
  glutReshapeWindow(Gscreen_width_,Gscreen_height_);
  glutInitWindowPosition(0,0);
  CreateMenu();
  glutReshapeFunc(HandleReshape);
  glutKeyboardFunc(HandleKeyboard);
  glutIdleFunc(HandleIdle);
  
  //glutDisplayFunc(display);
  glutDisplayFunc(IsRunning);
  //init();
  glutMainLoop();

  // Set up GLUT environment
	/* glutInit(&argc,argv);
  
  glutReshapeWindow(tap_counter->get_screen_width(),tap_counter->get_screen_height());
  glutInitWindowPosition(0,0);
  glutReshapeFunc(HandleReshape);
  glutKeyboardFunc(HandleKeyboard);
  //glutIdleFunc(HandleIdle);
  glutDisplayFunc(IsRunning);
  glutMainLoop();*/

  endTime = getTickCount();

  Glj->PrintLog(startTime,endTime);
  Glj->StreamStop();
  return(0);

}

