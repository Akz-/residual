/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/mutex.h"
#include "audio/mixer.h"
#include "audio/audiostream.h"
#include "audio/decoders/mp3.h"
#include "engines/grim/debug.h"
#include "engines/grim/resource.h"
#include "engines/grim/textsplit.h"
#include "engines/grim/emi/sound/mp3track.h"

namespace Grim {

void MP3Track::parseRIFFHeader(Common::SeekableReadStream *data) {
	uint32 tag = data->readUint32BE();
	if (tag == MKTAG('R','I','F','F')) {
		_endFlag = false;
		data->seek(18, SEEK_CUR);
		_channels = data->readByte();
		data->readByte();
		_freq = data->readUint32LE();
		data->seek(6, SEEK_CUR);
		_bits = data->readByte();
		data->seek(5, SEEK_CUR);
		_regionLength = data->readUint32LE();
		_headerSize = 44;
	} else {
		error("Unknown file header");
	}
}

MP3Track::MP3Track(Audio::Mixer::SoundType soundType) {
	_soundType = soundType;
	_headerSize = 0;
	_regionLength = 0;
	_freq = 0;
	_bits = 0,
	_channels = 0;
	_endFlag = false;
}

MP3Track::~MP3Track() {
	stop();
	delete _handle;
}

bool MP3Track::openSound(const Common::String &soundName, Common::SeekableReadStream *file) {
	if (!file) {
		Debug::debug(Debug::Sound, "Stream for %s not open", soundName.c_str());
		return false;
	}
	_soundName = soundName;

	Common::String jmmName(_soundName.c_str(), _soundName.size() - 4);
	jmmName += ".jmm";
	Common::SeekableReadStream *jmmStream = g_resourceloader->openNewStreamFile("Textures/spago/" + jmmName);
	float loopStart = 0.0f, loopEnd = 0.0f;
	float startTime = 0.0f;
	if (jmmStream) {
		TextSplitter ts(jmmName, jmmStream);
		while (!ts.isEof()) {
			if (ts.checkString(".start"))
				ts.scanString(".start %f", 1, &startTime);
			if (ts.checkString(".jump"))
				ts.scanString(".jump %f %f", 2, &loopEnd, &loopStart);
			ts.nextLine();
		}
	}

	Audio::Timestamp start(loopStart);
	Audio::Timestamp end(loopEnd);

#ifndef USE_MAD
	warning("Cannot open %s, MP3 support not enabled", soundName.c_str());
	return true;
#else
	parseRIFFHeader(file);
	
	Audio::AudioStream *loop = Audio::makeLoopingAudioStream(Audio::makeMP3Stream(file, DisposeAfterUse::YES), start, end, 0);
	Audio::SeekableAudioStream *intro = new Audio::SubSeekableAudioStream(Audio::makeMP3Stream(file, DisposeAfterUse::NO), Audio::Timestamp(startTime), start, DisposeAfterUse::YES);

	Audio::QueuingAudioStream *queueStream = Audio::makeQueuingAudioStream(intro->getRate(), intro->isStereo());

	queueStream->queueAudioStream(intro);
	queueStream->queueAudioStream(loop);
	
	_stream = queueStream;
	_handle = new Audio::SoundHandle();
	return true;
#endif
}

bool MP3Track::hasLooped() {
	if (!_stream)
		return false;
	Audio::LoopingAudioStream *las = static_cast<Audio::LoopingAudioStream*>(_stream);
	return las->getCompleteIterations() > 0;
}

bool MP3Track::isPlaying() {
	if (!_handle)
		return false;

	return g_system->getMixer()->isSoundHandleActive(*_handle);
}

} // end of namespace Grim
