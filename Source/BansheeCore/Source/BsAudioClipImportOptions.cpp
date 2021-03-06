//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsAudioClipImportOptions.h"
#include "BsAudioClipImportOptionsRTTI.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* AudioClipImportOptions::getRTTIStatic()
	{
		return AudioClipImportOptionsRTTI::instance();
	}

	RTTITypeBase* AudioClipImportOptions::getRTTI() const
	{
		return getRTTIStatic();
	}
}