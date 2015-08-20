/*
 *  LinkedListTest.cpp
 *  MWorksCore
 *
 *  Created by David Cox on 1/19/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "LinkedListTest.h"


BEGIN_NAMESPACE_MW


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( LinkedListTestFixture, "Unit Test" );

void LinkedListTestFixture::setUp(){
	a = shared_ptr<StimulusNode>(new StimulusNode());
	b = shared_ptr<StimulusNode>(new StimulusNode());
	c = shared_ptr<StimulusNode>(new StimulusNode());
	d = shared_ptr<StimulusNode>(new StimulusNode());
}

void LinkedListTestFixture::tearDown() {}



void LinkedListTestFixture::testAddToFront(){
	list.addToFront(a);
	list.addToFront(b);
	list.addToFront(c);
	
	node = list.getFrontmost();
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == NULL );
	
	node = list.getBackmost();
	CPPUNIT_ASSERT( node == a );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == b );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == c );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == NULL );
}

void LinkedListTestFixture::testAddToBack(){
	list.addToBack(a);
	list.addToBack(b);
	list.addToBack(c);
	
	node = list.getBackmost();
	CPPUNIT_ASSERT( node == c );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == b );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == a );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == NULL );
	
	node = list.getFrontmost();
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == NULL );
}	


void LinkedListTestFixture::testMoveForward(){
	list.addToFront(d);
	list.addToFront(c);
	list.addToFront(b);
	list.addToFront(a);
	
	c->moveForward();
	
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();			
	CPPUNIT_ASSERT( node == NULL );
	
	
	c->moveForward();
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();			
	CPPUNIT_ASSERT( node == NULL );
	
	c->moveForward();
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == c );
	
}



void LinkedListTestFixture::testMoveBackward(){
	list.addToFront(d);
	list.addToFront(c);
	list.addToFront(b);
	list.addToFront(a);
	
	b->moveBackward();
	
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();			
	CPPUNIT_ASSERT( node == NULL );
	
	
	b->moveBackward();
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();			
	CPPUNIT_ASSERT( node == NULL );
	
	b->moveBackward();
	node = list.getBackmost();
	
	CPPUNIT_ASSERT( node == b );
	
}

void LinkedListTestFixture::testBringToFront(){
	list.addToFront(d);
	list.addToFront(c);
	list.addToFront(b);
	list.addToFront(a);
	
	c->bringToFront();
	
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();			
	CPPUNIT_ASSERT( node == NULL );
	
	node = list.getBackmost();
	
	CPPUNIT_ASSERT( node == d );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == b );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == a );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == c );
	node = node->getPrevious();			
	CPPUNIT_ASSERT( node == NULL );
	
}


void LinkedListTestFixture::testShortList(){
	list.addToFront(b);
	list.addToFront(a);
	
	a->sendToBack();
	
	CPPUNIT_ASSERT( list.getFrontmost() == b );
	CPPUNIT_ASSERT( list.getBackmost() == a );
	
	a->bringToFront();
	
	CPPUNIT_ASSERT( list.getFrontmost() == a );
	CPPUNIT_ASSERT( list.getBackmost() == b );
	
}


void LinkedListTestFixture::testSendToBack(){
	list.addToFront(d);
	list.addToFront(c);
	list.addToFront(b);
	list.addToFront(a);
	
	b->sendToBack();
	
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();			
	CPPUNIT_ASSERT( node == NULL );
	
	node = list.getBackmost();
	
	CPPUNIT_ASSERT( node == b );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == d );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == c );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == a );
	node = node->getPrevious();			
	CPPUNIT_ASSERT( node == NULL );
	
}

void LinkedListTestFixture::testRemove(){
	list.addToFront(d);
	list.addToFront(c);
	list.addToFront(b);
	list.addToFront(a);
	
	b->remove();
	
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();
	CPPUNIT_ASSERT( node == NULL );
	
	node = list.getBackmost();
	
	CPPUNIT_ASSERT( node == d );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == c );
	node = node->getPrevious();
	CPPUNIT_ASSERT( node == a );
	node = node->getPrevious();			
	CPPUNIT_ASSERT( node == NULL );
}

void LinkedListTestFixture::testRemove2(){
    //
    // Same as testRemove, except elements are added with addToBack
    //
    
    list.addToBack(a);
    list.addToBack(b);
    list.addToBack(c);
    list.addToBack(d);
    
    b->remove();
    
    node = list.getFrontmost();
    
    CPPUNIT_ASSERT( node == a );
    node = node->getNext();
    CPPUNIT_ASSERT( node == c );
    node = node->getNext();
    CPPUNIT_ASSERT( node == d );
    node = node->getNext();
    CPPUNIT_ASSERT( node == NULL );
    
    node = list.getBackmost();
    
    CPPUNIT_ASSERT( node == d );
    node = node->getPrevious();
    CPPUNIT_ASSERT( node == c );
    node = node->getPrevious();
    CPPUNIT_ASSERT( node == a );
    node = node->getPrevious();
    CPPUNIT_ASSERT( node == NULL );
}

void LinkedListTestFixture::testEmpty(){
	node = list.getFrontmost();
	CPPUNIT_ASSERT( node == NULL );
	
	node = list.getBackmost();
	CPPUNIT_ASSERT( node == NULL );
}

void LinkedListTestFixture::testOneNode(){
	list.addToFront(a);
	
	node = list.getFrontmost();
	CPPUNIT_ASSERT( node == a );
	
	node = list.getBackmost();
	CPPUNIT_ASSERT( node == a );
	
	a->remove();
}

void LinkedListTestFixture::testGetNElements(){
	CPPUNIT_ASSERT( list.getNElements() == 0 );
	
	list.addToFront(a);
	CPPUNIT_ASSERT( list.getNElements() == 1 );
	
	list.addToFront(b);
	CPPUNIT_ASSERT( list.getNElements() == 2 );
	
	list.addToFront(c);
	CPPUNIT_ASSERT( list.getNElements() == 3 );
	
	list.addToFront(d);
	CPPUNIT_ASSERT( list.getNElements() == 4 );
	
	a->remove();
	CPPUNIT_ASSERT( list.getNElements() == 3 );
	
	b->remove();
	CPPUNIT_ASSERT( list.getNElements() == 2 );
	
	c->remove();
	CPPUNIT_ASSERT( list.getNElements() == 1 );
	
	d->remove();
	CPPUNIT_ASSERT( list.getNElements() == 0 );
}

void LinkedListTestFixture::testMultipleLists(){
	list.addToFront(a);
	a->remove();
	list2.addToFront(a);
	
	CPPUNIT_ASSERT( list.getNElements() == 0 );
	CPPUNIT_ASSERT( list2.getNElements() == 1 );
	
}


void LinkedListTestFixture::testRemoveAndReadd(){
	list.addToFront(a);
	list.addToFront(b);
	list.addToFront(c);
	list.addToFront(d);
	c->remove();
	
	list.addToFront(c);
	
	CPPUNIT_ASSERT( list.getNElements() == 4 );
	
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == NULL );
	
	c->remove();
	
	list.addToFront(c);
	
	CPPUNIT_ASSERT( list.getNElements() == 4 );
	
	node = list.getFrontmost();
	
	CPPUNIT_ASSERT( node == c );
	node = node->getNext();
	CPPUNIT_ASSERT( node == d );
	node = node->getNext();
	CPPUNIT_ASSERT( node == b );
	node = node->getNext();
	CPPUNIT_ASSERT( node == a );
	node = node->getNext();
	CPPUNIT_ASSERT( node == NULL );
	
}

void LinkedListTestFixture::testNotificationStyle() {
	LinkedList<VariableNotification> list3;

{
	//fprintf(stderr, "\nentering scoped section\n"); 
	shared_ptr<VariableNotification> vn = shared_ptr<VariableNotification>(new VariableNotification());
	ExpandableList<VariableNotification> *notifications = new ExpandableList<VariableNotification>();

	list3.addToFront(vn);
	notifications->addReference(vn);

	CPPUNIT_ASSERT(notifications->getNElements() == 1);
	CPPUNIT_ASSERT(list3.getNElements() == 1);
	
	delete notifications;
	vn->remove();
	//fprintf(stderr, "exiting scoped section\n"); 
}
	//fprintf(stderr, "everything is out of scope\n"); 

	CPPUNIT_ASSERT(list3.getNElements() == 0);

}


END_NAMESPACE_MW
