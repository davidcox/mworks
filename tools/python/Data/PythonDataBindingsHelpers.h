/*
 *  PythonDataBindingsHelpers.h
 *
 *  Created by David Cox on 12/8/09.
 *  Copyright 2009 Harvard University. All rights reserved.
 *
 */

#ifndef PYTHON_DATA_BINDINGS_HELPERS_H_
#define PYTHON_DATA_BINDINGS_HELPERS_H_

#include <Scarab/scarab.h>
#include <Scarab/scarab_utilities.h>
#include <boost/python.hpp>
#include <dfindex/dfindex.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

using namespace boost;
namespace bp = boost::python;


BEGIN_NAMESPACE_MW


class PythonDataFile : boost::noncopyable {
    std::string file_name;
    shared_ptr<dfindex> indexer;
    shared_ptr<DataFileIndexer::EventsIterator> eventsIterator;
    
    
public:
    
    PythonDataFile(std::string _file_name);    
    
    void open();    
    void close();    
    
    bool exists();
    
    bool loaded();    
    bool valid();
    
    std::string file();
    std::string true_mwk_file();
    
    MWTime minimum_time();
    MWTime maximum_time();
    
    void select_events(bp::list codes, const MWTime lower_bound, const MWTime upper_bound);
    EventWrapper get_next_event();
    std::vector<EventWrapper> get_events();
};



// A simple, non-indexing stream (e.g. file / socket) reader/writer for 
// python wrapping
class PythonDataStream : boost::noncopyable {
    
public:
    static void createFile(const std::string &filename);
    
    PythonDataStream(const std::string &uri);
    ~PythonDataStream();
    
    void open();
    void close();
    
    boost::python::object read();
    void write(const boost::python::object &obj);
	
    EventWrapper read_event();
    void write_event(const EventWrapper &e);
    
private:
    void requireValidSession() const;
    
    Datum readDatum();
    void writeDatum(const Datum &datum);
    
    const std::string uri;
    ScarabSession *session;

};    


boost::python::object extract_event_value(const EventWrapper &e);


END_NAMESPACE_MW


#endif /* !defined(PYTHON_DATA_BINDINGS_HELPERS_H_) */


























