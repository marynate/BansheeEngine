/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _Texture_H__
#define _Texture_H__

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmHardwareBuffer.h"
#include "CmPixelUtil.h"
#include "CmTextureView.h"

namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Resources
	*  @{
	*/
	/** Enum identifying the texture usage
    */
    enum TextureUsage
    {
		/// @copydoc HardwareBuffer::Usage
		TU_STATIC = GBU_STATIC, // Optimal setting if texture is read by the GPU often, and very rarely written by CPU
		TU_DYNAMIC = GBU_DYNAMIC, // Optimal if the texture is updated by CPU often (e.g. every frame)
		TU_RENDERTARGET = 0x200, // Used for rendering by the GPU
		TU_DEPTHSTENCIL = 0x400,
		TU_DEFAULT = TU_STATIC
    };

    /** Enum identifying the texture type
    */
    enum TextureType
    {
        /// 1D texture, used in combination with 1D texture coordinates
        TEX_TYPE_1D = 1,
        /// 2D texture, used in combination with 2D texture coordinates (default)
        TEX_TYPE_2D = 2,
        /// 3D volume texture, used in combination with 3D texture coordinates
        TEX_TYPE_3D = 3,
        /// 3D cube map, used in combination with 3D texture coordinates
        TEX_TYPE_CUBE_MAP = 4
    };

	/** Enum identifying special mipmap numbers
    */
	enum TextureMipmap
	{
		/// Generate mipmaps up to 1x1
		MIP_UNLIMITED = 0x7FFFFFFF
	};

    /** Abstract class representing a Texture resource.
        @remarks
            The actual concrete subclass which will exist for a texture
            is dependent on the rendering system in use (Direct3D, OpenGL etc).
            This class represents the commonalities, and is the one 'used'
            by programmers even though the real implementation could be
            different in reality. Texture objects are created through
            the 'create' method of the TextureManager concrete subclass.
     */
    class CM_EXPORT Texture : public Resource
    {
    public:
        /** Gets the type of texture 
        */
        virtual TextureType getTextureType(void) const { return mTextureType; }

        /** Gets the number of mipmaps to be used for this texture.
        */
        virtual UINT32 getNumMipmaps(void) const {return mNumMipmaps;}

		/** Gets whether this texture will be set up so that on sampling it, 
		hardware gamma correction is applied.
		*/
		virtual bool isHardwareGammaEnabled() const { return mHwGamma; }

		/** Get the level of multisample AA to be used if this texture is a 
		rendertarget.
		*/
		virtual UINT32 getFSAA() const { return mFSAA; }

		/** Get the multisample AA hint if this texture is a rendertarget.
		*/
		virtual const String& getFSAAHint() const { return mFSAAHint; }

		/** Returns the height of the texture.
        */
        virtual UINT32 getHeight(void) const { return mHeight; }

        /** Returns the width of the texture.
        */
        virtual UINT32 getWidth(void) const { return mWidth; }

        /** Returns the depth of the texture (only applicable for 3D textures).
        */
        virtual UINT32 getDepth(void) const { return mDepth; }

        /** Returns the TextureUsage indentifier for this Texture
        */
        virtual int getUsage() const { return mUsage; }

		/** Returns the pixel format for the texture surface. */
		virtual PixelFormat getFormat() const { return mFormat; }

        /** Returns true if the texture has an alpha layer. */
        virtual bool hasAlpha(void) const;

        /** Return the number of faces this texture has. This will be 6 for a cubemap
        	texture and 1 for a 1D, 2D or 3D one.
        */
        virtual UINT32 getNumFaces() const;

		/**
		 * @brief	Sets raw texture pixels for the specified mip level and texture face. Pixel format
		 * 			must match the format of the texture.
		 * 			
		 * @note	Not-async. This operation will block the current thread until the render thread
		 *			executes the command.
		 */
		void setRawPixels(const PixelData& data, UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Sets raw texture pixels for the specified mip level and texture face. Pixel format
		 * 			must match the format of the texture. Returns immediately
		 * 			but the texture won't be updated until the command
		 * 			executes on the render thread.
		 * 			
		 * @see		Texture::setRawPixels		
		 */
		void setRawPixels_async(const PixelData& data, UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Internal version of Texture::setRawPixels. Only callable
		 * 			from the render thread.
		 *
		 * @see		Texture::setRawPixels
		 */
		virtual void setRawPixels_internal(const PixelData& data, UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Gets raw pixels from the texture. This is a slow operation
		 * 			as it will read data from the GPU. If the texture is compressed
		 * 			the returned data will be contain compressed pixels as well.
		 * 			
		 * @note	Not-async. This operation will block the current thread until the render thread
		 *			executes the command.
		 */
		PixelDataPtr getRawPixels(UINT32 face = 0, UINT32 mip = 0);
		

		/**
		 * @brief	Async version of Texture::getRawPixels. Returns immediately
		 * 			but you won't have access to the pixel data until the command
		 * 			executes on the render thread.
		 *
		 * @see		Texture::getRawPixels
		 */
		AsyncOp getRawPixels_async(UINT32 face = 0, UINT32 mip = 0);

		/**
		 * @brief	Internal version of Texture::getRawPixels. Only callable
		 * 			from the render thread.
		 *
		 * @see		Texture::getRawPixels
		 */
		virtual void getRawPixels_internal(UINT32 face, UINT32 mip, AsyncOp& op);

		PixelData lock(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);
		void unlock();

		/** Copies the contents of this texture to
			another texture. */
		void copy(TexturePtr& target);

		/************************************************************************/
		/* 								TEXTURE VIEW                      		*/
		/************************************************************************/

		static TextureViewPtr requestView(TexturePtr texture, UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices, GpuViewUsage usage);
		static void releaseView(TextureViewPtr view);

	protected:

		virtual TextureViewPtr createView();
		void clearBufferViews();

		struct TextureViewReference
		{
			TextureViewReference(TextureViewPtr _view)
				:view(_view), refCount(0)
			{ }

			TextureViewPtr view;
			UINT32 refCount;
		};

		std::unordered_map<TEXTURE_VIEW_DESC, TextureViewReference*, TextureView::HashFunction, TextureView::EqualFunction> mTextureViews;

    protected:
		friend class TextureManager;

        UINT32 mHeight;
        UINT32 mWidth;
        UINT32 mDepth;

		UINT32 mNumMipmaps;
		bool mHwGamma;
		UINT32 mFSAA;
		String mFSAAHint;

        TextureType mTextureType;
		PixelFormat mFormat;
        int mUsage; // Bit field, so this can't be TextureUsage

		Texture();

		/**
		 * @brief	Initializes the texture. This must be called right after the texture is constructed. Called by TextureManager
		 * 			upon texture creation, so usually you don't want to call this manually.
		 * 			
		 * @note	Initialization is not done immediately, and is instead just scheduled on the
		 * 			render thread. Unless called from render thread, in which case it is initialized
		 * 			right away.
		 */
		void initialize(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps, 
			PixelFormat format, int usage, bool hwGamma, UINT32 fsaa, const String& fsaaHint);

		virtual PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0) = 0;
		virtual void unlockImpl() = 0;

		virtual void copyImpl(TexturePtr& target) = 0;

		/// @copydoc Resource::calculateSize
		UINT32 calculateSize(void) const;

		void throwIfNotRenderThread() const;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class TextureRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

		/************************************************************************/
		/* 								STATICS		                     		*/
		/************************************************************************/
	public:
		static HTexture create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
			int num_mips, PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);

		static HTexture create(TextureType texType, UINT32 width, UINT32 height, int num_mips,
			PixelFormat format, int usage = TU_DEFAULT,
			bool hwGammaCorrection = false, UINT32 fsaa = 0, const String& fsaaHint = StringUtil::BLANK);
    };
	/** @} */
}

#endif