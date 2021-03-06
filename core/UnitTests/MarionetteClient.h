/*!
* @header Marionette
 *
 */

#import <Cocoa/Cocoa.h>
#import "MWorksCore/Client.h"

using namespace mw;

@interface MarionetteClient : NSObject {
    shared_ptr<Client> client;
	
	BOOL sent_run_event;
	BOOL sent_experiment;
	BOOL experiment_loaded;
	BOOL sent_close_experiment;
	BOOL sent_open_data_file;
	BOOL sent_close_data_file;
	BOOL data_file_open;
	BOOL state_system_running;
	BOOL experiment_ended;
	BOOL asserted;
	
	NSMutableArray *permitted_error_messages;
	NSMutableArray *expected_messages;
	NSMutableArray *expected_events;
	
}

@property (nonatomic, retain) NSMutableArray *permittedErrorMessages;
@property (nonatomic, retain) NSMutableArray *expectedMessages;
@property (nonatomic, retain) NSMutableArray *expectedEvents;
@property (nonatomic, assign) BOOL sentRunEvent, stateSystemRunning;
@property (nonatomic, assign) BOOL asserted, experimentEnded;
@property (nonatomic, assign) BOOL experimentLoaded, sentExperiment, sentCloseExperiment;
@property (nonatomic, assign) BOOL sentOpenDataFile, sentCloseDataFile, dataFileOpen;

@end
