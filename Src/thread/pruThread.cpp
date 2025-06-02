#include <cstdio>

#include "pruThread.h"
#include "../modules/module.h"

#include "stm32f4xx_hal.h"

using namespace std;

// Thread constructor
pruThread::pruThread(uint8_t slice, uint32_t frequency) :
	slice(slice),
	frequency(frequency)
{
	printf("Creating thread %d\n", this->frequency);
	
	this->semaphore = false;
	this->execute = false;
}

void pruThread::startThread(void)
{
	TimerPtr = new pruTimer(this->slice, this->frequency, this);
}

void pruThread::stopThread(void)
{
    this->TimerPtr->stopTimer();
}


void pruThread::registerModule(Module* module)
{
	this->vThread.push_back(module);
}


void pruThread::registerModulePost(Module* module)
{
	this->vThreadPost.push_back(module);
	this->hasThreadPost = true;
}


void pruThread::run(void)
{

	if(!this->execute)
		return;	
	
	while (this->semaphore == true);	
	this->semaphore = true;	

	// iterate over the Thread pointer vector to run all instances of Module::runModule()
	for (iter = vThread.begin(); iter != vThread.end(); ++iter) (*iter)->runModule();

	//need a check here on the base thread to ensure the pulse width is correct.
	if(this->frequency > 10000 ){
		for (int j = 67; j > 0; j--) {
			__ASM volatile ("nop");
		}
	}

	// iterate over the second vector that contains module pointers to run after (post) the main vector
	if (hasThreadPost)
	{
		for (iter = vThreadPost.begin(); iter != vThreadPost.end(); ++iter) (*iter)->runModulePost();
	}

	this->execute = false;
	this->semaphore = false;
}
