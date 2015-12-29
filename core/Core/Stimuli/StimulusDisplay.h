/*
 *  StimulusDisplay.h
 *  MWorksCore
 *
 *  Created by David Cox on 6/10/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef STIMULUS_DISPLAY_H_
#define STIMULUS_DISPLAY_H_

#include <map>
#include <vector>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <CoreVideo/CVDisplayLink.h>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "Clock.h"
#include "LinkedList.h"
#include "OpenGLContextLock.h"


BEGIN_NAMESPACE_MW


	// forward declarations
    class Datum;
	class Stimulus;
	class StimulusNode;
	class StimulusDisplay;
    class OpenGLContextManager;
    class VariableCallbackNotification;
	
	/**
	 * StimulusDisplay represents a single abstracted stimulus display.
	 * The idea is that after initialization, you can use routines in here
	 * to set up a display chain, configure high-level changes in the 
	 * OpenGL context, control the drawing to the display.
	 *
	 * All of the details of actually creating OpenGL contexts (i.e. platform
	 * specific stuff) is handled under the hood by the OpenGLContextManager.
	 * This includes actual Cocoa or AGL calls to create, attach, and configure
	 * the contexts.  OpenGLContextManager will handle the details of doing its
	 * best in terms of producing an optimally accelerated and meeting the color
	 * demands of the user (also full-screen AA).  Functions will be provided for 
	 * reporting back exactly what StimulusDisplay is getting.  Eventually, 
	 * a ContextViewer utility will display the status and stats for every
	 * context, and possibly allow users to create and change them. 
	 */
		
  
	
	class StimulusDisplay : public boost::enable_shared_from_this<StimulusDisplay>, boost::noncopyable {
    protected:
        std::vector<int> context_ids;
		int current_context_index;
		shared_ptr< LinkedList<StimulusNode> > display_stack;
        
        shared_ptr<Clock> clock;
        shared_ptr<OpenGLContextManager> opengl_context_manager;
		
		boost::mutex display_lock;
        typedef boost::mutex::scoped_lock unique_lock;
        boost::condition_variable refreshCond;
        bool waitingForRefresh;

        bool needDraw;
		
		GLdouble left, right, top, bottom; // display bounds
        GLclampf backgroundRed, backgroundGreen, backgroundBlue;  // background color
        
        shared_ptr<VariableCallbackNotification> stateSystemNotification;
        std::vector<CVDisplayLinkRef> displayLinks;
        using DisplayLinkContext = std::pair<StimulusDisplay *, int>;
        std::vector<std::unique_ptr<DisplayLinkContext>> displayLinkContexts;
        bool displayLinksRunning;
        double mainDisplayRefreshRate;
        int64_t lastFrameTime;
        MWTime currentOutputTimeUS;
        
        const bool announceIndividualStimuli;
        bool announceStimuliOnImplicitUpdates;
        std::vector< shared_ptr<StimulusNode> > stimsToAnnounce;
        std::vector<Datum> stimAnnouncements;
        
        GLuint framebuffer;
        GLuint renderbuffer;
        std::map<int, GLint> bufferWidths, bufferHeights;
        
        void setMainDisplayRefreshRate();
        void allocateBufferStorage();
        void drawStoredBuffer(int contextIndex) const;
		
        void glInit();
		void setDisplayBounds();
        void refreshMainDisplay();
        void refreshMirrorDisplay(int contextIndex) const;
        void drawDisplayStack();
        void ensureRefresh(unique_lock &lock);

        void announceDisplayUpdate(bool updateIsExplicit);
        void announceDisplayStack(MWTime time);
        Datum getAnnounceData(bool updateIsExplicit);
        bool shouldAnnounceStimuli(bool updateIsExplicit) { return updateIsExplicit || announceStimuliOnImplicitUpdates; }

        void stateSystemCallback(const Datum &data, MWorksTime time);
        static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                            const CVTimeStamp *now,
                                            const CVTimeStamp *outputTime,
                                            CVOptionFlags flagsIn,
                                            CVOptionFlags *flagsOut,
                                            void *_context);
		
    public:
        static void getDisplayBounds(const Datum &mainScreenInfo,
                                     GLdouble &left,
                                     GLdouble &right,
                                     GLdouble &bottom,
                                     GLdouble &top);
		
		explicit StimulusDisplay(bool announceIndividualStimuli);
		~StimulusDisplay();
        
//        virtual void initialize();
		
		void addContext(int _context_id);
		
        // These are meant to be used by stimulus classes that need to manage per-context GL resources.
        // They should *not* be used internally by StimulusDisplay.
        int getNContexts() { return (context_ids.empty() ? 0 : 1); }
		OpenGLContextLock setCurrent(int i);	
		int getCurrentContextIndex() { return current_context_index; }
        
        void getCurrentViewportSize(GLint &width, GLint &height);
		
        shared_ptr<StimulusNode> addStimulus(shared_ptr<Stimulus> stim);
		void addStimulusNode(shared_ptr<StimulusNode> stimnode);
		
        void setBackgroundColor(GLclampf red, GLclampf green, GLclampf blue);
        void setAnnounceStimuliOnImplicitUpdates(bool announceStimuliOnImplicitUpdates);
		MWTime updateDisplay();
		void clearDisplay();
        void getDisplayBounds(GLdouble &left, GLdouble &right, GLdouble &bottom, GLdouble &top);
        double getMainDisplayRefreshRate() const { return mainDisplayRefreshRate; }
        MWTime getCurrentOutputTimeUS() const { return currentOutputTimeUS; }
        
        static shared_ptr<StimulusDisplay> getCurrentStimulusDisplay();
		
		
        // Delegated methods for transformations
        // Default implementations are for orthographic display
        virtual void translate2D(double x_deg, double y_deg);	
        virtual void rotateInPlane2D(double rot_angle_deg);
        virtual void scale2D(double x_size_deg, double y_size_deg);  
        
        // Stimulus transformation in 
        virtual void translate2D_screenUnits(double x, double y);
        virtual void scale2D_screenUnits(double x_size, double y_size);
                
	  private:
        GLdouble degrees_per_screen_unit;
//        StimulusDisplay(const StimulusDisplay& s) { }
        void operator=(const StimulusDisplay& l) { }
    };


    class VirtualTangentScreenDisplay : public StimulusDisplay {

      protected:
        GLdouble screen_width, screen_height, screen_distance, screen_radius,
                 screen_aspect_ratio;
        GLdouble x_offset_screen_units, y_offset_screen_units;
        GLdouble fov_y_deg, fov_x_deg;
        GLdouble near_clip_distance, far_clip_distance;

        virtual void glInit();
		virtual void setDisplayBounds();

      public:
    
        VirtualTangentScreenDisplay(bool announceIndividualStimuli);
        virtual ~VirtualTangentScreenDisplay();

        virtual void translate2D(double x_deg, double y_deg);	
        virtual void rotateInPlane2D(double rot_angle_deg);
        virtual void scale2D(double x_size_deg, double y_size_deg);        
        
        virtual void translate2D_screenUnits(double x, double y);
        virtual void scale2D_screenUnits(double x_size, double y_size);
        
	  private:
//      VirtualTangentScreenDisplay(const VirtualTangentScreenDisplay& s);
        void operator=(const VirtualTangentScreenDisplay& l) { }
    };
    


END_NAMESPACE_MW

#endif


