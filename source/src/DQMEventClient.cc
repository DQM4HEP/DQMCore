  /// \file DQMEventClient.cc
/*
 *
 * DQMEventClient.cc source template automatically generated by a class generator
 * Creation date : mar. sept. 8 2015
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

// -- dqm4hep headers
#include "dqm4hep/DQMEventClient.h"
#include "dqm4hep/DQMEventStreamer.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMLogging.h"

namespace dqm4hep
{

DQMEventClient::DQMEventClient() :
		m_pEventStreamer(NULL),
		m_maximumQueueSize(100),
		m_eventStreamerOwner(true)
{
	pthread_mutex_init(&m_mutex, NULL);
}

//-------------------------------------------------------------------------------------------------

DQMEventClient::~DQMEventClient()
{
	this->clearQueue();

	if( m_eventStreamerOwner && NULL != m_pEventStreamer )
		delete m_pEventStreamer;

	pthread_mutex_destroy(&m_mutex);
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::setCollectorName(const std::string &collectorName)
{
	// if connected to service, can't change collector name
	if( this->isConnectedToService() )
		return;

	m_collectorName = collectorName;
}

//-------------------------------------------------------------------------------------------------

const std::string &DQMEventClient::getCollectorName() const
{
	return m_collectorName;
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::setEventStreamer(DQMEventStreamer *pEventStreamer, bool owner)
{
	if( m_eventStreamerOwner && NULL != m_pEventStreamer )
		delete m_pEventStreamer;

	m_pEventStreamer = pEventStreamer;
	m_eventStreamerOwner = owner;
}

//-------------------------------------------------------------------------------------------------

DQMEventStreamer *DQMEventClient::getEventStreamer() const
{
	return m_pEventStreamer;
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::setMaximumQueueSize(unsigned int maxQueueSize)
{
	if( 0 == maxQueueSize )
		return;

	m_maximumQueueSize = maxQueueSize;

	scoped_lock( & this->m_mutex);

	// shrink the queue to fit the new max queue size
	while( m_eventQueue.size() >  m_maximumQueueSize )
		m_eventQueue.pop();
}

//-------------------------------------------------------------------------------------------------

unsigned int DQMEventClient::getMaximumQueueSize() const
{
	return m_maximumQueueSize;
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::clearQueue()
{
	scoped_lock( & this->m_mutex );

	while( ! m_eventQueue.empty() )
		m_eventQueue.pop();
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::setSubEventIdentifier(const std::string &identifier)
{
	m_subEventIdentifier = identifier;
}

//-------------------------------------------------------------------------------------------------

const std::string &DQMEventClient::getSubEventIdentifier() const
{
	return m_subEventIdentifier;
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::takeEvent(DQMEventPtr &event)
{
	scoped_lock( & this->m_mutex );

	if( ! m_eventQueue.empty() )
	{
		event = m_eventQueue.front();
		m_eventQueue.pop();
	}
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::addListener(DQMEventClientListener *pListener)
{
	scoped_lock( & this->m_mutex);

	if(NULL == pListener)
		return;

	m_listeners.insert(pListener);
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::removeListener(DQMEventClientListener *pListener)
{
	scoped_lock( & this->m_mutex);

	if(NULL == pListener)
		return;

	m_listeners.erase(pListener);
}

//-------------------------------------------------------------------------------------------------

void DQMEventClient::pushEvent(DQMEvent *pEvent)
{
	if(NULL == pEvent)
		return;

	pthread_mutex_lock(& this->m_mutex);

	if( m_eventQueue.size() == m_maximumQueueSize )
		m_eventQueue.pop();

	DQMEventPtr event(pEvent);
	m_eventQueue.push(event);

	// need unlock before notifying
	pthread_mutex_unlock(& this->m_mutex );

	for(std::set<DQMEventClientListener*>::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
			endIter != iter ; ++iter)
		(*iter)->eventPushed(this);
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMEventClient::connectToService()
{
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->performServiceConnection());

	for(std::set<DQMEventClientListener*>::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
			endIter != iter ; ++iter)
		(*iter)->onEventClientConnnect(this);

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMEventClient::disconnectFromService()
{
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->performServiceDisconnection());

	for(std::set<DQMEventClientListener*>::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
			endIter != iter ; ++iter)
		(*iter)->onEventClientDisconnnect(this);

	return STATUS_CODE_SUCCESS;
}

}

