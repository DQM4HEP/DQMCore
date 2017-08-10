/*
 *
 * DQMEventCollectorImp.h header template automatically generated by a class generator
 * Creation date : mer. sept. 9 2015
 *
 * This file is part of DQM4HEP libraries.
 * 
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef DQMEVENTCOLLECTORIMP_H
#define DQMEVENTCOLLECTORIMP_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"

namespace dqm4hep
{

/** DQMEventCollectorImp class
 */
class DQMEventCollectorImp
{
public:
	/** Destructor
	 */
	virtual ~DQMEventCollectorImp() {}

	/** Set the collector name
	 */
	virtual StatusCode setCollectorName(const std::string &collectorName) = 0;

	/** Get the collector name
	 */
	virtual const std::string &getCollectorName() const = 0;

	/** Whether the collector server is running
	 */
	virtual bool isRunning() const = 0;

	/** Start the collector server
	 */
	virtual StatusCode startCollector() = 0;

	/** Stop the collector server
	 */
	virtual StatusCode stopCollector() = 0;

	/** Set the event streamer to serialize/deserialize the in/out-coming events
	 */
	virtual void setEventStreamer(DQMEventStreamer *pEventStreamer) = 0;

	/** Get the event streamer
	 */
	virtual DQMEventStreamer *getEventStreamer() const = 0;
}; 

} 

#endif  //  DQMEVENTCOLLECTORIMP_H