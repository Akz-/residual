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

#include "common/endian.h"
#include "common/foreach.h"
#include "engines/grim/debug.h"
#include "engines/grim/grim.h"
#include "engines/grim/material.h"
#include "engines/grim/gfx_base.h"
#include "engines/grim/resource.h"
#include "engines/grim/set.h"
#include "engines/grim/emi/costumeemi.h"
#include "engines/grim/emi/modelemi.h"
#include "engines/grim/emi/animationemi.h"
#include "engines/grim/emi/skeleton.h"

namespace Grim {

struct Vector3int {
	int _x;
	int _y;
	int _z;
	void setVal(int x, int y, int z) {
		_x = x; _y = y; _z = z;
	}
};

struct BoneInfo {
	int _incFac;
	int _joint;
	float _weight;
};

Common::String readLAString(Common::ReadStream *ms) {
	int strLength = ms->readUint32LE();
	char *readString = new char[strLength];
	ms->read(readString, strLength);

	Common::String retVal(readString);
	delete[] readString;

	return retVal;
}

void EMIMeshFace::loadFace(Common::SeekableReadStream *data) {
	_flags = data->readUint32LE();
	_hasTexture = data->readUint32LE();

	if (_hasTexture)
		_texID = data->readUint32LE();
	_faceLength = data->readUint32LE();
	_faceLength = _faceLength / 3;
	int x = 0, y = 0, z = 0;
	_indexes = new Vector3int[_faceLength];
	int j = 0;
	for (uint32 i = 0; i < _faceLength; i ++) {
		// FIXME: Are these ever going to be < 0 ?
		if (g_grim->getGamePlatform() == Common::kPlatformPS2) {
			x = data->readSint32LE();
			y = data->readSint32LE();
			z = data->readSint32LE();
		} else {
			x = data->readSint16LE();
			y = data->readSint16LE();
			z = data->readSint16LE();
		}
		_indexes[j++].setVal(x, y, z);
	}
}

EMIMeshFace::~EMIMeshFace() {
	delete[] _indexes;
}

void EMIModel::setTex(uint32 index) {
	if (index < _numTextures && _mats[index])
		_mats[index]->select();
}

void EMIModel::loadMesh(Common::SeekableReadStream *data) {
	//int strLength = 0; // Usefull for PS2-strings

	Common::String nameString = readLAString(data);

	char f[4];
	data->read(f, 4);
	_radius = get_float(f);
	_center->readFromStream(data);

	_boxData->readFromStream(data);
	_boxData2->readFromStream(data);

	_numTexSets = data->readUint32LE();
	_setType = data->readUint32LE();
	_numTextures = data->readUint32LE();

	_texNames = new Common::String[_numTextures];

	for (uint32 i = 0; i < _numTextures; i++) {
		_texNames[i] = readLAString(data);
		// Every texname seems to be followed by 4 0-bytes (Ref mk1.mesh,
		// this is intentional)
		data->skip(4);
	}

	prepareTextures();

	int type = data->readUint32LE();
	// Check that it is one of the known types
	//3  is no texture vertecies
	//18 is no normals
	//19 is regular
	assert(type == 19 || type == 18 || type == 3);

	_numVertices = data->readUint32LE();

	_lighting = new Math::Vector3d[_numVertices];

	// Vertices
	_vertices = new Math::Vector3d[_numVertices];
	_drawVertices = new Math::Vector3d[_numVertices];
	for (int i = 0; i < _numVertices; i++) {
		_vertices[i].readFromStream(data);
		_drawVertices[i] = _vertices[i];
	}
	_normals = new Math::Vector3d[_numVertices];
	_drawNormals = new Math::Vector3d[_numVertices];
	if (type != 18) {
		for (int i = 0; i < _numVertices; i++) {
			_normals[i].readFromStream(data);
			_drawNormals[i] = _normals[i];
		}
	}
	_colorMap = new EMIColormap[_numVertices];
	for (int i = 0; i < _numVertices; ++i) {
		_colorMap[i].r = data->readByte();
		_colorMap[i].g = data->readByte();
		_colorMap[i].b = data->readByte();
		_colorMap[i].a = data->readByte();
	}
	if (type != 3) {
		_texVerts = new Math::Vector2d[_numVertices];
		for (int i = 0; i < _numVertices; i++) {
			_texVerts[i].readFromStream(data);
		}
	}
	// Faces

	_numFaces = data->readUint32LE();
	if (data->eos()) {
		_numFaces = 0;
		_faces = nullptr;
		return;
	}

	_faces = new EMIMeshFace[_numFaces];

	for (uint32 j = 0; j < _numFaces; j++) {
		_faces[j].setParent(this);
		_faces[j].loadFace(data);
	}

	int hasBones = data->readUint32LE();

	if (hasBones == 1) {
		_numBones = data->readUint32LE();
		_boneNames = new Common::String[_numBones];
		for (int i = 0; i < _numBones; i++) {
			_boneNames[i] = readLAString(data);
		}

		_numBoneInfos =  data->readUint32LE();
		_boneInfos = new BoneInfo[_numBoneInfos];

		for (int i = 0; i < _numBoneInfos; i++) {
			_boneInfos[i]._incFac = data->readUint32LE();
			_boneInfos[i]._joint = data->readUint32LE();
			_boneInfos[i]._weight = data->readUint32LE();
		}
	} else {
		_numBones = 0;
		_numBoneInfos = 0;
	}
	prepareForRender();
}

void EMIModel::setSkeleton(Skeleton *skel) {
	if (_skeleton == skel) {
		return;
	}
	_skeleton = skel;
	if (!skel || !_numBoneInfos) {
		return;
	}
	int boneVert = 0;
	delete[] _vertexBoneInfo; _vertexBoneInfo = nullptr;
	delete[] _vertexBone; _vertexBone = nullptr;
	_vertexBoneInfo = new int[_numBoneInfos];
	_vertexBone = new int[_numBoneInfos]; // Oversized, but yeah.

	for (int i = 0; i < _numBoneInfos; i++) {
		_vertexBoneInfo[i] = _skeleton->findJointIndex(_boneNames[_boneInfos[i]._joint], _skeleton->_numJoints);

		if (_boneInfos[i]._incFac == 1) {
			_vertexBone[boneVert] = i;
			boneVert++;
		}
	}

	Math::Vector3d vertex;
	Math::Matrix4 mat;
	for (int i = 0; i < _numVertices; i++) {
		vertex = _vertices[i];
		if (_vertexBoneInfo[_vertexBone[i]] != -1) {
			mat = _skeleton->_joints[_vertexBoneInfo[_vertexBone[i]]]._absMatrix;
			mat.inverseTranslate(&vertex);
			mat.inverseRotate(&vertex);
		}
		_vertices[i] = vertex;
	}
}

void EMIModel::prepareForRender() {
	if (!_skeleton || !_vertexBoneInfo)
		return;
	for (int i = 0; i < _numVertices; i++) {
		_drawVertices[i] = _vertices[i];
		_drawNormals[i] = _normals[i];
		int animIndex = _vertexBoneInfo[_vertexBone[i]];
		_skeleton->_joints[animIndex]._finalMatrix.transform(_drawVertices + i, true);

		Math::Matrix4 inv = _skeleton->_joints[animIndex]._absMatrix;
		inv.invertAffineOrthonormal();
		Math::Matrix4 anim = _skeleton->_joints[animIndex]._finalMatrix * inv;

		Math::Matrix4 normalMatrix = anim;
		normalMatrix.invertAffineOrthonormal();
		normalMatrix.transpose();
		
		normalMatrix.transform(_drawNormals + i, false);
	}
	g_driver->updateEMIModel(this);
}

void EMIModel::prepareTextures() {
	_mats = new Material*[_numTextures];
	for (uint32 i = 0; i < _numTextures; i++) {
		// HACK: As we dont know what specialty-textures are yet, we skip loading them
		if (!_texNames[i].contains("specialty"))
			_mats[i] = _costume->loadMaterial(_texNames[i]);
		else
			_mats[i] = g_driver->getSpecialtyTexture(_texNames[i][9] - '0');
	}
}

void EMIModel::draw(const Math::Matrix4 &matrix) {
	prepareForRender();

	Math::AABB bounds = calculateWorldBounds(matrix);
	if (bounds.isValid() && !g_grim->getCurrSet()->getFrustum().isInside(bounds))
		return;

	updateLighting(matrix);
	// We will need to add a call to the skeleton, to get the modified vertices, but for now,
	// I'll be happy with just static drawing
	for (uint32 i = 0; i < _numFaces; i++) {
		setTex(_faces[i]._texID);
		g_driver->drawEMIModelFace(this, &_faces[i]);
	}
}

void EMIModel::updateLighting(const Math::Matrix4 &matrix) {
	Set *set = g_grim->getCurrSet();

	Math::Matrix4 normalMatrix = matrix;
	normalMatrix.invertAffineOrthonormal();
	normalMatrix.transpose();

	for (int i = 0; i < _numVertices; i++) {
		_lighting[i].set(0.0f, 0.0f, 0.0f);
	}

	for (int i = 0; i < _numVertices; i++) {
		Math::Vector3d normal = _drawNormals[i];
		Math::Vector3d vertex = _drawVertices[i];
		matrix.transform(&vertex, true);
		normalMatrix.transform(&normal, false);

		foreach(Light *l, set->getLights()) {
			if (!l->_enabled)
				continue;

			// Note: Spot lights currently unimplemented.
			if (l->_type == Light::Spot)
				continue;

			float light = l->_intensity;
			Math::Vector3d dir;

			if (l->_type == Light::Direct) {
				dir.set(0, 0, -1);
				Math::Matrix4 r = l->_quat.toMatrix();
				r.transform(&dir, false);
			} else {
				dir = l->_pos - vertex;
			}

			if (l->_type != Light::Ambient) {
				float dot = MAX(0.0f, normal.dotProduct(dir.getNormalized()));
				light *= dot;

				if (l->_type != Light::Direct) {
					float dist = dir.getMagnitude();
					if (dist > l->_falloffFar)
						continue;

					float attn = MIN(1.0f, 1.0f - (dist - l->_falloffNear) / (l->_falloffFar - l->_falloffNear));
					light *= attn;
				}
			}

			light = MIN(1.0f, light);

			Math::Vector3d color;
			color.x() = l->_color.getRed() / 255.0f;
			color.y() = l->_color.getGreen() / 255.0f;
			color.z() = l->_color.getBlue() / 255.0f;

			Math::Vector3d &result = _lighting[i];
			result += color * light;
			result.x() = MIN(1.0f, result.x());
			result.y() = MIN(1.0f, result.y());
			result.z() = MIN(1.0f, result.z());
		}
	}
}

void EMIModel::getBoundingBox(int *x1, int *y1, int *x2, int *y2) const {
	int winX1, winY1, winX2, winY2;
	g_driver->getBoundingBoxPos(this, &winX1, &winY1, &winX2, &winY2);
	if (winX1 != -1 && winY1 != -1 && winX2 != -1 && winY2 != -1) {
		*x1 = MIN(*x1, winX1);
		*y1 = MIN(*y1, winY1);
		*x2 = MAX(*x2, winX2);
		*y2 = MAX(*y2, winY2);
	}
}

Math::AABB EMIModel::calculateWorldBounds(const Math::Matrix4 &matrix) const {
	Math::AABB bounds;
	for (int i = 0; i < _numVertices; i++) {
		bounds.expand(_drawVertices[i]);
	}
	bounds.transform(matrix);
	return bounds;
}

EMIModel::EMIModel(const Common::String &filename, Common::SeekableReadStream *data, EMICostume *costume) :
		_fname(filename), _costume(costume) {
	_numVertices = 0;
	_vertices = nullptr;
	_drawVertices = nullptr;
	_normals = nullptr;
	_colorMap = nullptr;
	_texVerts = nullptr;
	_numFaces = 0;
	_faces = nullptr;
	_numTextures = 0;
	_texNames = nullptr;
	_mats = nullptr;
	_numBones = 0;
	_boneInfos = nullptr;
	_numBoneInfos = 0;
	_vertexBoneInfo = nullptr;
	_vertexBone = nullptr;
	_skeleton = nullptr;
	_radius = 0;
	_center = new Math::Vector3d();
	_boxData = new Math::Vector3d();
	_boxData2 = new Math::Vector3d();
	_numTexSets = 0;
	_setType = 0;
	_boneNames = nullptr;
	_lighting = nullptr;

	loadMesh(data);
	g_driver->createEMIModel(this);
}

EMIModel::~EMIModel() {
	delete[] _vertices;
	delete[] _drawVertices;
	delete[] _normals;
	delete[] _colorMap;
	delete[] _texVerts;
	delete[] _faces;
	delete[] _texNames;
	delete[] _mats;
	delete[] _boneInfos;
	delete[] _vertexBone;
	delete[] _vertexBoneInfo;
	delete[] _boneNames;
	delete[] _lighting;
	delete _center;
	delete _boxData;
	delete _boxData2;
}

} // end of namespace Grim
