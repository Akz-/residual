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
	float loopStartMs = 0.0f, loopEndMs = 0.0f;
	float startTimeMs = 0.0f;
	if (jmmStream) {
		TextSplitter ts(jmmName, jmmStream);
		while (!ts.isEof()) {
			if (ts.checkString(".start"))
				ts.scanString(".start %f", 1, &startTimeMs);
			if (ts.checkString(".jump"))
				ts.scanString(".jump %f %f", 2, &loopEndMs, &loopStartMs);
			ts.nextLine();
		}
	}

	Audio::Timestamp start(startTimeMs / 1000, ((int)startTimeMs * 1000) % 1000000, 1000000);
	Audio::Timestamp loopStart(loopStartMs / 1000, ((int)loopStartMs * 1000) % 1000000, 1000000);
	Audio::Timestamp loopEnd(loopEndMs / 1000, ((int)loopEndMs * 1000) % 1000000, 1000000);

	if (loopEnd <= loopStart)
		warning("oops");

	loopStart = loopStart - start;
	loopEnd = loopEnd - start;

#ifndef USE_MAD
	warning("Cannot open %s, MP3 support not enabled", soundName.c_str());
	return true;
#else
	parseRIFFHeader(file);
	
	Audio::SeekableAudioStream *mp3Stream = Audio::makeMP3Stream(file, DisposeAfterUse::YES);
	mp3Stream = new Audio::SubSeekableAudioStream(mp3Stream, start, mp3Stream->getLength());

	if (loopEnd <= loopStart)
		_stream = mp3Stream;
	else
		_stream = new Audio::SubLoopingAudioStream(mp3Stream, 0, loopStart, loopEnd);
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
