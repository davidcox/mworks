/*
 *  EventBuffer.h
 *  MWorksCore
 *
 *  Created by David Cox on 8/12/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _EVENTBUFFER_H
#define _EVENTBUFFER_H


#include "Buffers.h"
#include "Event.h"
#include <boost/thread/mutex.hpp>


BEGIN_NAMESPACE_MW


class EventBuffer : public EventReceiver {
    protected:
		boost::mutex bufferLock;
        shared_ptr<Event> headEvent;
		
    public:
		EventBuffer();
        virtual ~EventBuffer(){}
    
        // from EventReceiver protocol
        virtual void putEvent(shared_ptr<Event> event);        
        
        shared_ptr<Event> getHeadEvent();
		void reset();
};

extern shared_ptr<EventBuffer> global_outgoing_event_buffer;
extern shared_ptr<EventBuffer> global_incoming_event_buffer;
    

class EventBufferReader {

    protected:
        shared_ptr<EventBuffer> buffer;
		shared_ptr<Event>currentEvent;
		boost::mutex readerLock;
		

    public:
    
        EventBufferReader(shared_ptr<EventBuffer> _buffer);            
		bool nextEventExists();
		shared_ptr<Event> getNextEvent();
		bool hasAtLeastNEvents(const unsigned int n);
		void reset() {
			boost::mutex::scoped_lock lock(readerLock);
			buffer->reset();
			currentEvent = buffer->getHeadEvent();
		}
};
 
    
void initEventBuffers();
    

//class BufferManager {
//
//    protected:
//        shared_ptr<EventBuffer> main_event_buffer;
//        // Network Buffers.
//        shared_ptr<EventBuffer> incomingBuffer;
//    public:
//        
//		BufferManager();
//        
//        /**
//         * Accessor methods to get different kinds of buffers.
//         */
//        shared_ptr<EventBufferReader> getNewMainBufferReader();
//        shared_ptr<EventBufferReader> getNewDisplayBufferReader();
//        shared_ptr<EventBufferReader> getNewDiskBufferReader();
//        shared_ptr<EventBufferReader> getNewIncomingNetworkBufferReader();
//        
//        /**
//         * Puts an event into the main event buffer.
//         */
//        void putEvent(shared_ptr<Event> newevent);
//		//void putEvent(Event *newevent, long timeUS);
//        
//        /**
//         * Puts an event into the incoming network buffer
//         */
//        void putIncomingNetworkEvent(shared_ptr<Event> newEvent);
//        
//	   /**
//	    *  reset
//		*/
//		void reset();
//};
//
//extern shared_ptr<BufferManager> global_outgoing_event_buffer;
//


END_NAMESPACE_MW


#endif
