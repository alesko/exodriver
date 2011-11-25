#ifndef _GAIN_H_
#define _GAIN_H_

#include <QComboBox>
#include "renderarea.h"

class Gain : public QComboBox {

    Q_OBJECT
      
      public:
  
  
  Gain(int id, RenderArea* ptr );
  private slots:
  void setGain(int i);
  
 private:
  int gain;
  int id_;
  RenderArea* ptr_;
  
 public:
  inline float getGain() {return gain;};
  
};

#endif
