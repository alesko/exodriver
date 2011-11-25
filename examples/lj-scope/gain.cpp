#include "gain.h"

#include<qfont.h>
#include<stdio.h>


Gain::Gain(int id, RenderArea* ptr) : QComboBox() {

	setMinimumWidth ( fontMetrics().width("x100XX") );

	addItem(tr("   x1"),1);
	addItem(tr("  x10"),10);
	addItem(tr(" x100"),100);
	addItem(tr("x1000"),1000);
	gain = 1;
	id_ = id;
	ptr_ = ptr;
	
	connect(this,
		SIGNAL( activated(int) ),
		this,
		SLOT( setGain(int) ) );

}

void Gain::setGain ( int index ) {
  gain = itemData(index).toInt();
  ptr_->setGain(id_, gain);
}
