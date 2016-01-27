/*
 *
 * DQMMonitorElementSender.cc source template automatically generated by a class generator
 * Creation date : sam. mai 9 2015
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
#include "dqm4hep/DQMMonitorElementSender.h"
#include "dqm4hep/DQMMessaging.h"
#include "dqm4hep/DQMDataStream.h"

// -- dim headers
#include "dic.hxx"

#include <sys/time.h>

namespace dqm4hep
{

DQMMonitorElementSender::DQMMonitorElementSender() :
		m_collectorName("DEFAULT"),
		m_dataStream(4*1024*1024), // 4 Mo to start
		m_timerValue(0.f),
		m_pSendMeTimerService(0)
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

DQMMonitorElementSender::~DQMMonitorElementSender()
{
	if(m_pSendMeTimerService)
		delete m_pSendMeTimerService;
}

//-------------------------------------------------------------------------------------------------

void DQMMonitorElementSender::setCollectorName(const std::string &collectorName)
{
	if(collectorName.empty())
		return;

	m_collectorName = collectorName;
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMMonitorElementSender::sendMonitorElements(const std::string &moduleName, const DQMMonitorElementList &monitorElementList)
{
	struct timeval tpstart;
	gettimeofday(&tpstart, NULL);

	if(moduleName.empty() || monitorElementList.empty())
		return STATUS_CODE_INVALID_PARAMETER;

	if(!m_pSendMeTimerService)
		m_pSendMeTimerService = new DQMPerformanceService(moduleName + "/SEND_ME_TO_COLLECTOR", m_timerValue);

	DQMMonitorElementPublication publication;
	publication.m_publication[moduleName] = monitorElementList;

	// stream the whole publication
	m_dataStream.reset();
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, publication.serialize(&m_dataStream));

	// and send the buffer
	dqm_char *pBuffer = m_dataStream.getBuffer();
	dqm_uint bufferSize = m_dataStream.getBufferSize();

	std::string commandName = "DQM4HEP/MonitorElementCollector/" + m_collectorName + "/";
	commandName += "MONITOR_ELEMENT_PACKET_RECEPTION";

	DimClient::sendCommandNB(commandName.c_str(), (void *)pBuffer, bufferSize);

	struct timeval tpend;
	gettimeofday(&tpend, NULL);
	m_timerValue = (tpend.tv_sec - tpstart.tv_sec) * (uint64_t)1000 + (tpend.tv_usec - tpstart.tv_usec)/ 1000.f;
	m_pSendMeTimerService->updateService(m_timerValue);

	return STATUS_CODE_SUCCESS;
}

} 

