/*
 * RearPanelUI.h
 *
 *  Created on: Jan 6, 2016
 *      Author: kris.simonsen
 */

#ifndef _CDDLMEMBRANEUI_H_
#define _CDDLMEMBRANEUI_H_

extern "C" {
#include "oly.h"
}


typedef enum
{
	OLY_UI_ARRAY,
	OLY_UI_FR,
	OLY_UI_SUBWOOFER
} oly_ui_mode;

namespace oly {

class Config;

class cddlMembraneUI {

protected:
	oly_ui_mode 	UI_Mode;
	Config 		* pConfig;

public:

	cddlMembraneUI(Config *);
	void SW2();
	void SW2_long();
	void SW2_long_long();
	void UpdateUser(uint32_t instance);
	uint32_t cddlRemapUser(uint32_t instance);
	void UpdateInputSource(oly_audio_source_t source);
};

} /* namespace oly */

#endif /* _CDDLMEMBRANEUI_H_ */
