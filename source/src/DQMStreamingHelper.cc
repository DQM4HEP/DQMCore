  /// \file DQMStreamingHelper.cc
/*
 *
 * DQMStreamingHelper.cc source template automatically generated by a class generator
 * Creation date : lun. ao�t 17 2015
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
#include "dqm4hep/DQMStreamingHelper.h"
#include "dqm4hep/DQMMonitorElement.h"

namespace dqm4hep
{

const std::string DQMKey::MODULE_NAME = "MODULE_NAME";
const std::string DQMKey::DETECTOR_NAME = "DETECTOR_NAME";
const std::string DQMKey::ME_NAME = "ME_NAME";
const std::string DQMKey::ME_PATH = "ME_PATH";
const std::string DQMKey::ME_TYPE = "ME_TYPE";
const std::string DQMKey::ME_DESCRIPTION = "ME_DESCRIPTION";
const std::string DQMKey::SYSTEM_NAME = "SYSTEM_NAME";
const std::string DQMKey::NODE_NAME = "NODE_NAME";
const std::string DQMKey::RELEASE = "RELEASE";
const std::string DQMKey::VERSION = "VERSION";
const std::string DQMKey::MACHINE = "MACHINE";
const std::string DQMKey::HOST_NAME = "HOST_NAME";

//-------------------------------------------------------------------------------------------------

xdrstream::Status DQMStreamingHelper::read( xdrstream::IODevice *pDevice, DQMPublication &publication )
{
	uint32_t mapSize = 0;
	XDR_STREAM( pDevice->read( & mapSize ) );

	for(uint32_t i=0 ; i<mapSize ; i++)
	{
		std::string moduleName;
		XDR_STREAM( pDevice->read( & moduleName ) );

		uint32_t meListSize = 0;
		XDR_STREAM( pDevice->read( & meListSize ) );

		DQMPublication::value_type value(moduleName, DQMMonitorElementPtrList());
		DQMPublication::iterator iter = publication.insert(value).first;

		for(unsigned int i=0 ; i<meListSize ; i++)
		{
			DQMMonitorElementPtr monitorElement = std::make_shared<DQMMonitorElement>();

			if( xdrstream::XDR_SUCCESS != monitorElement->stream( xdrstream::XDR_READ_STREAM , pDevice ) )
				continue;

			iter->second.push_back(monitorElement);
		}
	}

	return xdrstream::XDR_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

xdrstream::Status DQMStreamingHelper::write( xdrstream::IODevice *pDevice, const DQMPublication &publication )
{
	uint32_t mapSize = publication.size();
	XDR_STREAM( pDevice->write( & mapSize ) );

	for(DQMPublication::const_iterator iter = publication.begin(), endIter = publication.end() ;
		endIter != iter ; ++iter)
	{
		std::string moduleName(iter->first);
		DQMMonitorElementPtrList meList(iter->second);
		uint32_t nMonitorElements = meList.size();

		XDR_STREAM( pDevice->write( & moduleName ) );

		xdrstream::xdr_size_t position = pDevice->getPosition();
		XDR_STREAM( pDevice->write( & nMonitorElements ) );

		uint32_t nWrittenMonitorElements = 0;

		for(unsigned int i=0 ; i<meList.size() ; i++)
		{
			DQMMonitorElementPtr &monitorElement = meList.at(i);

			xdrstream::xdr_size_t mePosition = pDevice->getPosition();

			if( xdrstream::XDR_SUCCESS != monitorElement->stream( xdrstream::XDR_WRITE_STREAM , pDevice ) )
			{
				pDevice->seek(mePosition);
				continue;
			}

			nWrittenMonitorElements++;
		}

		if(nWrittenMonitorElements != nMonitorElements)
		{
			xdrstream::xdr_size_t endPosition = pDevice->getPosition();
			pDevice->seek(position);
			XDR_STREAM( pDevice->write( & nWrittenMonitorElements ) )
			pDevice->seek(endPosition);
		}
	}

	return xdrstream::XDR_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

xdrstream::Status DQMStreamingHelper::read( xdrstream::IODevice *pDevice , DQMMonitorElementInfoList &infoList )
{
	dqm_uint nElements = 0;
	XDR_STREAM( pDevice->read( & nElements ) );

	for( uint32_t i=0 ; i<nElements ; i++ )
	{
		DQMMonitorElementInfo info;
		XDR_STREAM( DQMStreamingHelper::read( pDevice , info ) )

		infoList.push_back( info );
	}

	return xdrstream::XDR_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

xdrstream::Status DQMStreamingHelper::write( xdrstream::IODevice *pDevice , const DQMMonitorElementInfoList &infoList )
{
	dqm_uint nElements = infoList.size();
	XDR_STREAM( pDevice->write( & nElements ) );

	for( uint32_t i=0 ; i<nElements ; i++ )
	{
		XDR_STREAM( DQMStreamingHelper::write( pDevice , infoList.at(i) ) )
	}

	return xdrstream::XDR_SUCCESS;
}

} 
