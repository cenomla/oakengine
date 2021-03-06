#version 450 core

/**
 \file reconstruct.glsl
 \author Morgan McGuire, NVIDIA Research

 Routines for reconstructing linear Z, camera-space position, and camera-space face normals from a standard
 or infinite OpenGL projection matrix from G3D.
 */

// Note that positions (which may affect z) are snapped during rasterization, but 
// attributes are not.

/* 
 Clipping plane constants for use by reconstructZ

 clipInfo = (z_f == -inf()) ? Vector3(z_n, -1.0f, 1.0f) : Vector3(z_n * z_f,  z_n - z_f,  z_f);
*/
uniform vec3 clipInfo;

float reconstructCSZ(float d) {
	return clipInfo[0] / (clipInfo[1] * d + clipInfo[2]);
}

/**
  Open Source under the "BSD" license: http://www.opensource.org/licenses/bsd-license.php

  Copyright (c) 2011-2012, NVIDIA
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
layout (binding = 2) uniform sampler2D DEPTH_AND_STENCIL_buffer;

layout (location = 0) out float o_result;

void main() {
	o_result = reconstructCSZ(texelFetch(DEPTH_AND_STENCIL_buffer, ivec2(gl_FragCoord.xy), 0).r);
}