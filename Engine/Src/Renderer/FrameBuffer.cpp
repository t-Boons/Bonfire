#include "FrameBuffer.h"
#include "Core/Core.h"

namespace Bonfire
{
	FrameBuffer::FrameBuffer(const FrameBufferProperties& properties)
	{
		SetProperties(properties);
	}

	FrameBuffer::~FrameBuffer()
	{
		// Delete objects.
		glDeleteFramebuffers(1, &m_RenderID);
		glDeleteTextures(1, &m_ColorAttachmentID);
		glDeleteTextures(1, &m_DepthAttachmentID);
	}

	void FrameBuffer::Bind()
	{
		// Bind frame buffer object.
		glBindFramebuffer(GL_FRAMEBUFFER, m_RenderID);
	}

	void FrameBuffer::BindColorAttachment(uint32_t index)
	{
		// Set active textureslot.
		glActiveTexture(GL_TEXTURE0 + index);

		// Bind color texture.
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentID);
	}

	void FrameBuffer::BindDepthAttachment(uint32_t index)
	{
		// Set active textureslot.
		glActiveTexture(GL_TEXTURE0 + index);

		// Bind color texture.
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentID);
	}

	void FrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::SetProperties(const FrameBufferProperties& properties)
	{
		// Delete old frame buffer and texture objects if framebuffer was already instantiated.
		if (m_RenderID)
		{
			glDeleteFramebuffers(1, &m_RenderID);
			glDeleteTextures(1, &m_ColorAttachmentID);
			glDeleteTextures(1, &m_DepthAttachmentID);
		}

		// Generate frame buffer object and set its properties.
		glCreateFramebuffers(1, &m_RenderID);

		Bind();

		BF_ASSERT((properties.Width != 0), "Texture width can not be 0.");
		BF_ASSERT((properties.Height != 0), "Texture height can not be 0.");

		// Generate color texture and bind it.
		glGenTextures(1, &m_ColorAttachmentID);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentID);

		// Write data to the color texture.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, properties.Width, properties.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		// Set texture properties.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Set color texture to frame buffer color attachment.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentID, 0);
		
		// Generate color texture and bind it.
		glGenTextures(1, &m_DepthAttachmentID);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentID);

		// Write data to the depth texture.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, properties.Width, properties.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float borderColor[] = { 1,1,1,1 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		// Set depth texture to frame buffer depth attachment.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentID, 0);

		BF_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Framebuffer is not complete.")

		UnBind();
	}
}