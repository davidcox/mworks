/*
 *  ScarabNetworkTest.h
 *  MWorksCore
 *
 *  Created by David Cox on 4/11/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "ScarabFileTest.h"
#include <pthread.h>


BEGIN_NAMESPACE_MW


#define TEST_URI	"ldobinary:tcp_buffered://127.0.0.1:20000"
//#define TEST_URI	"ldobinary:tcp://127.0.0.1:20000"

#define	serr(srv)	\
	errInt(	scarab_session_geterr((srv)), \
			scarab_session_getoserr((srv)) \
		)

#define	errInt(code, oserr)  \
	printErr(	(code), \
				scarab_moderror((code)), scarab_strerror((code)), \
				(oserr), \
				scarab_os_strerror((oserr)) \
			)

#define	printErr(errcode, mod, msg, oserr, osmsg) \
		do{ \
		fprintf(stderr,"ERROR: SCR-%5.5i: %s: %s\n", \
				(errcode), (mod), (msg)); \
		if ((oserr)) { \
		fprintf(stderr, "OSERR: %i: %s\n", \
				(oserr), (osmsg)); \
		} \
		}while(0)


#define N_BURST_WRITES	10000

void *test_server(void *args);
void *test_server_burst(void *args);
void *test_server_float_burst(void *args);

class ScarabNetworkTestFixture : public ScarabFileTestFixture {


	CPPUNIT_TEST_SUITE( ScarabNetworkTestFixture );
	
	CPPUNIT_TEST( testIntegerPutGet );
	CPPUNIT_TEST( testBigIntegerPutGet );
	CPPUNIT_TEST( testDictPutGet );
    //CPPUNIT_TEST( testListPutGet );
	CPPUNIT_TEST( testNestedDictPutGet );
	CPPUNIT_TEST( testFloatPutGet );
	CPPUNIT_TEST( testBurst );
	CPPUNIT_TEST( testFloatBurst );
	//CPPUNIT_TEST( testPutGetCodec );
									
	CPPUNIT_TEST_SUITE_END();
	
		
	
	public:
	
	
		
	
		void testBurst(){
			fprintf(stderr, "Running ScarabNetworkTestFixture::testBurst()\n");

			fprintf(stderr, "Testing network burst...\n");
			pthread_t thread;
			
			// Connect as server (separate thread)...
			int status = pthread_create(&thread, NULL, 
							&test_server_burst, NULL);
			
			usleep(1000000);
						
			// Connect for reading
			session = scarab_session_connect(TEST_URI);

			if (!session) {
				printErr(0,"SCARAB", "Session could not be allocated.", 0, "");
			} else {
				//fprintf(stderr, "[CLIENT CONNECTED]\n");
			}
			CPPUNIT_ASSERT( session != NULL );


			if (scarab_session_geterr(session)) {
				serr(session);
			}
			CPPUNIT_ASSERT( scarab_session_geterr(session) == 0 );
			
			ScarabDatum *outdatum;
			for(int i = 0; i < N_BURST_WRITES; i++){
				outdatum = scarab_read(session);
				
				CPPUNIT_ASSERT( outdatum != NULL );
				CPPUNIT_ASSERT( outdatum->type == SCARAB_INTEGER );
				//fprintf(stderr, "%d ", outdatum->data.integer);
				CPPUNIT_ASSERT( outdatum->data.integer == i );
			}
			
			if (scarab_session_geterr(session)) {
				serr(session);
			}
			CPPUNIT_ASSERT( scarab_session_geterr(session) == 0 );
			
			scarab_session_close(session);	
			//fprintf(stderr, "[CLIENT DISCONNECTED]\n");
			
		
		}


		void testFloatBurst(){		
			fprintf(stderr, "Running ScarabNetworkTestFixture::testFloatBurst()\n");

			fprintf(stderr, "Testing network burst (floats)...\n");
			pthread_t thread;
			
			// Connect as server (separate thread)...
			int status = pthread_create(&thread, NULL, 
							&test_server_float_burst, NULL);
			
			usleep(1000000);
						
			// Connect for reading
			session = scarab_session_connect(TEST_URI);

			if (!session) {
				printErr(0,"SCARAB", "Session could not be allocated.", 0, "");
			} else {
				//fprintf(stderr, "[CLIENT CONNECTED]\n");
			}
			CPPUNIT_ASSERT( session != NULL );


			if (scarab_session_geterr(session)) {
				serr(session);
			}
			CPPUNIT_ASSERT( scarab_session_geterr(session) == 0 );
			
			ScarabDatum *outdatum;
			int n = 0;
			fprintf(stderr, "stress testing...");
			for(double i = -N_BURST_WRITES; i < N_BURST_WRITES; i+=0.1){
				if(n++ % 1000 == 0){ fprintf(stderr, "."); }
				outdatum = scarab_read(session);
				
				CPPUNIT_ASSERT( outdatum != NULL );
				CPPUNIT_ASSERT( outdatum->type == SCARAB_FLOAT );
				CPPUNIT_ASSERT( (outdatum->data.floatp - i) < 1e-7 );
			}
			fprintf(stderr, "\n");
			
			if (scarab_session_geterr(session)) {
				serr(session);
			}
			CPPUNIT_ASSERT( scarab_session_geterr(session) == 0 );
			
			scarab_session_close(session);	
			//fprintf(stderr, "[CLIENT DISCONNECTED]\n");
			
		
		}

				
		void tearDown(){ }
	
		virtual ScarabDatum *putGet(ScarabDatum *indatum){
			
			
			pthread_t thread;
			
			// Connect as server (separate thread)...
			int status = pthread_create(&thread, NULL, 
							&test_server, (void *)indatum);
			
			
			
			usleep(1000000);
			//usleep(9000000);
			
			
												
			// Connect for reading
			session = scarab_session_connect(TEST_URI);

			if (!session) {
				printErr(0,"SCARAB", "Session could not be allocated.", 0, "");
			} else {
				//fprintf(stderr, "[CLIENT CONNECTED]\n");
			}
			CPPUNIT_ASSERT( session != NULL );


			if (scarab_session_geterr(session)) {
				serr(session);
			}
			CPPUNIT_ASSERT( scarab_session_geterr(session) == 0 );
			
			ScarabDatum *outdatum;
			outdatum = scarab_read(session);
			
			if (scarab_session_geterr(session)) {
				serr(session);
			}
			CPPUNIT_ASSERT( scarab_session_geterr(session) == 0 );
			
			scarab_session_close(session);	
			//fprintf(stderr, "[CLIENT DISCONNECTED]\n");
			
		
			return outdatum;
		}

};
		
void *test_server(void *args) {
		
	ScarabSession	*srv;
	ScarabSession	*client;
	ScarabDatum	 *the_datum = (ScarabDatum *)args;
	int i;


	//fprintf(stderr, "Server binding to %s...\n", TEST_URI);
	srv = scarab_session_listen(TEST_URI);
	if (!srv) {
		printErr(0,"SCARAB", "Session could not be allocated.", 0, "");
		return NULL;
	}
	if (scarab_session_geterr(srv)) {
		serr(srv);
		return NULL;
	}
	//for(;;)	{
	client = scarab_session_accept(srv);
	if (!srv) {
		printErr(0,"SCARAB",
						"Client session could not be allocated.", 0, "");
		
		return NULL;
	}
	if (scarab_session_geterr(client)) {
		serr(client);
		return NULL;
	}
	
	//fprintf(stderr, "[SERVER ACCEPT]\n");
	scarab_write(client, the_datum);
	
	scarab_session_close(client);
	//fprintf(stderr, "[SERVER DISCONNECT]\n");
	
	scarab_session_close(srv);
	//fprintf(stderr, "[SERVER STOP ACCEPTING]\n");

	return 0;
}


void *test_server_burst(void *args) {
		
	ScarabSession	*srv;
	ScarabSession	*client;
	int i;


	//fprintf(stderr, "Server binding to %s...\n", TEST_URI);
	srv = scarab_session_listen(TEST_URI);
	if (!srv) {
		printErr(0,"SCARAB", "Session could not be allocated.", 0, "");
		return NULL;
	}
	if (scarab_session_geterr(srv)) {
		serr(srv);
		return NULL;
	}
	
	
	client = scarab_session_accept(srv);
	if (!srv) {
		printErr(0,"SCARAB",
						"Client session could not be allocated.", 0, "");
		
		return NULL;
	}
	if (scarab_session_geterr(client)) {
		serr(client);
		return NULL;
	}
	
	//fprintf(stderr, "[SERVER ACCEPT]\n");
	
	for(int i = 0; i < N_BURST_WRITES; i++){
		scarab_write(client, scarab_new_integer(i));
	}
	
	
	scarab_session_close(client);
	//fprintf(stderr, "[SERVER DISCONNECT]\n");
	
	scarab_session_close(srv);
	//fprintf(stderr, "[SERVER STOP ACCEPTING]\n");

	return 0;
}


void *test_server_float_burst(void *args) {
		
	ScarabSession	*srv;
	ScarabSession	*client;
	int i;



	//fprintf(stderr, "Server binding to %s...\n", TEST_URI);
	srv = scarab_session_listen(TEST_URI);
	if (!srv) {
		printErr(0,"SCARAB", "Session could not be allocated.", 0, "");
		return NULL;
	}
	if (scarab_session_geterr(srv)) {
		serr(srv);
		return NULL;
	}
	
	
	client = scarab_session_accept(srv);
	if (!srv) {
		printErr(0,"SCARAB",
						"Client session could not be allocated.", 0, "");
		
		return NULL;
	}
	if (scarab_session_geterr(client)) {
		serr(client);
		return NULL;
	}
	
	//fprintf(stderr, "[SERVER ACCEPT]\n");
	
	for(double i = -N_BURST_WRITES; i < N_BURST_WRITES; i += 0.1){
		scarab_write(client, scarab_new_float(i));
	}
	
	
	scarab_session_close(client);
	//fprintf(stderr, "[SERVER DISCONNECT]\n");
	
	scarab_session_close(srv);
	//fprintf(stderr, "[SERVER STOP ACCEPTING]\n");

	return 0;
}


END_NAMESPACE_MW
