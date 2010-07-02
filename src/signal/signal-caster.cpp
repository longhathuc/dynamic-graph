/*
 * SignalCaster.cpp
 *
 *  Created on: Jun 14, 2010
 *      Author: François Bleibel, Nicolas Mansard
 */

#include <dynamic-graph/signal-caster.h>
#include <dynamic-graph/dynamic-graph-api.h>
#include <exception>
#include <boost/lambda/bind.hpp>
#include <string>
#include <sstream>
#include <algorithm>
#include <dynamic-graph/exception-signal.h>

using namespace std;
using namespace boost;

namespace dynamicgraph {

SignalCaster::SignalCaster() {
	// nothing to initialize
}

SignalCaster::~SignalCaster() {
	// no special cleanup to do
}

void SignalCaster::registerCast(const type_info& type, SignalCaster::displayer_type displayer,
		SignalCaster::caster_type caster, SignalCaster::tracer_type tracer) {
	if ( existsCast(type) )
		throw ExceptionSignal(ExceptionSignal::GENERIC); //TODO: throw "cast already registered for type" exception
	functions_[type.name()] = cast_functions_type(displayer,caster, tracer);
}

void SignalCaster::unregisterCast(const std::type_info& type) {
	size_t n = functions_.erase(type.name());
	if ( 0 == n ) // erase did not find element
		throw ExceptionSignal(ExceptionSignal::GENERIC); // TODO: throw Cast not registered exception
}

bool SignalCaster::existsCast(const type_info& type) {
	return functions_.find(type.name()) != functions_.end();
}

void SignalCaster::disp(const any& object, ostream& os) {
	const char* type_name = object.type().name();
	map<string, cast_functions_type>::iterator it =
			functions_.find(type_name);

	if ( it == functions_.end() )
		throw ExceptionSignal(ExceptionSignal::BAD_CAST, "bad cast");
	//TODO: throw "cast not registered" exception
	(*it).second.get<0>()(object, os); // call display function (tuple index 0)
}

void SignalCaster::trace(const any& object, ostream& os) {
	const char* type_name = object.type().name();
	map<string, cast_functions_type>::iterator it =
			functions_.find(type_name);
	if ( it == functions_.end() )
		throw ExceptionSignal(ExceptionSignal::BAD_CAST, "bad cast");;
	//TODO: throw "cast not registered" exception
	(*it).second.get<2>()(object, os); // call trace function (tuple index 2)
}

any SignalCaster::cast(const type_info& type, istringstream& iss) {
	const char* type_name = type.name();
	map<string, cast_functions_type>::iterator it =	functions_.find(type_name);

	if ( it == functions_.end() )
		throw ExceptionSignal(ExceptionSignal::BAD_CAST, "bad cast");
	//TODO: throw "cast not registered" exception
	return (*it).second.get<1>()(iss); // call cast function (tuple index 1)
}


/// The global instance of the caster class.
SignalCaster g_caster;

/// Registers useful casts
namespace {
	DefaultCastRegisterer<double> double_reg;
	DefaultCastRegisterer<int> int_reg;
	DefaultCastRegisterer<unsigned int> uint_reg;
}

} // namespace dynamicgraph

