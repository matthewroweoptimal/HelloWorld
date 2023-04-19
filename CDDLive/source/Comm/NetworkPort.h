/*
 * NetworkPort.h
 *
 *  Created on: Oct 1, 2015
 *      Author: Kris.Simonsen
 */

#ifndef _NETWORKPORT_H_
#define _NETWORKPORT_H_

#include <MandolinPort.h>

namespace oly {

class NetworkPort: public MandolinPort
{

_queue_id 		config_qid;

public:
	virtual void Run(int nMs);

	NetworkPort();
	void WriteMessage(mandolin_message * pMsg, bool bRequireConnection=false);	//	set bRequireConnection if mandolin connection process must have completed to send this message

	virtual void ConfigureMeters(int nAutoMetersListId, int nAutoMetersRate, int nAutoMetersFlags, bool bAutoMetersNoAck);
};

} /* namespace oly */




#endif /* _NETWORKPORT_H_ */
